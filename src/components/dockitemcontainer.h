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
#include <gdkmm/pixbuf.h>
#include <sigc++/sigc++.h>
#include <libbamf/libbamf.h>

#include <giomm/desktopappinfo.h>
#include <gtkmm/icontheme.h>
#include "components/dockitem.h"
#include "utils/singletonbase.h"
#include "utils/pixbuf.h"
// clang-format on

namespace docklight
{
    class DockItemContainer : public SingletonBase<DockItemContainer>
    {
      public:
        DockItemContainer();
        virtual ~DockItemContainer();

        const std::map<guint32, Glib::RefPtr<DockItem>> get_appmap() const;
        int remove(guint32 xid);
        guint32 exist(const Glib::ustring& group) const;

        bool exist(guint32 xid) const;
        bool insert(guint32 xid, GdkPixbuf* gdkpixbuf, const Glib::ustring& instance_name,
                    const Glib::ustring& group_name, const Glib::ustring& window_name,
                    const Glib::ustring& window_icon_name, bool icon_is_fallback);

      private:
        void on_theme_changed();
        bool get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf, Glib::ustring& title_name,
                            Glib::ustring& desktop_file, Glib::ustring& icon_name);

      private:
        std::map<guint32, Glib::RefPtr<DockItem>> m_appmap;
        BamfMatcher* m_matcher = nullptr;
    };

    DockItemContainer* get_dockcontainer();

}  // namespace docklight
