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
#include <gdkmm/display.h>
#include "gio/gdesktopappinfo.h"
#include <libwnck/libwnck.h>
#include <sstream>
// clang-format on

namespace docklight
{
    class DockItem;
    class IDockItem : public Glib::Object
    {
      protected:
        // 32-bit window identification number,
        gulong m_xid = 0;
        gulong m_hash = 0;
        WnckWindow* m_wnckwindow = nullptr;

        bool m_has_desktop_file = true;
        bool m_attached = false;
        bool m_active_window = false;

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

      protected:
        // setters
        virtual void set_attached(bool attached = true) = 0;
        virtual void set_active(bool active) = 0;
        virtual void set_tag(guint tag) = 0;
        virtual void set_title(Glib::ustring title) = 0;
        virtual void set_window_name(Glib::ustring window_name) = 0;
        virtual void set_instance_name(Glib::ustring instance_name) = 0;
        virtual void set_group_name(Glib ::ustring group_name) = 0;
        virtual void set_desktop_file(Glib ::ustring m_desktop_file) = 0;
        virtual void set_icon_name(Glib::ustring icon_name) = 0;
        virtual void set_description(Glib::ustring icon_name) = 0;
        virtual void set_has_desktop_file(bool has) = 0;

        // getters
        virtual WnckWindow* get_wnckwindow() const = 0;

        virtual guint get_container_size() = 0;
        virtual guint get_tag() const = 0;
        virtual guint get_width() const = 0;
        virtual guint get_height() const = 0;
        virtual guint get_wintype() const = 0;

        virtual bool get_attached() const = 0;
        virtual bool get_active() const = 0;

        virtual gulong get_xid() const = 0;
        virtual gulong get_hash() const = 0;

        virtual const Glib::ustring& get_window_name() = 0;
        virtual const Glib::ustring& get_title() const = 0;
        virtual const Glib::ustring& get_instance_name() const = 0;
        virtual const Glib::ustring& get_group_name() const = 0;
        virtual const Glib::ustring& get_description() const = 0;
        virtual const Glib::ustring& get_desktop_file() const = 0;
        virtual const Glib::ustring& get_icon_name() const = 0;
    };

    class DockItem : public IDockItem
    {
      public:
        DockItem(gulong xid, WnckWindow* window, const Glib::ustring& instance_name,
                 const Glib::ustring& group_name, guint wintype);
        ~DockItem();

        // setters
        void set_attached(bool attached = true);
        void set_tag(guint tag);
        void set_active(bool active);
        void set_icon_name(Glib::ustring icon_name);
        void set_has_desktop_file(bool has);
        void set_title(Glib::ustring title);
        void set_window_name(Glib::ustring window_name);
        void set_instance_name(Glib::ustring instance_name);
        void set_group_name(Glib::ustring group_name);
        void set_description(Glib::ustring description);
        void set_desktop_file(Glib::ustring desktop_file);

        // Getters
        virtual WnckWindow* get_wnckwindow() const;
        virtual guint get_container_size() { return 0; };

        guint get_tag() const;
        guint get_wintype() const;
        guint get_width() const;
        guint get_height() const;

        bool has_desktop_file() const;
        bool get_attached() const;
        bool get_active() const;

        gulong get_xid() const;
        gulong get_hash() const;

        const Glib::ustring& get_window_name();
        const Glib::ustring& get_title() const;
        const Glib::ustring& get_group_name() const;
        const Glib::ustring& get_description() const;
        const Glib::ustring& get_instance_name() const;
        const Glib::ustring& get_desktop_file() const;
        const Glib::ustring& get_icon_name() const;

        void launch();

        virtual Glib::ustring to_string();
    };

}  // namespace docklight
