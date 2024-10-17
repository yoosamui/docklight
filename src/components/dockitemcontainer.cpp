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
#include "components/dockitemcontainer.h"
// clang-format on

namespace docklight
{
    template <typename T>
    guint DockItemContainer::count()
    {
        guint count = m_dockitems.size();
        std::shared_ptr<T> dockitem;

        for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
            std::any a = it->second;
            if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;

            count += dockitem->get_container_size();
        }
        return count;
    }

    template <typename T>
    bool DockItemContainer::get(gulong xid, std::any& a)
    {
        auto byxid = [&xid, &a](std::pair<gulong, std::any> p) {
            if (p.first != xid) return false;

            a = p.second;

            return a.has_value() ? true : false;
        };

        if (auto it = std::find_if(begin(m_dockitems), end(m_dockitems), byxid);
            it != std::end(m_dockitems)) {
            return true;
        }

        return false;
    }

    template <typename T>
    bool DockItemContainer::exist(const Glib::ustring& group_name, std::shared_ptr<T>& dockitem)
    {
        auto byxid = [&dockitem, &group_name](std::pair<gulong, std::any> p) {
            std::any a = p.second;

            if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) return false;
            if (dockitem->get_group_name() != group_name) return false;

            return true;
        };

        if (auto it = std::find_if(begin(m_dockitems), end(m_dockitems), byxid);
            it != std::end(m_dockitems)) {
            return true;
        }

        return false;
    }

    template <typename T>
    bool DockItemContainer::exist(gulong xid, bool deep)
    {
        if (xid == 0) {
            exit(1);
        }

        std::shared_ptr<T> dockitem;

        for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
            std::any a = it->second;

            if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;

            if (dockitem->get_xid() == xid) return true;

            if (deep) {
                if (dockitem->get_childmap().count(xid) == xid) return true;
            }
        }

        return false;
    }

    template <typename T>
    int DockItemContainer::remove(gulong xid)
    {
        int result = 0;
        std::shared_ptr<T> dockitem;

        for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
            std::any a = it->second;
            if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;

            if (dockitem->get_attached()) {
                if (!dockitem->get_childmap().size()) continue;

                result += dockitem->remove_child(xid);
                continue;
            }

            if (dockitem->get_childmap().count(xid)) {
                result += dockitem->remove_child(xid);
            }

            if (dockitem->get_childmap().size() == 0) {
                result++;
                m_dockitems.erase(it);
            }

            if (result) break;
        }

        return result;
    }

    template <typename T>
    std::vector<std::shared_ptr<DockItemIcon>>& DockItemContainer::data()
    {
        if (m_last_size == m_dockitems.size()) return m_data;

        m_data.clear();
        std::shared_ptr<T> dockitem;

        for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
            std::any a = it->second;
            if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;
            m_data.push_back(dockitem);
        }
        m_last_size = m_dockitems.size();

        return m_data;
    }

    void DockItemContainer::add(gulong xid, std::any a)
    {
        m_dockitems.push_back(std::make_pair(xid, a));
    }

    guint DockItemContainer::size()
    {
        //
        return m_dockitems.size();
    }

    template bool DockItemContainer::get<DockItemIcon>(gulong xid, std::any& a);
    template std::vector<std::shared_ptr<DockItemIcon>>& DockItemContainer::data<DockItemIcon>();
    template int DockItemContainer::remove<DockItemIcon>(gulong xid);
    template bool DockItemContainer::exist<DockItemIcon>(gulong xid, bool deep);
    template bool DockItemContainer::exist<DockItemIcon>(const Glib::ustring& group_name,
                                                         std::shared_ptr<DockItemIcon>& dockitem);
}  // namespace docklight
