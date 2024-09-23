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
#include <gdkmm/pixbuf.h>
#include <sstream>
// clang-format on

namespace docklight
{
    class DockItem;
    class IDockItem : public Glib::Object
    {
      protected:
        // 32-biti window identification number,
        guint32 m_xid = 0;

        bool m_has_desktop_file = true;
        bool m_attached = false;

        Glib::ustring m_title = {};
        Glib::ustring m_window_name = {};
        Glib::ustring m_instance_name = {};
        Glib::ustring m_group_name = {};
        Glib::ustring m_desktop_file = {};
        Glib::ustring m_icon_name = {};
        Glib::ustring m_description = {};
        guint m_wintype = 0;
        guint m_tag = 0;

        guint m_width = 0;
        guint m_height = 0;

        Glib::RefPtr<Gdk::Pixbuf> m_icon;
        std::map<guint32, Glib::RefPtr<DockItem>> m_childmap;

      protected:
        // setters
        virtual void set_tag(guint tag) = 0;
        virtual void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon) = 0;
        virtual void set_attached(bool attached) = 0;
        virtual void set_title(Glib::ustring title) = 0;
        virtual void set_window_name(Glib::ustring window_name) = 0;
        virtual void set_instance_name(Glib::ustring instance_name) = 0;
        virtual void set_group_name(Glib ::ustring group_name) = 0;
        virtual void set_desktop_file(Glib ::ustring m_desktop_file) = 0;
        virtual void set_icon_name(Glib::ustring icon_name) = 0;
        virtual void set_description(Glib::ustring icon_name) = 0;
        virtual void set_has_desktop_file(bool has) = 0;

        virtual const void add_child(Glib::RefPtr<DockItem> child) = 0;

        // getters
        virtual const guint get_tag() const = 0;
        virtual const bool get_is_attached() const = 0;
        virtual const guint32 get_xid() const = 0;
        virtual const guint get_width() const = 0;
        virtual const guint get_height() const = 0;
        virtual const guint get_wintype() const = 0;
        virtual const Glib::ustring& get_window_name() const = 0;
        virtual const Glib::ustring& get_title() const = 0;
        virtual const Glib::ustring& get_instance_name() const = 0;
        virtual const Glib::ustring& get_group_name() const = 0;
        virtual const Glib::ustring& get_description() const = 0;
        virtual const Glib::ustring& get_desktop_file() const = 0;
        virtual const Glib::ustring& get_icon_name() const = 0;
        virtual const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const = 0;
        virtual const Glib::RefPtr<Gdk::Pixbuf> get_icon(guint size) = 0;

        virtual const Glib::RefPtr<DockItem> clone() = 0;
        virtual const std::map<guint32, Glib::RefPtr<DockItem>>& get_childmap() const = 0;
        virtual const DockItem* get() const = 0;
        virtual const int remove_child(guint32 xid) = 0;
    };

    class DockItem : public IDockItem
    {
      public:
        DockItem(guint32 xid, const Glib::ustring& instance_name, const Glib::ustring& group_name,
                 guint wintype);
        ~DockItem();

        // implementations

        // setters
        void set_attached(bool attached);
        void set_tag(guint tag);
        void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon);
        void set_icon_name(Glib::ustring icon_name);
        void set_has_desktop_file(bool has);
        void set_title(Glib::ustring title);
        void set_window_name(Glib::ustring window_name);
        void set_instance_name(Glib::ustring instance_name);
        void set_group_name(Glib::ustring group_name);
        void set_description(Glib::ustring description);
        void set_desktop_file(Glib::ustring desktop_file);
        const void add_child(Glib::RefPtr<DockItem> child);

        // Getters
        const guint get_tag() const;
        bool const has_desktop_file() const;
        const Glib::RefPtr<DockItem> clone();
        const DockItem* get() const;
        const std::map<guint32, Glib::RefPtr<DockItem>>& get_childmap() const;
        const bool get_is_attached() const;
        const guint32 get_xid() const;
        const guint get_wintype() const;
        const Glib::ustring& get_window_name() const;
        const Glib::ustring& get_title() const;
        const Glib::ustring& get_group_name() const;
        const Glib::ustring& get_description() const;
        const Glib::ustring& get_instance_name() const;
        const Glib::ustring& get_desktop_file() const;
        const Glib::ustring& get_icon_name() const;
        const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const;
        const Glib::RefPtr<Gdk::Pixbuf> get_icon(guint size);
        const guint get_width() const;
        const guint get_height() const;
        const int remove_child(guint32 xid);
        const Glib::ustring to_string();
    };
}  // namespace docklight
