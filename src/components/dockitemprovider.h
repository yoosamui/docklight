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

#include "components/dockitemcontainer.h"
//#include "components/dockitemicon.h"
#include "components/config.h"
//#include "utils/singletonbase.h"
#include "utils/pixbuf.h"
// clang-format on
#include <chrono>
namespace docklight
{

    enum window_action_t { OPEN, CLOSE, UPDATE };
    typedef sigc::signal<void, window_action_t, gint> type_signal_update;

    constexpr const guint DEF_MAX_ICON_SIZE = 128;

    class DockItemProvider : public Glib::Object
    {
      public:
        DockItemProvider();
        virtual ~DockItemProvider();

        void request_update_signal();
        //    guint count(guint additional_size) const
        guint required_size(guint additional_size);
        // const std::map<gulong, Glib::RefPtr<DockItemIcon>> get_appmap() const;
        //          const std::map<gulong, std::any> get_appmap();
        int remove(gulong xid);
        gulong exist(const Glib::ustring& group);

        bool exist(gulong xid);
        bool insert(WnckWindow* window);

        //  guint items_count();
        // signal accessor:
        type_signal_update signal_update();

        std::vector<std::shared_ptr<DockItemIcon>>& data()
        {
            return m_container.data<DockItemIcon>();
        }

        // std::map<gulong, std::any>& get_map()
        //{
        ////  return m_container.m_map;
        // return m_map;
        ////  return Container::getInstance()->m_map;
        //}

      private:
        sigc::connection m_sigc;

        void on_theme_changed();
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
        DockItemContainer m_container;
        // std::unordered_map<gulong, std::any> m_map;
        //        std::map<gulong, std::any> m_map;
        //         std::map<gulong, std::any> m_appmap;
        type_signal_update m_signal_update;
        BamfMatcher* m_matcher = nullptr;
    };

    Glib::RefPtr<DockItemProvider> get_dockitem_provider();

}  // namespace docklight