#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>
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
    WnckWindow* m_wnckwindow;
    unsigned long m_xid;
    Glib::RefPtr<Gdk::Pixbuf> m_image;
} appinfo_t;

class DockItem
{
  public:
    DockItem(appinfo_t appinfo);
    ~DockItem();
    string get_name() const;
    WnckWindow* get_wnckwindow() const;
    vector<shared_ptr<DockItem>> m_items;

  protected:
    appinfo_t m_app_info;
};

DL_NS_END

