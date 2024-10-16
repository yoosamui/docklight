
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
    DockItemIcon::DockItemIcon(gulong xid, const Glib::ustring& instance_name,
                               const Glib::ustring& group_name, guint wintype
                               /*  Glib::RefPtr<Gdk::Pixbuf> icon*/)
        : DockItem(xid, instance_name, group_name, wintype)
    {
        //  m_icon = icon;
        // g_assert(m_icon);

        // m_width = icon->get_width();
        // m_height = icon->get_height();
    }

    std::shared_ptr<DockItemIcon> DockItemIcon::clone()
    {
        // auto clone = std::shared_ptr<DockItemIcon>(
        // new DockItemIcon(m_xid, m_instance_name, m_group_name, m_wintype));

        auto clone = std::shared_ptr<DockItemIcon>(
            new DockItemIcon(m_xid, m_instance_name, m_group_name, m_wintype));

        clone->m_has_desktop_file = m_has_desktop_file;
        clone->m_attached = m_attached;
        clone->m_title = m_title;
        clone->m_window_name = m_window_name;
        clone->m_desktop_file = m_desktop_file;
        clone->m_icon_name = m_icon_name;
        clone->m_description = m_description;
        clone->m_icon = m_icon;
        clone->m_wintype = m_wintype;
        clone->m_tag = m_tag;
        clone->m_width = m_width;
        clone->m_height = m_height;
        clone->m_attached = m_attached;

        // covariant return type.
        return clone;
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
    const Glib::RefPtr<Gdk::Pixbuf> DockItemIcon::get_icon(guint size)
    {
        //  if (size == m_width) return m_icon;

        auto pixbuf =
            Glib::wrap(m_icon->gobj(), true)->scale_simple(size, size, Gdk::INTERP_BILINEAR);

        g_assert(pixbuf);

        m_width = size;
        m_height = size;

        return pixbuf;
    }

    const std::map<gulong, std::shared_ptr<DockItemIcon>>& DockItemIcon::get_childmap() const
    {
        return m_map;
    }

    // inline
    // const
    int DockItemIcon::remove_child(gulong xid)
    {
        return m_map.erase(xid);
    }

}  // namespace docklight
