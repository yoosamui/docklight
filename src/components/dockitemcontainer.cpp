
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

        // Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
        // theme->signal_changed().connect(sigc::mem_fun(*this,
        // &DockItemContainer::on_theme_changed));
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

    const std::map<Glib::ustring, Glib::RefPtr<DockIcon>> DockItemContainer::get_iconmap() const
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
        // int refxid = 0;

        // for (auto& kp : m_appmap) {
        // Glib::RefPtr<DockItem> item = kp.second;
        // if (item->get_title() == title) {
        // count++;
        // if (count == 1 && item->get_refxid() == 0) {
        // refxid = item->get_xid();
        //} else if (item->get_refxid() > 0) {
        // item->set_refxid(refxid);
        //}
        //}
        //}

        return count;
    }
    void DockItemContainer::on_theme_changed()
    {
        return;
        for (const auto& item : m_iconmap) {
            Glib::RefPtr<DockIcon> dockicon = item.second;
            Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
                Gio::DesktopAppInfo::create_from_filename(dockicon->get_desktop_file());

            if (!appinfo) continue;

            char* icon_name = g_desktop_app_info_get_string(appinfo->gobj(), "Icon");
            if (!icon_name) continue;

            if (m_iconmap.count(icon_name) != 1) continue;

            // TODO fix this GtkWindow* need to opass            Glib::RefPtr<Gdk::Pixbuf> icon =
            // pixbuf::get_icon(icon_name);
            //             if (!icon) continue;

            // dockicon->set_icon(icon);
            // m_iconmap.at(icon_name) = dockicon;
        }
    }

    bool DockItemContainer::insertX(gint32 xid, const Glib::ustring& window_name,
                                    const Glib::ustring& instance_name,
                                    const Glib::ustring& group_name, const gchar* window_icon_name,
                                    GdkPixbuf* window_icon)
    {
        return false;
    }
    // clang-format off
    bool DockItemContainer::insert(gint32 xid,
                      const Glib::ustring& window_name,
                      const Glib::ustring& instance_name,
                      const Glib::ustring& group_name,
                      const gchar* window_icon_name,
                      GdkPixbuf* window_icon)
    {
        // clang-format on
        if (is_exist(xid)) return false;
        //#ifdef ENTRY
        // set the members values in DockItem
        const Glib::RefPtr<DockItem> dockitem = Glib::RefPtr<DockItem>(new DockItem());
        dockitem->set_xid(xid);
        dockitem->set_window_name(window_name);
        dockitem->set_instance_name(instance_name);
        dockitem->set_group_name(group_name);

        // create the bamf object
        BamfApplication* bamfapp = bamf_matcher_get_application_for_xid(m_matcher, xid);
        if (!bamfapp) {
            g_warning("DockItemContainer BamfApplication bamfapp is null. %s\n",
                      instance_name.c_str());
            return false;
        }
        // Get the desktop app file.
        // We handle here only apps with desktop files.
        const gchar* desktop_file = bamf_application_get_desktop_file(bamfapp);
        //#endif
        //#ifdef TESTING
        if (desktop_file) {
            dockitem->set_desktop_file(desktop_file);

            Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
                Gio::DesktopAppInfo::create_from_filename(desktop_file);

            if (!appinfo) return false;

            // get the icon name from the desktop file.
            char* icon_name = g_desktop_app_info_get_string(appinfo->gobj(), "Icon");
            if (!icon_name) return false;

            // add fields to the DockItem object.
            Glib::ustring title = appinfo->get_name();
            dockitem->set_icon_name(icon_name);
            dockitem->set_title(title);
            dockitem->set_description(appinfo->get_description());

            // Insert the new app DockItem object.
            m_appmap.insert({xid, dockitem});

            //#endif
            // insert the desktop icon to the iconmap only if not already exist.
            // The scope finish hre return true if already exist.
            if (m_iconmap.count(instance_name) != 0) return true;

            // Get the Pixbuf from the default theme or from window_icon
            Glib::RefPtr<Gdk::Pixbuf> icon = pixbuf::get_icon(icon_name, window_icon);
            if (icon) {
                const Glib::RefPtr<DockIcon> dockicon =
                    Glib::RefPtr<DockIcon>(new DockIcon(icon, desktop_file));

                // add the DockIcon object with the new icon.
                m_iconmap.insert({instance_name, dockicon});
            }

            return true;
        }

        dockitem->set_has_desktop_file(false);
        dockitem->set_title(window_icon_name);
        dockitem->set_icon_name(instance_name);

        m_appmap.insert({xid, dockitem});

        // insert the window icon if not already exist.
        if (m_iconmap.count(instance_name) != 0) return false;

        const Glib::RefPtr<Gdk::Pixbuf> icon = pixbuf::get_window_icon(window_icon);
        Glib::RefPtr<DockIcon> dockicon = Glib::RefPtr<DockIcon>(new DockIcon(icon, ""));

        // insert the new DockIcon to the map.
        m_iconmap.insert({instance_name, dockicon});

        //#endif
        return true;
    }

}  // namespace docklight
