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
    string m_comment;
    string m_lang;
    bool m_error = false;
    string m_desktop_file;
    string m_desktop_name;
    string m_desktop_icon_name;
    WnckWindow* m_wnckwindow;
    dock_item_type_t m_dock_item_type;
    unsigned long m_xid;
    Glib::RefPtr<Gdk::Pixbuf> m_image;
    int m_separator_length;
    int m_width;
    int m_height;
} appinfo_t;

class DockItem
{
  public:
    DockItem(appinfo_t appinfo, dock_item_type_t itemtype = dock_item_type_t::launcher);
    DockItem();
    ~DockItem();
    appinfo_t* get_appinfo();
    void set_attach(bool attach);
    string get_name() const;
    string get_title() const;
    string get_windowname() const;
    string get_desktop_filename() const;
    WnckWindow* get_wnckwindow() const;
    unsigned long get_xid() const;
    shared_ptr<DockItem> get_next() const;
    bool is_attached() const;
    int get_width();
    int get_height();
    void set_index(int index);
    int get_index() const;
    void set_y(int y);
    void set_x(int x);
    int get_y();
    int get_x();
    void set_image(Glib::RefPtr<Gdk::Pixbuf> image);
    dock_item_type_t get_dock_item_type() const;
    string get_desktop_icon_name() const;
    const Glib::RefPtr<Gdk::Pixbuf> get_image();
    vector<shared_ptr<DockItem>> m_items;

  private:
    void set_height(int value);
    void set_width(int value);
    appinfo_t m_app_info;
    bool m_attached = false;
    int m_y;
    int m_x;
    int m_index = 0;

    void swap_size();
};

DL_NS_END

