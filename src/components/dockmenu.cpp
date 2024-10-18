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
#include "components/dockmenu.h"
#include "translations.h"
// clang-format on

namespace docklight
{

    DockMenu::DockMenu()
    {
        // Home menu
        m_home_menu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_home_menu_quit_item.set_label(MSG_QUIT);
        m_home_menu_close_all_item.set_label(MSG_CLOSE_ALL);

        m_home_menu.append(m_home_menu_close_all_item);
        m_home_menu.append(m_separatorMenuHome1);
        m_home_menu.append(m_home_menu_quit_item);
        m_home_menu.show_all();

        // items menu
        m_item_menu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_item_menu_unminimize_all.set_label(MSG_SHOW_ALL);
        m_item_menu_minimize_all.set_label(MSG_MINIMIZE_ALL);
        m_item_menu_close_all.set_label(MSG_CLOSE_ALL);
        m_item_menu_new.set_label(MSG_OPEN_NEW);
        m_item_menu_attach.set_label(MSG_ATTACH);

        m_item_menu.append(m_item_menu_unminimize_all);
        m_item_menu.append(m_item_menu_minimize_all);
        m_item_menu.append(m_item_menu_close_all);
        m_item_menu.append(m_separatorMenuItem1);
        m_item_menu.append(m_item_menu_attach);
        m_item_menu.append(m_separatorMenuItem0);
        m_item_menu.append(m_item_menu_new);
        m_item_menu.show_all();

        // events
        m_home_menu_quit_item.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_home_menu_quit_event));
    }

    void DockMenu::on_home_menu_position(int& x, int& y, bool& push_in)
    {
        Position()->get_dockmenu_position(0, x, y, m_home_menu.get_width(),
                                          m_home_menu.get_height());
    }

    void DockMenu::on_item_menu_position(int& x, int& y, bool& push_in)
    {
        Position()->get_dockmenu_position(m_dockitem_index, x, y, m_item_menu.get_width(),
                                          m_item_menu.get_height());
    }
}  // namespace docklight
