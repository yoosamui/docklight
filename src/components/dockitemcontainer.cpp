
// clang-format off
#include "gio/gdesktopappinfo.h"
#include "components/dockitemcontainer.h"
// clang-format on

namespace docklight
{

    DockItemContainer* get_dockcontainer()
    {
        return DockItemContainer::getInstance();
    }

    DockItemContainer::DockItemContainer()
    {
        m_matcher = bamf_matcher_get_default();
        g_assert(BAMF_IS_MATCHER(m_matcher));
    }

    DockItemContainer::~DockItemContainer()
    {
        g_object_unref(m_matcher);
    }

    bool DockItemContainer::is_exist(gint32 xid) const
    {
        return m_appmap.count(xid) > 0;
    }

    const std::map<int, Glib::RefPtr<DockItem>> DockItemContainer::get_appmap() const
    {
        return m_appmap;
    }

    const std::map<Glib::ustring, Glib::RefPtr<Gdk::Pixbuf>> DockItemContainer::get_iconmap() const
    {
        return m_iconmap;
    }
    // TODO make it thread save
    int DockItemContainer::remove_entry(gint32 xid)
    {
        return m_appmap.erase(xid);
    }

    // As long as only const operations are used on the container the execution is thread-safe.
    // That is the case ot at.
    bool DockItemContainer::get_dockitem_by_xid(gint32 xid, Glib::RefPtr<DockItem>& item)
    {
        if (!is_exist(xid)) return false;

        item = m_appmap.at(xid);

        return true;
    }

    int DockItemContainer::count_items_by_title(const Glib::ustring& title)
    {
        int count = 0;
        int refxid = 0;

        for (auto& kp : m_appmap) {
            Glib::RefPtr<DockItem> item = kp.second;
            if (item->get_title() == title) {
                count++;
                if (count == 1 && item->get_refxid() == 0) {
                    refxid = item->get_xid();
                } else if (item->get_refxid() > 0) {
                    item->set_refxid(refxid);
                }
            }
        }

        return count;
    }

    bool DockItemContainer::add_entry(gint32 xid, const Glib::ustring& window_name,
                                      const Glib::ustring& instance_name,
                                      const Glib::ustring& group_name, GdkPixbuf* window_icon)
    {
        if (is_exist(xid)) return false;

        const Glib::RefPtr<DockItem> dockitem = Glib::RefPtr<DockItem>(new DockItem());
        dockitem->set_xid(xid);
        dockitem->set_window_name(window_name);
        dockitem->set_instance_name(instance_name);

        BamfApplication* bamfapp = bamf_matcher_get_application_for_xid(m_matcher, xid);
        if (!bamfapp) {
            g_warning("DockItemContainer BamfApplication bamfapp is null. %s\n",
                      instance_name.c_str());
            return false;
        }

        const gchar* desktop_file = bamf_application_get_desktop_file(bamfapp);
        if (desktop_file) {
            dockitem->set_desktop_file(desktop_file);

            Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
                Gio::DesktopAppInfo::create_from_filename(desktop_file);

            if (appinfo) {
                char* iconname = g_desktop_app_info_get_string(appinfo->gobj(), "Icon");
                if (iconname) {
                    dockitem->set_icon_name(iconname);

                    // insert the desktop icon if not already exist.
                    if (m_iconmap.count(iconname) == 0) {
                        Glib::RefPtr<Gdk::Pixbuf> icon =
                            pixbuf::convert_gioicon(appinfo->get_icon());

                        m_iconmap.insert({iconname, icon});
                    }
                }

                Glib::ustring title = appinfo->get_name();
                dockitem->set_title(title);

                m_appmap.insert({xid, dockitem});
            }
        } else {
            dockitem->set_has_desktop_file(false);
            dockitem->set_title(window_name);
            dockitem->set_icon_name(instance_name);

            // insert the window icon if not already exist.
            if (m_iconmap.count(instance_name) == 0) {
                Glib::RefPtr<Gdk::Pixbuf> icon = pixbuf::convert_window_icon(window_icon);
                if (icon) {
                    m_iconmap.insert({instance_name, icon});
                }
            }
            m_appmap.insert({xid, dockitem});
        }

        return true;
    }

}  // namespace docklight
