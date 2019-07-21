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
    string m_desktop_icon_name;
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
    bool is_attached() const;
    unsigned int get_width() const;
    unsigned int get_height() const;
    void set_index(unsigned int index);
    void set_y(unsigned int y);
    void set_x(unsigned int x);
    void set_image(Glib::RefPtr<Gdk::Pixbuf> image);
    string get_desktop_icon_name() const;
    const Glib::RefPtr<Gdk::Pixbuf> get_image();
    vector<shared_ptr<DockItem>> m_items;

  protected:
    appinfo_t m_app_info;
    bool m_attached = false;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_y;
    unsigned int m_x;
    unsigned int m_index;
};

DL_NS_END

