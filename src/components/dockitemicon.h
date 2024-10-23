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
#include "components/dockitem.h"
// clang-format on

namespace docklight
{
    class DockItemIcon : public DockItem
    {
      public:
        DockItemIcon(gulong xid, WnckWindow* window, const Glib::ustring& instance_name,
                     const Glib::ustring& group_name, guint wintype);

        void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon);
        const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const;
        const Glib::RefPtr<Gdk::Pixbuf> get_icon(guint size);

        const std::vector<WnckWindow*>& get_wnck_window_list();
        const std::map<gulong, std::shared_ptr<DockItemIcon>>& get_childmap() const;
        void add_child(std::shared_ptr<DockItemIcon> child);
        int remove_child(gulong xid);

        void remove_all() { return m_map.clear(); }
        std::shared_ptr<DockItemIcon> clone();

        guint get_container_size() override;

      private:
        Glib::RefPtr<Gdk::Pixbuf> m_icon;
        std::map<gulong, std::shared_ptr<DockItemIcon>> m_map;
        std::vector<WnckWindow*> m_wnck_window_list;
    };
}  // namespace docklight
