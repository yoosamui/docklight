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

    WnckWindow* PanelBase::m_active_window;
    DockMenu::DockMenu()
    {
        // Home menu
        m_home_menu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_home_menu_quit_item.set_label(MSG_QUIT);
        m_home_menu_close_all_item.set_label(MSG_CLOSE_ALL_WINDOWS);
        m_HomeMinimizeAllWindowsMenuItem.set_label(MSG_MINIMIZE_ALL_WINDOWS);
        m_HomeUnMinimizeAllWindowsMenuItem.set_label(MSG_MAXIMIZE_ALL_WINDOWS);

        m_home_menu.append(m_HomeMinimizeAllWindowsMenuItem);
        m_home_menu.append(m_HomeUnMinimizeAllWindowsMenuItem);
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

        m_item_menu.append(m_item_menu_attach);
        m_item_menu.append(m_separatorMenuItem1);
        m_item_menu.append(m_item_menu_unminimize_all);
        m_item_menu.append(m_item_menu_minimize_all);
        m_item_menu.append(m_item_menu_close_all);
        m_item_menu.append(m_separatorMenuItem0);
        m_item_menu.append(m_item_menu_new);

        m_item_menu.show_all();

        // events
        m_home_menu_quit_item.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_home_menu_quit_event));

        m_HomeMinimizeAllWindowsMenuItem.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_HomeMinimizeAllWindows_event));

        m_HomeUnMinimizeAllWindowsMenuItem.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_HomeUnMinimizeAllWindows_event));

        m_home_menu_close_all_item.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_HomeCloseAllWindows_event));

        m_item_menu_attach.set_active(false);
        m_item_menu_attach.signal_toggled().connect(
            sigc::mem_fun(*this, &DockMenu::on_item_menu_attach_event));

        m_item_menu_new.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_item_menu_new_event));

        m_item_menu_close_all.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_item_menu_close_group_event));

        m_item_menu_minimize_all.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_item_menu_minimize_all_event));

        m_item_menu_unminimize_all.signal_activate().connect(
            sigc::mem_fun(*this, &DockMenu::on_item_menu_unminimize_all_event));

        m_item_menu.signal_show().connect(sigc::mem_fun(*this, &DockMenu::on_menu_show_event));
        m_item_menu.signal_hide().connect(sigc::mem_fun(*this, &DockMenu::on_menu_hide_event));
    }

    void DockMenu::on_item_menu_attach_event()
    {
        bool attached = m_item_menu_attach.get_active();
        Provider()->attach(m_dockitem_index, attached);
    }

    void DockMenu::on_menu_show_event()
    {
        m_context_menu_active = true;
    }

    void DockMenu::on_menu_hide_event()
    {
        m_context_menu_active = false;
    }

    void DockMenu::on_HomeMinimizeAllWindows_event()
    {
        wnck::HomeMinimizeAll();
    }

    void DockMenu::on_HomeUnMinimizeAllWindows_event()
    {
        wnck::HomeUnMinimizeAll();
    }

    void DockMenu::on_HomeCloseAllWindows_event()
    {
        wnck::HomeCloseAllWindows();
    }

    void DockMenu::on_item_menu_minimize_all_event()
    {
        if (m_dockitem_index < 1) return;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!Provider()->get_dockitem_by_index(m_dockitem_index, dockitem)) return;

        for (auto& item : dockitem->get_childmap()) {
            auto window = item.second->get_wnckwindow();
            if (!window) continue;

            wnck::minimize(window);
        }
    }

    void DockMenu::on_item_menu_unminimize_all_event()
    {
        if (m_dockitem_index < 1) return;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!Provider()->get_dockitem_by_index(m_dockitem_index, dockitem)) return;

        for (auto& item : dockitem->get_childmap()) {
            auto window = item.second->get_wnckwindow();
            if (!window) continue;

            wnck::unminimize(window);
        }
    }

    void DockMenu::on_item_menu_close_group_event()
    {
        if (m_dockitem_index < 1) return;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!Provider()->get_dockitem_by_index(m_dockitem_index, dockitem)) return;

        for (auto& item : dockitem->get_childmap()) {
            auto window = item.second->get_wnckwindow();
            if (!window) continue;

            wnck::close_window(window);
        }
    }

    void DockMenu::on_item_menu_new_event()
    {
        if (m_dockitem_index < 1) return;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!Provider()->get_dockitem_by_index(m_dockitem_index, dockitem)) return;

        dockitem->launch();
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
