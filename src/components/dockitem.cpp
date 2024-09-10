#include "components/dockitem.h"

namespace docklight
{

    DockItem::DockItem()
    {
        //
    }

    DockItem::~DockItem()
    {
        //
    }

    inline const bool DockItem::has_desktop_file() const
    {
        return m_has_desktop_file;
    }

    // Setters
    inline void DockItem::set_xid(gint32 xid)
    {
        m_xid = xid;
    }

    inline void DockItem::set_refxid(gint32 refxid)
    {
        m_refxid = refxid;
    }

    inline void DockItem::set_has_desktop_file(bool has)
    {
        m_has_desktop_file = has;
    }

    inline void DockItem::set_icon_name(Glib::ustring icon_name)
    {
        m_icon_name = icon_name;
    }

    inline void DockItem::set_icon(Glib::RefPtr<Gdk::Pixbuf> icon)
    {
        m_icon = icon;
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

    inline void DockItem::set_desktop_file(Glib::ustring desktop_file)
    {
        m_desktop_file = desktop_file;
    }

    // Getters
    inline const gint32 DockItem::get_refxid() const
    {
        return m_refxid;
    }

    inline const gint32 DockItem::get_xid() const
    {
        return m_xid;
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

    inline const Glib::ustring& DockItem::get_instance_name() const
    {
        return m_instance_name;
    }

    inline const Glib::ustring& DockItem::get_desktop_file() const
    {
        return m_desktop_file;
    };

    inline const Glib::ustring& DockItem::get_icon_name() const
    {
        return m_icon_name;
    }

    inline const Glib::RefPtr<Gdk::Pixbuf>& DockItem::get_icon() const
    {
        return m_icon;
    };

    const Glib::ustring DockItem::to_string() const
    {
        // clang-format off
            std::stringstream ss;
            ss << "xid: " << m_xid << std::endl
               << "refxid: " << m_refxid << std::endl
               << "title: " << m_title << std::endl
               << "group: " << m_group_name << std::endl
               << "instance: " << m_instance_name << std::endl
               << "icon_name: " << m_icon_name << std::endl
               << "deaktop_file: " << m_desktop_file << std::endl
               << "window_name: " << m_window_name<< std::endl
               << std::endl;

        // clang-format on

        return ss.str();
    }
}  // namespace docklight
