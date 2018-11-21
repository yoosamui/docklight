//*****************************************************************
//
//  Copyright (C) 2015-2017 Juan R. Gonzalez
//  Created on February 21, 2017 
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
// 
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************

#ifndef SESSIONWINDOW_H
#define	SESSIONWINDOW_H

#define WNCK_I_KNOW_THIS_IS_UNSTABLE  1

#include "Utilities.h"
#include <libwnck/libwnck.h>

#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include <gtkmm/grid.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/grid.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/image.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/listbox.h>
#include "IconLoader.h"
#include <gdkmm/pixbufloader.h>

class DockPanel;
class DockItem;

#define DEF_BUTTON_ADDTOLIST 3
#define DEF_BUTTON_OK 4
#define DEF_BUTTON_CANCEL 5

#define DEF_PIXBUFFER_MAX 1024 << 3
#define DEF_APPBUFFER_MAX 66

struct sessionGrpData
{
    guint8 pixbuff[ DEF_PIXBUFFER_MAX] = {0};
    char appname[DEF_APPBUFFER_MAX] = {'\0'};
    char parameters[MAX_INPUT] = {'\0'};
    char titlename[MAX_INPUT] = {'\0'};
};

class ListRow : public Gtk::ListBoxRow
{
public:
    ListRow(const Glib::ustring& appname,
            const Glib::ustring& parameters,
            const Glib::ustring& titlename,
            Glib::RefPtr<Gdk::Pixbuf> appIcon);

    Glib::ustring get_appname() const
    {
        return m_appname.get_text();
    }

    Glib::ustring get_parameters() const
    {
        return m_parameters.get_text();
    }

    Glib::ustring get_titlename() const
    {
        return m_titlename.get_text();
    }

    Glib::RefPtr<Gdk::Pixbuf> get_pixbuf()
    {
        return m_image.get_pixbuf();
    }
protected:
    void on_launch_button_clicked(ListRow& row);
    void on_remove_button_clicked(ListRow& row);
private:
    Gtk::Button m_launchButton;
    Gtk::Button m_removeButton;
    Gtk::Image m_image;
    Gtk::Grid m_grid;
    Gtk::Frame m_frame;
    Gtk::Entry m_parameters;
    Gtk::Box m_HBox;
    Gtk::Box m_VBox;
    Gtk::Box m_HSeparator;
    Gtk::Label m_appname;
    Gtk::Label m_titlename;
};

class SessionWindow : public Gtk::Window
{
public:

    SessionWindow();
    ~SessionWindow();
    void init(DockPanel& panel, DockItem* dockitem, const int id);

protected:
    // Signal handlers.
    bool on_delete_event(GdkEventAny* event);
    void on_button_clicked(guint buttonId);

    static void on_active_window_changed_callback(WnckScreen *screen,
            WnckWindow *previously_active_window, gpointer user_data);

    static WnckWindow* m_activeWindow;
private:
    //signal accessor:
    typedef sigc::signal<void, WnckWindow*> type_signal_getactive;
    type_signal_getactive signal_getactive();
    static type_signal_getactive m_signal_getactive;
    void on_signal_getactive(WnckWindow* window);


    Glib::ustring getFilePath();
    Glib::ustring getTitleFilePath();
    void save();
    void addToList();

    WnckWindow* m_window = nullptr;
    DockPanel* m_panel;
    DockItem* m_dockitem;
    static bool m_deleteSet;
    std::string m_sessiongrpname;

    // Member data.
    Gtk::Grid m_grid;
    Gtk::Box m_VBox;
    Gtk::Box m_HBox;
    Gtk::Box m_HBoxBottom;
    Gtk::Label m_labelAddActive;
    Gtk::Entry m_EntryAppName;
    
    Gtk::Label m_labelSessionName;
    Gtk::Entry m_SessionName;
    Gtk::Button m_OkButton;
    Gtk::Button m_CancelButton;
    Gtk::Button m_AddToGroupButton;
    Gtk::ScrolledWindow m_ScrolledWindow;
    Gtk::ListBox m_ListBox;

};

#endif	/* SESSIONWINDOW_H */

