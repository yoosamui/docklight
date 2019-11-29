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
    string m_locale;
    bool m_error = false;
    string m_desktop_file;
    string m_icon_name;
    bool m_cache = false;
    WnckWindow* m_wnckwindow = nullptr;
    dock_item_type_t m_dock_item_type;
    unsigned long m_xid = 0;
    Glib::RefPtr<Gdk::Pixbuf> m_image = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
    int m_separator_length = 0;
    bool m_separartor_expands = true;
    int m_width;
    int m_height;
} appinfo_t;

class DockItem : public Glib::Object
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
    void set_desktop_filename(const string& desktop_file);
    WnckWindow* get_wnckwindow() const;
    unsigned long get_xid() const;
    shared_ptr<DockItem> get_next() const;
    bool is_attached() const;
    int get_width() const;
    int get_height() const;
    void set_expander_size(int size);
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

    int m_preview_frame_count = 0;
    bool m_preview_image_is_dynamic = true;
    Glib::RefPtr<Gdk::Pixbuf> m_preview_first_image = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;

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

