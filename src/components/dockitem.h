#pragma once

#include <gdkmm/pixbuf.h>
#include "common.h"

DL_NS_BEGIN

typedef struct {
    string m_name;
    string m_instance;
    string m_group;
    string m_title;
    bool m_error = false;
    string m_desktop_file;
    string m_desktop_name;
    Glib::RefPtr<Gdk::Pixbuf> m_icon;
} appinfo_t;

class DockItem
{
  public:
    appinfo_t m_app_info;

  protected:
};

DL_NS_END

