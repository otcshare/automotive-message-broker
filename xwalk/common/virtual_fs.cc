// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "common/virtual_fs.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <pkgmgr-info.h>
#include <tzplatform_config.h>

#include <cassert>
#include <algorithm>
#include <sstream>
#include <string>

#include "common/extension.h"

namespace {

const char kInternalStorage[] = "internal";
const char kRemovableStorage[] = "removable";

const char kStorageTypeInternal[] = "INTERNAL";
const char kStorageTypeExternal[] = "EXTERNAL";
const char kStorageStateMounted[] = "MOUNTED";
const char kStorageStateRemoved[] = "REMOVED";
const char kStorageStateUnmountable[] = "UNMOUNTABLE";

}  // namespace

namespace vfs_const {

const unsigned kDefaultFileMode = 0755;
const char kLocationCamera[] = "camera";
const char kLocationMusic[] = "music";
const char kLocationImages[] = "images";
const char kLocationVideos[] = "videos";
const char kLocationDownloads[] = "downloads";
const char kLocationDocuments[] = "documents";
const char kLocationRingtones[] = "ringtones";
const char kLocationWgtPackage[] = "wgt-package";
const char kLocationWgtPrivate[] = "wgt-private";
const char kLocationWgtPrivateTmp[] = "wgt-private-tmp";

}  // namespace vfs_const

VirtualFS::VirtualFS() {
  std::string app_path = GetApplicationPath();
  if (!app_path.empty()) {
    AddInternalStorage(vfs_const::kLocationWgtPackage, app_path);
    AddInternalStorage(vfs_const::kLocationWgtPrivate, JoinPath(app_path, "private"));
    AddInternalStorage(vfs_const::kLocationWgtPrivateTmp, JoinPath(app_path, "tmp"));
  }

  AddInternalStorage(vfs_const::kLocationCamera, tzplatform_getenv(TZ_USER_CAMERA));
  AddInternalStorage(vfs_const::kLocationMusic, tzplatform_getenv(TZ_USER_SOUNDS));
  AddInternalStorage(vfs_const::kLocationImages, tzplatform_getenv(TZ_USER_IMAGES));
  AddInternalStorage(vfs_const::kLocationVideos, tzplatform_getenv(TZ_USER_VIDEOS));
  AddInternalStorage(vfs_const::kLocationDownloads, tzplatform_getenv(TZ_USER_DOWNLOADS));
  AddInternalStorage(vfs_const::kLocationDocuments, tzplatform_getenv(TZ_USER_DOCUMENTS));
  AddInternalStorage(vfs_const::kLocationRingtones,
      tzplatform_mkpath(TZ_USER_SHARE, "settings/Ringtones"));
  storage_changed_cb_ = NULL;
  cb_user_data_ = NULL;
}

VirtualFS::~VirtualFS() {
}

std::string VirtualFS::JoinPath(const std::string& one,
    const std::string& another) {
  return one + '/' + another;
}

bool VirtualFS::MakePath(const std::string& path, int mode) {
  // Path should start with '/' and contain at least 1 character after '/'.
  if (path.empty() || path[0] != '/' || path.length() < 2)
    return false;

  struct stat st;
  std::string dir = path;
  if (stat(dir.c_str(), &st) == 0)
    return true;

  // Add trailing '/' if missing, so we can iterate till the end of the path.
  if (dir[dir.size() - 1] != '/')
    dir += '/';

  for (std::string::iterator iter = dir.begin(); iter != dir.end();) {
    std::string::iterator cur_iter = std::find(iter, dir.end(), '/');

    // If '/' is found at the beginning of the string, iterate to the next '/'.
    if (cur_iter == iter) {
      ++iter;
      cur_iter = std::find(iter, dir.end(), '/');
    }

    std::string new_path = std::string(dir.begin(), cur_iter);

    // If path doesn't exist, try to create one and continue iteration.
    // In case of error, stop iteration and return.
    if (stat(new_path.c_str(), &st) != 0) {
      if (mkdir(new_path.c_str(), mode) != 0 && errno != EEXIST )
          return false;
    // If path exists and it is not a directory, stop iteration and return.
    } else if (!S_ISDIR(st.st_mode)) {
      return false;
    }

    // Advance iterator and create next parent folder.
    iter = cur_iter;
    if (cur_iter != dir.end())
      ++iter;
  }
  return true;
}

int VirtualFS::GetDirEntryCount(const char* path) {
  int count = 0;
  DIR* dir = opendir(path);
  if (!dir)
    return count;

  struct dirent entry;
  struct dirent *result;
  int ret = readdir_r(dir, &entry, &result);

  for (; ret == 0 && result != NULL; ret = readdir_r(dir, &entry, &result)) {
    if (entry.d_type == DT_REG || entry.d_type == DT_DIR)
      count++;
  }

  closedir(dir);
  return count;
}

std::string VirtualFS::GetAppId(const std::string& package_id) {
  char* appid = NULL;
  pkgmgrinfo_pkginfo_h pkginfo_handle;
  int ret = pkgmgrinfo_pkginfo_get_pkginfo(package_id.c_str(), &pkginfo_handle);
  if (ret != PMINFO_R_OK)
    return std::string();
  ret = pkgmgrinfo_pkginfo_get_mainappid(pkginfo_handle, &appid);
  if (ret != PMINFO_R_OK) {
    pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo_handle);
    return std::string();
  }

  std::string retval(appid);
  pkgmgrinfo_pkginfo_destroy_pkginfo(pkginfo_handle);
  return retval;
}

