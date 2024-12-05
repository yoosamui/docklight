#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <glibmm/object.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/separatormenuitem.h>
#include "components/animboomwindow.h"
#include "utils/wnck.h"
#include "components/position.h"
// clang-format on

//#include "components/testwindow.h"
namespace docklight
{

    class PanelBase : public Gtk::DrawingArea
    {
      public:
      protected:
        Glib::RefPtr<Gtk::Application> m_app;

        static WnckWindow* m_active_window;

        bool m_context_menu_active = false;
        bool m_mouse_enter = false;
        bool m_drag_drop_starts = false;

        guint m_dockitem_index = 0;
        guint m_drag_drop_item_index = 0;
        guint m_drag_drop_item_move_index = 0;
        gulong m_dockitem_active_index = 0;
    };

    class DockMenu : public PanelBase
    {
        Glib::RefPtr<AnimBoomWindow> m_anim;

      public:
        DockMenu();

      protected:
        // Home menu
        Gtk::Menu m_home_menu;
        Gtk::MenuItem m_home_menu_quit_item;
        Gtk::MenuItem m_home_menu_close_all_item;
        Gtk::MenuItem m_HomeMinimizeAllWindowsMenuItem;
        Gtk::MenuItem m_HomeUnMinimizeAllWindowsMenuItem;

        // Item menu
        Gtk::Menu m_item_menu;
        Gtk::Menu* m_item_menu_windowlist = nullptr;

        Gtk::MenuItem m_item_menu_new;
        Gtk::CheckMenuItem m_item_menu_attach;
        Gtk::MenuItem m_item_menu_close_all;
        Gtk::MenuItem m_item_menu_minimize_all;
        Gtk::MenuItem m_item_menu_unminimize_all;

        Gtk::SeparatorMenuItem m_separatorMenuHome0;
        Gtk::SeparatorMenuItem m_separatorMenuHome1;

        Gtk::SeparatorMenuItem m_separatorMenuItem0;
        Gtk::SeparatorMenuItem m_separatorMenuItem1;

        // events

        virtual void on_home_menu_quit_event();
        virtual void on_item_menu_new_event();

        virtual void on_home_menu_position(int& x, int& y, bool& push_in);
        virtual void on_item_menu_position(int& x, int& y, bool& push_in);

        void on_HomeMinimizeAllWindows_event();
        void on_HomeUnMinimizeAllWindows_event();

        void on_item_menu_attach_event();
        void on_item_menu_close_group_event();
        void on_item_menu_minimize_all_event();
        void on_item_menu_unminimize_all_event();
        void on_HomeCloseAllWindows_event();
        void on_menu_show_event();
        void on_menu_hide_event();
    };

}  // namespace docklight
