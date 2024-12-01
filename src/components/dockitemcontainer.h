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
#include <vector>

#include "utils/factory.h"
#include "components/dockitemicon.h"
// clang-format on

namespace docklight
{

    class DockItemContainer : public Glib::Object
    {
      public:
        DockItemContainer();

        guint size();

        template <typename T>
        guint count();

        template <typename T>
        bool get(gulong xid, std::any& a);

        void add(gulong xid, std::any a);

        template <typename T>
        bool exist(gulong xid, bool deep = false);

        template <typename T>
        bool exist(const Glib::ustring& group_name, std::shared_ptr<T>& dockitem);

        template <typename T>
        int remove(gulong xid);

        template <typename T>
        std::vector<std::shared_ptr<DockItemIcon>>& data();

        //  template <typename T>
        void drop(int source_index, int dest_index)
        {
            int size = (int)m_dockitems.size();
            if (source_index < 0 || source_index > size || dest_index < 0 || dest_index > size) {
                return;
            }

            // Source item
            std::shared_ptr<DockItemIcon> dockitem;
            std::pair<gulong, std::any> p = m_dockitems.at(source_index);
            std::any a = p.second;

            g_message("CALLLLLLL %d", source_index);
            if (factory::any_cast<std::shared_ptr<DockItemIcon>>(a, dockitem)) {
                auto xid = dockitem->get_xid();
                // remove<DockItemIcon>(xid);

                m_dockitems.erase(m_dockitems.begin() + source_index);

                auto pair = std::make_pair(xid, a);
                m_dockitems.insert(m_dockitems.begin() + dest_index, pair);

                g_message("done");
            }

            m_last_size = 0;
        }

        void swap(int source_index, int dest_index)
        {
            drop(source_index, dest_index);
            return;

            int size = (int)m_dockitems.size();
            if (source_index < 0 || source_index > size || dest_index < 0 || dest_index > size) {
                return;
            }

            // auto a = m_dockitems.at(source_index);

            // remove(source->get_xid());

            // people.insert(people.begin() + index, temp);
            //  m_dockitems.insert(m_dockitems.begin() + dest_index.source;

            std::iter_swap(m_dockitems.begin() + source_index, m_dockitems.begin() + dest_index);
            m_last_size = 0;
        }

      private:
        guint m_last_size = 0;
        std::vector<std::shared_ptr<DockItemIcon>> m_data;
        std::vector<std::pair<gulong, std::any>> m_dockitems;
    };
}  // namespace docklight
