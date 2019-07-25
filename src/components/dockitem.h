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
    dock_item_type_t m_dock_item_type;
    unsigned long m_xid;
    Glib::RefPtr<Gdk::Pixbuf> m_image;
} appinfo_t;

class DockItem
{
  public:
    DockItem(appinfo_t appinfo,
             dock_item_type_t itemtype = dock_item_type_t::single);
    ~DockItem();
    string get_name() const;
    WnckWindow* get_wnckwindow() const;
    bool is_attached() const;
    int get_width() const;
    int get_height() const;
    void set_index(int index);
    void set_y(int y);
    void set_x(int x);
    void set_image(Glib::RefPtr<Gdk::Pixbuf> image);
    dock_item_type_t get_dock_item_type() const;
    string get_desktop_icon_name() const;
    const Glib::RefPtr<Gdk::Pixbuf> get_image();
    vector<shared_ptr<DockItem>> m_items;

  protected:
    appinfo_t m_app_info;
    bool m_attached = false;
    int m_width;
    int m_height;
    int m_y;
    int m_x;
    int m_index;
};

DL_NS_END

