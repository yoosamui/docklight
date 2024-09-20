#pragma once

#include <glibmm/main.h>
#include <libwnck/libwnck.h>

#include <memory>

template <typename T>
class Factory
{
  public:
    //  static std::unique_ptr<T> create() { return std::make_unique<T>(); }
    // static Glib::RefPtr<T> create(guint xid, Glib::ustring& instance_name, Glib::ustring
    // group_name, WnckWindowType wintype)
    //{
    ////  return Glib::RefPtr<T>(new DockItem(xid, instance_name, group_name, WnckWindowType
    ///wintype));
    //}
};

// static const auto dockitem =
//     Glib::RefPtr<DockItem>(new DockItem(xid, instance_name, group_name, WnckWindowType wintype));
