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
#include <any>
#include <gdkmm/pixbuf.h>
#include <sigc++/sigc++.h>
#include <libbamf/libbamf.h>
#include <libwnck/libwnck.h>
#include <giomm/desktopappinfo.h>
#include <gtkmm/icontheme.h>


//#include "components/dockitemicon.h"
#include "components/dockitemcontainer.h"
#include "components/config.h"
// clang-format on

namespace docklight
{

    enum window_action_t { OPEN, CLOSE, UPDATE };
    typedef sigc::signal<void, window_action_t, gint> type_signal_update;

    class DockItemProvider : public Glib::Object
    {
      public:
        DockItemProvider();
        virtual ~DockItemProvider();

        // void request_update_signal();
        type_signal_update signal_update();

        guint required_size(guint additional_size);
        int remove(gulong xid);

        bool exist(gulong xid);
        bool insert(WnckWindow* window);
        bool get_dockitem_by_index(guint index, std::shared_ptr<DockItemIcon>& dockitem);

        std::vector<std::shared_ptr<DockItemIcon>>& data();

        guint count();

      private:
        sigc::connection m_sigc;

        void on_theme_changed();
        bool on_timeout_draw();

        bool get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf, Glib::ustring& title_name,
                            Glib::ustring& desktop_file, Glib::ustring& icon_name);

        bool createFromDesktopFile(gulong xid, GdkPixbuf* gdkpixbuf, Glib::ustring instance_name,
                                   std::string group_name, Glib::ustring window_name,
                                   Glib::ustring window_icon_name, bool icon_is_fallback,
                                   WnckWindowType wintype);

        bool createFromWindow(gulong xid, GdkPixbuf* gdkpixbuf, Glib::ustring instance_name,
                              std::string groupname, Glib::ustring window_name,
                              Glib::ustring window_icon_name, bool icon_is_fallback,
                              WnckWindowType wintype);

        bool insert(gulong xid, GdkPixbuf* gdkpixbuf, const Glib::ustring instance_name,
                    std::string group_name, const Glib::ustring window_name,
                    const Glib::ustring window_icon_name, bool icon_is_fallback,
                    WnckWindowType wintype);

      private:
        sigc::connection m_sigc_timer;
        DockItemContainer m_container;
        type_signal_update m_signal_update;
        BamfMatcher* m_matcher = nullptr;

        bool m_startup_time_set = false;
    };

    Glib::RefPtr<DockItemProvider> create_provider();
    Glib::RefPtr<DockItemProvider> Provider();

}  // namespace docklight
