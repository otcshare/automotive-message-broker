#ifndef SUPERPTR_H_
#define SUPERPTR_H_

#include <glib.h>
#include <gio/gio.h>

#include <memory>

namespace amb {

template<typename T> struct traits;

template<>
struct traits<GVariant> {
  struct delete_functor {
	void operator()(GVariant * p) const {
	  if (p != nullptr)
		g_variant_unref(p);
	}
  };
};

template<>
struct traits<GError> {
  struct delete_functor {
	void operator()(GError * p) const {
	  if (p != nullptr)
		g_error_free(p);
	}
  };
};

template<>
struct traits<GDBusProxy> {
  struct delete_functor {
	void operator()(GDBusProxy * p) const {
	  if (p != nullptr)
		g_object_unref(p);
	}
  };
};

template<>
struct traits<GVariantIter> {
  struct delete_functor {
	void operator()(GVariantIter * p) const {
	  if (p != nullptr)
		g_variant_iter_free(p);
	}
  };
};

template<>
struct traits<gchar> {
  struct delete_functor {
	void operator()(gchar * p) const {
	  if (p != nullptr)
		g_free(p);
	}
  };
};

template <>
struct traits<GDBusConnection> {
  struct delete_functor {
	void operator()(GDBusConnection* p) const {
	  if (p != nullptr)
		g_dbus_connection_close_sync(p, nullptr, nullptr);
	}
  };
};

template<typename T> using super_ptr =
		 ::std::unique_ptr<T, typename traits<T>::delete_functor>;

template<typename T> using gobject_ptr =
		 ::std::unique_ptr<T , std::function<void(T*)> >;

template<typename T> super_ptr<T> make_super(T* t)
{
  return super_ptr<T>(t);
}

template<typename T> ::std::unique_ptr<T> make_unique(T* t)
{
  return ::std::unique_ptr<T>(t);
}

template<typename T> gobject_ptr<T> make_gobject(T* t)
{
	return gobject_ptr<T>(t, [](T* ptr) { if(ptr) g_object_unref(ptr);});
}

}
#endif
