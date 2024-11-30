
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

#include "components/dockitemicon.h"

namespace docklight
{
    DockItemIcon::DockItemIcon(gulong xid, WnckWindow* window, const Glib::ustring& instance_name,
                               const Glib::ustring& group_name, guint wintype)
        : DockItem(xid, window, instance_name, group_name, wintype)
    {
    }

    std::shared_ptr<DockItemIcon> DockItemIcon::clone()
    {
        auto clone = std::shared_ptr<DockItemIcon>(
            new DockItemIcon(m_xid, nullptr, m_instance_name, m_group_name, m_wintype));

        clone->m_has_desktop_file = m_has_desktop_file;
        clone->m_attached = m_attached;
        clone->m_title = m_title;
        clone->m_window_name = m_window_name;
        clone->m_desktop_file = m_desktop_file;
        clone->m_icon_name = m_icon_name;
        clone->m_description = m_description;
        clone->m_icon = m_icon;
        clone->m_image = m_image;
        clone->m_wintype = m_wintype;
        clone->m_tag = m_tag;
        clone->m_width = m_width;
        clone->m_height = m_height;
        clone->m_attached = m_attached;
        clone->m_wnckwindow = m_wnckwindow;
        clone->m_hash = m_hash;
        clone->m_active_window = m_active_window;

        // covariant return type.
        return clone;
    }

    guint DockItemIcon::get_container_size()
    {
        return m_map.size();
    }

    void DockItemIcon::set_icon(Glib::RefPtr<Gdk::Pixbuf> icon)
    {
        g_assert(icon);

        m_width = icon->get_width();
        m_height = icon->get_height();
        m_icon = icon;
    }

    void DockItemIcon::add_child(std::shared_ptr<DockItemIcon> child)
    {
        m_map.insert({child->get_xid(), child});
    }

    const Glib::RefPtr<Gdk::Pixbuf>& DockItemIcon::get_icon() const
    {
        g_assert(m_icon);
        return m_icon;
    }

    const Glib::RefPtr<Gdk::Pixbuf> DockItemIcon::get_icon_from_window(guint size)
    {
        auto pixbuf =
            Glib::wrap(m_icon->gobj(), true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);

        if (WNCK_IS_WINDOW(m_wnckwindow)) {
            Glib::ustring current_iconname(wnck_window_get_icon_name(m_wnckwindow));

            if (current_iconname != m_icon_name) {
                auto gdkpixbuf = wnck_window_get_icon(m_wnckwindow);

                if (gdkpixbuf) {
                    pixbuf =
                        Glib::wrap(gdkpixbuf, true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);
                }
            }
        }

        g_assert(pixbuf);

        return pixbuf;
    }

    const Glib::RefPtr<Gdk::Pixbuf> DockItemIcon::get_icon(guint size)
    {
        auto pixbuf =
            Glib::wrap(m_icon->gobj(), true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);

        g_assert(pixbuf);

        m_width = size;
        m_height = size;

        return pixbuf;
    }

    const std::vector<gulong>& DockItemIcon::get_wnck_xid_list()
    {
        m_wnck_xid_list.clear();

        for (auto& item : m_map) {
            auto dockitem = item.second;
            gulong xid = dockitem->get_xid();

            m_wnck_xid_list.push_back(xid);
        }

        return m_wnck_xid_list;
    }

    const std::vector<WnckWindow*>& DockItemIcon::get_wnck_window_list()
    {
        m_wnck_window_list.clear();

        for (auto& item : m_map) {
            auto dockitem = item.second;
            WnckWindow* window = dockitem->get_wnckwindow();
            if (!window) continue;

            m_wnck_window_list.push_back(window);
        }

        return m_wnck_window_list;
    }

    const std::map<gulong, std::shared_ptr<DockItemIcon>>& DockItemIcon::get_childmap() const
    {
        return m_map;
    }

    int DockItemIcon::remove_child(gulong xid)
    {
        if (!m_map.count(xid)) return 0;
        return m_map.erase(xid);
    }

}  // namespace docklight
