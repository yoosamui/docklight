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

#include "components/dockitem.h"

namespace docklight
{

    DockItem::DockItem(guint32 xid, const Glib::ustring& instance_name,
                       const Glib::ustring& group_name, guint wintype)
    {
        m_xid = xid;
        m_wintype = wintype;
        m_instance_name = instance_name;
        m_group_name = group_name;
    }

    DockItem::~DockItem()
    {
        g_print("Dockitem removed  %s %d\n", m_title.c_str(), m_xid);
    }

    inline const Glib::RefPtr<DockItem> DockItem::clone()
    {
        auto clone =
            Glib::RefPtr<DockItem>(new DockItem(m_xid, m_instance_name, m_group_name, m_wintype));

        clone->m_has_desktop_file = m_has_desktop_file;
        clone->m_attached = m_attached;
        clone->m_title = m_title;
        clone->m_window_name = m_window_name;
        clone->m_desktop_file = m_desktop_file;
        clone->m_icon_name = m_icon_name;
        clone->m_description = m_description;
        clone->m_icon = m_icon;
        clone->m_wintype = m_wintype;

        // covariant return type.
        return clone;
    }

    inline const bool DockItem::has_desktop_file() const
    {
        return m_has_desktop_file;
    }

    // Setters
    inline const void DockItem::add_child(Glib::RefPtr<DockItem> child)
    {
        m_childmap.insert({child->get_xid(), child});
    }

    inline void DockItem::set_attached(bool attached)
    {
        m_attached = attached;
    }

    inline void DockItem::set_has_desktop_file(bool has)
    {
        m_has_desktop_file = has;
    }

    inline void DockItem::set_icon(Glib::RefPtr<Gdk::Pixbuf> icon)
    {
        m_icon = icon;
    }

    inline const std::map<guint32, Glib::RefPtr<DockItem>>& DockItem::get_childmap() const
    {
        return m_childmap;
    }

    inline void DockItem::set_icon_name(Glib::ustring icon_name)
    {
        m_icon_name = icon_name;
    }

    inline void DockItem::set_title(Glib::ustring title)
    {
        m_title = title;
    }

    inline void DockItem::set_window_name(Glib::ustring window_name)
    {
        m_window_name = window_name;
    };

    inline void DockItem::set_instance_name(Glib::ustring instance_name)
    {
        m_instance_name = instance_name;
    };

    inline void DockItem::set_group_name(Glib ::ustring group_name)
    {
        m_group_name = group_name;
    }

    inline void DockItem::set_description(Glib::ustring description)
    {
        m_description = description;
    }

    inline void DockItem::set_desktop_file(Glib::ustring desktop_file)
    {
        m_desktop_file = desktop_file;
    }

    // Getters
    inline const guint32 DockItem::get_xid() const
    {
        return m_xid;
    }

    inline const guint DockItem::get_wintype() const
    {
        return m_wintype;
    }

    inline const bool DockItem::get_is_attached() const
    {
        return m_attached;
    }

    inline const Glib::ustring& DockItem::get_window_name() const
    {
        return m_window_name;
    }

    inline const Glib::ustring& DockItem::get_title() const
    {
        return m_title;
    }

    inline const Glib::ustring& DockItem::get_group_name() const
    {
        return m_group_name;
    }

    inline const Glib::ustring& DockItem::get_description() const
    {
        return m_description;
    }

    inline const Glib::ustring& DockItem::get_instance_name() const
    {
        return m_instance_name;
    }

    inline const Glib::ustring& DockItem::get_desktop_file() const
    {
        return m_desktop_file;
    };

    inline const Glib::RefPtr<Gdk::Pixbuf>& DockItem::get_icon() const
    {
        return m_icon;
    }

    inline const DockItem* DockItem::get() const
    {
        return this;
    }

    inline const Glib::ustring& DockItem::get_icon_name() const
    {
        return m_icon_name;
    }

    inline const int DockItem::remove_child(guint32 xid)
    {
        return m_childmap.erase(xid);
    }

    const Glib::ustring DockItem::to_string()
    {
        // clang-format off
            std::stringstream ss;
            ss << "xid: " << m_xid << std::endl
               << "wintype: " << std::to_string(m_wintype)<< std::endl
               << "childrens: " << m_childmap.size()<< std::endl
               << "title: " << m_title << std::endl
               << "group: " << m_group_name << std::endl
               << "instance: " << m_instance_name << std::endl
               << "icon_name: " << m_icon_name << std::endl
               << "deaktop_file: " << m_desktop_file << std::endl
               << "window_name: " << m_window_name<< std::endl
               << "description: " << m_description<< std::endl
               << std::endl;
        // clang-format on

        return ss.str();
    }
}  // namespace docklight
