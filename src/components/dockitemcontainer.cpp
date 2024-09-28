#include "components/dockitemcontainer.h"

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
    }
}  // namespace docklight
