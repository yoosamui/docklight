#pragma once

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
        guint size()
        {
            //
            return m_dockitems.size();
        }

        template <typename T>
        guint count();
        /*{
            guint count = m_dockitems.size();
            std::shared_ptr<T> dockitem;

            for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
                std::any a = it->second;
                if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;

                count += dockitem->get_container_size();
            }
            return count;
        }
*/
        template <typename T>
        bool get(gulong xid, std::any& a);
        /*{
            //   std::shared_ptr<T> dockitem;  // = get_map().at(cxid);
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
        }*/

        void add(gulong xid, std::any a)
        {
            // creates the pair and the insert it to the list.
            m_dockitems.push_back(std::make_pair(xid, a));
        }

        template <typename T>
        bool exist(const Glib::ustring& group_name, std::shared_ptr<T>& dockitem)
        {
            // std::shared_ptr<T> dockitem;

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
        bool exist(gulong xid, bool deep = false)
        {
            // auto byxid = [&xid](std::pair<gulong, std::any> p) { return p.first == xid; };

            // if (auto it = std::find_if(begin(m_dockitems), end(m_dockitems), byxid);
            // it != std::end(m_dockitems)) {
            // std::cout << " does contain  the xid   \n" << std::endl;
            // return true;
            //} else if (deep) {
            ////
            //}

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
        int remove(gulong xid)
        {
            int result = 0;
            std::shared_ptr<T> dockitem;

            for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
                std::any a = it->second;
                if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;

                if (dockitem->get_childmap().count(xid)) {
                    result += dockitem->remove_child(xid);
                }

                if (dockitem->get_childmap().size() == 0) {
                    // result += m_dockitems.erase(dockitem->get_xid());
                    result += 1;
                    m_dockitems.erase(it);
                }

                if (result) break;
            }

            return result;
        }

        template <typename T>
        std::vector<std::shared_ptr<DockItemIcon>>& data()
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

            // g_print("Dara adquired !!!! size : %lu\n", m_data.size());
            return m_data;
        }

      private:
        guint m_last_size = 0;
        std::vector<std::shared_ptr<DockItemIcon>> m_data;
        std::vector<std::pair<gulong, std::any>> m_dockitems;
    };
}  // namespace docklight