std::string VirtualFS::GetExecPath(const std::string& app_id) {
  char* exec_path = NULL;
  pkgmgrinfo_appinfo_h appinfo_handle;
  int ret = pkgmgrinfo_appinfo_get_appinfo(app_id.c_str(), &appinfo_handle);
  if (ret != PMINFO_R_OK)
    return std::string();
  ret = pkgmgrinfo_appinfo_get_exec(appinfo_handle, &exec_path);
  if (ret != PMINFO_R_OK) {
    pkgmgrinfo_appinfo_destroy_appinfo(appinfo_handle);
    return std::string();
  }

  std::string retval(exec_path);
  pkgmgrinfo_appinfo_destroy_appinfo(appinfo_handle);
  return retval;
}

bool VirtualFS::GetStorageByLabel(const std::string& label, Storage& storage) {
  storage_foreach_device_supported(OnStorageDeviceSupported, this);
  Storages::const_iterator it = storages_.find(label);

  if (it == storages_.end()) {
    return false;
  }
  storage = it->second;
  return true;
}

Storages::iterator VirtualFS::begin() {
  storage_foreach_device_supported(OnStorageDeviceSupported, this);
  return storages_.begin();
}

Storages::const_iterator VirtualFS::end() const {
  return storages_.end();
}

std::string VirtualFS::GetApplicationPath() {
  std::string id_str = common::Extension::GetRuntimeVariable("app_id", 64);
  std::string pkg_id = id_str.substr(1, id_str.rfind('"') - 1);
  if (pkg_id.empty())
    return std::string();
  std::string app_id = GetAppId(pkg_id);
  if (app_id.empty())
    return std::string();
  std::string exec_path = GetExecPath(app_id);
  if (exec_path.empty())
    return std::string();

  size_t index = exec_path.find(pkg_id);
  if (index != std::string::npos)
    return exec_path.substr(0, index + pkg_id.length());
  return std::string();
}

std::string VirtualFS::GetRealPath(const std::string& fullPath) const {
  std::size_t pos = fullPath.find_first_of('/');
  Storages::const_iterator it = storages_.find(fullPath.substr(0, pos));

  if (it == storages_.end())
    return std::string();

  if (pos != std::string::npos)
    return it->second.GetFullPath() + fullPath.substr(pos);

  return it->second.GetFullPath();
}

void VirtualFS::AddInternalStorage(
    const std::string& label, const std::string& path) {
  if (MakePath(path, vfs_const::kDefaultFileMode))
    storages_.insert(StorageLabelPair(label,
                                      Storage(-1,
                                      Storage::STORAGE_TYPE_INTERNAL,
                                      Storage::STORAGE_STATE_MOUNTED,
                                      path)));
}

void VirtualFS::AddStorage(int id,
                           storage_type_e type,
                           storage_state_e state,
                           const std::string& path) {
  std::string label;
  if (type == STORAGE_TYPE_INTERNAL)
    label = kInternalStorage + std::to_string(id);
  else if (type == STORAGE_TYPE_EXTERNAL)
    label = kRemovableStorage + std::to_string(id);

  storages_.insert(StorageLabelPair(label,
                                    Storage(id,
                                    type,
                                    state,
                                    path)));
  if (std::find(watched_storages_.begin(),
                watched_storages_.end(), id) != watched_storages_.end()) {
    watched_storages_.push_back(id);
    storage_set_state_changed_cb(id, OnStorageStateChanged, this);
  }
}

void VirtualFS::SetOnStorageChangedCb(CallBackFunctionPtr cb, void* user_data) {
  storage_changed_cb_ = cb;
  cb_user_data_ = user_data;
}

void VirtualFS::NotifyStorageStateChanged(int id, storage_state_e state) {
  for (Storages::iterator it = storages_.begin(); it != storages_.end(); ++it) {
    if (it->second.GetId() == id) {
      it->second.SetState(state);
      if (storage_changed_cb_) {
        storage_changed_cb_(it->first, it->second, cb_user_data_);
      }
      break;
    }
  }
}

bool VirtualFS::OnStorageDeviceSupported(
    int id, storage_type_e type, storage_state_e state,
    const char* path, void* user_data) {
  reinterpret_cast<VirtualFS*>(user_data)->AddStorage(
      id, type, state, path);
  return true;
}

void VirtualFS::OnStorageStateChanged(
    int id, storage_state_e state, void* user_data) {
  reinterpret_cast<VirtualFS*>(user_data)->NotifyStorageStateChanged(
      id, state);
}

/*
 * Storage Class
 */

Storage::Storage(
    int id, int type, int state, const std::string& fullpath)
    : id_(id),
      type_(type),
      state_(state),
      full_path_(fullpath) { }

std::string Storage::GetType() const {
  return (type_ == Storage::STORAGE_TYPE_INTERNAL) ? kStorageTypeInternal :
      kStorageTypeExternal;
}

std::string Storage::GetState() const {
  switch (state_) {
    case Storage::STORAGE_STATE_MOUNTED:
    case Storage::STORAGE_STATE_MOUNTED_READONLY:
      return kStorageStateMounted;
    case Storage::STORAGE_STATE_REMOVED:
      return kStorageStateRemoved;
    case Storage::STORAGE_STATE_UNMOUNTABLE:
      return kStorageStateUnmountable;
    default:
      assert(!"Not reached");
  }
  return std::string();
}
