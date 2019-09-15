#pragma once

#include <gtkmm/button.h>
#include <gtkmm/buttonbox.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/paned.h>
#include <gtkmm/separator.h>
#include <gtkmm/window.h>
#include "common.h"

DL_NS_BEGIN

class DockItem;

class launcher_buttons_box : public Gtk::Frame
{
  public:
    launcher_buttons_box(bool horizontal, const Glib::ustring& title, gint spacing,
                         Gtk::ButtonBoxStyle layout);

    Gtk::Button m_button_createLauncher;
    Gtk::Button m_button_testLauncher;
    Gtk::Button m_button_cancel;
};

class launcher_window : public Gtk::Window
{
  public:
    launcher_window();
    ~launcher_window();
    void init(const shared_ptr<DockItem> dockitem);

  private:
    shared_ptr<DockItem> m_dockitem;

    bool m_test_launch = false;
    // Signal handlers:
    void on_button_file_clicked();
    void on_button_close_clicked();
    void on_button_createLauncher_clicked();
    void on_button_testLauncher_clicked();
    void on_button_categories_link_clicked();
    void invalid_entry_message();
    bool is_valid_command();
    bool on_delete_event(GdkEventAny* event);

    // Child widgets:
    Gtk::Grid m_grid;
    Gtk::Box m_vbox;
    Gtk::Paned m_vpaned;
    Gtk::Box m_vbox_center;
    Gtk::ButtonBox m_button_box;
    Gtk::Label m_label_file;
    Gtk::Button m_button_file;
    Gtk::Button m_button_create;
    Gtk::Button m_button_test;
    Gtk::Button m_button_categories_link;
    Gtk::Label m_label_info;
    Gtk::Label m_label_categories;
    Gtk::Button m_button_close;
    Gtk::Frame m_frame_top;
    Gtk::Frame m_frame;
    Gtk::Box m_hbox;
    Gtk::Box m_hbox2;
    Gtk::Entry m_entry_file;
    Gtk::Entry m_entry_categories;
};

DL_NS_END

