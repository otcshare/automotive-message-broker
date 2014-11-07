// Copyright (c) 2014 Intel Corporation. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMMON_VIRTUAL_FS_H_
#define COMMON_VIRTUAL_FS_H_

#include <appfw/app_storage.h>

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace vfs_const {

extern const unsigned kDefaultFileMode;
extern const char kLocationCamera[];
extern const char kLocationMusic[];
extern const char kLocationImages[];
extern const char kLocationVideos[];
extern const char kLocationDownloads[];
extern const char kLocationDocuments[];
extern const char kLocationRingtones[];
extern const char kLocationWgtPackage[];
extern const char kLocationWgtPrivate[];
extern const char kLocationWgtPrivateTmp[];

}  // namespace vfs_const

class Storage {
 public:
  /* Mapped to storage_type_e */
  enum StorageType {
    STORAGE_TYPE_INTERNAL, STORAGE_TYPE_EXTERNAL,
  };

  /* Mapped to storage_state_e */
  enum StorageState {
    STORAGE_STATE_UNMOUNTABLE = -2,
    STORAGE_STATE_REMOVED = -1,
    STORAGE_STATE_MOUNTED = 0,
    STORAGE_STATE_MOUNTED_READONLY = 1,
  };

  Storage(int id = 0, int type = 0, int state = 0,
      const std::string& fullpath = "");

  std::string GetType() const;
  std::string GetState() const;
  int GetId() const { return id_; }
  const std::string& GetFullPath() const { return full_path_; }
  void SetState(int state) { state_ = state; }

 private:
  int id_;
  int type_;
  int state_;
  std::string full_path_;
};

typedef std::map<std::string, Storage> Storages;
typedef void(*CallBackFunctionPtr)(const std::string&, Storage, void*);

/**
 * The VirtualFS class provide an abstraction of the TIZEN virtual filesystem.
 * It manages mounted storages and virtual roots, creating missing directories
 * if needed.
 * Convenient functions are also provided for working with paths (real or
 * virtual).
 */
class VirtualFS {
 public:
  VirtualFS();
  ~VirtualFS();
  /**
   * Resolve the given fullpath within the virtual filesystem to an absolute
   * path within the real filesystem.
   * @param fullPath: fully-qualified path of the form: <root name>/<path>
   *    where <rootname> is the name of the virtual root and <path> is the path
   *    to the file or directory relative to that root.
   * @return full Linux path.
   */
  std::string GetRealPath(const std::string& fullPath) const;
  bool GetStorageByLabel(const std::string& label, Storage& storage);
  Storages::iterator begin();
  Storages::const_iterator end() const;
  void SetOnStorageChangedCb(CallBackFunctionPtr cb, void* user_data);

  /**
   * Concatenate two paths.
   * @param one: base path.
   * @param another: path within 'one'.
   * @return new path.
   */
  static std::string JoinPath(const std::string& one,
      const std::string& another);
  /**
   * Create full path and parent directories when needed.
   * Similar to "mkdir -p".
   * @param path: the path to be created.
   * @param mode: the unix access mode applied to the new directories.
   * @return true if success.
   */
  static bool MakePath(const std::string& path, int mode);
  static int GetDirEntryCount(const char* path);
  static std::string GetAppId(const std::string& package_id);
  static std::string GetExecPath(const std::string& app_id);
  static std::string GetApplicationPath();

 private:
  void AddInternalStorage(const std::string& label, const std::string& path);
  void AddStorage(int storage, storage_type_e type, storage_state_e state,
      const std::string& path);
  void NotifyStorageStateChanged(int id, storage_state_e state);
  static bool OnStorageDeviceSupported(int id, storage_type_e type,
      storage_state_e state, const char* path, void* user_data);
  static void OnStorageStateChanged(int id, storage_state_e state,
      void* user_data);

  CallBackFunctionPtr storage_changed_cb_;
  void* cb_user_data_;

  typedef std::pair<std::string, Storage> StorageLabelPair;
  Storages storages_;
  std::vector<int> watched_storages_;
};

#endif  // COMMON_VIRTUAL_FS_H_
