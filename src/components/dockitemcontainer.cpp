
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

    bool DockItemContainer::is_exist(guint32 xid) const
    {
        return m_appmap.count(xid) > 0;
    }

    bool DockItemContainer::is_exist(guint32 xid, const Glib::ustring& group) const
    {
        for (const auto& it : m_appmap) {
            Glib::RefPtr<DockItem> dockitem = it.second;
            if (xid != dockitem->get_xid() && dockitem->get_group_name() == group) {
                return true;
            }
        }

        return false;
    }

    const std::map<guint32, Glib::RefPtr<DockItem>> DockItemContainer::get_appmap() const
    {
        return m_appmap;
    }

    const std::map<guint32, Glib::RefPtr<DockIcon>> DockItemContainer::get_iconmap() const
    {
        return m_iconmap;
    }

    // TODO make it thread save
    int DockItemContainer::remove(guint32 xid)
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

            // ..if (m_iconmap.count(icon_name) != 1) continue;

            // TODO fix this GtkWindow* need to opass            Glib::RefPtr<Gdk::Pixbuf> icon =
            // pixbuf::get_icon(icon_name);
            //             if (!icon) continue;

            // dockicon->set_icon(icon);
            // m_iconmap.at(icon_name) = dockicon;
        }
    }

    bool DockItemContainer::get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf,
                                           Glib::ustring& title_name, Glib::ustring& desktop_file,
                                           Glib::ustring& icon_name)
    {
        // This matcher is owned by bamf and shared between other callers.
        BamfMatcher* matcher = bamf_matcher_get_default();
        if (!matcher) {
            g_warning("get_theme_icon::(amfMatcher: the object has not been created.");
            return false;
        }

        BamfApplication* bamfapp = bamf_matcher_get_application_for_xid(matcher, xid);
        if (!bamfapp) {
            g_warning("get_theme_icon::BamfApplication: the object has not been created.");
            return false;
        }

        const char* desktopfile = bamf_application_get_desktop_file(bamfapp);
        if (!desktopfile) {
            g_warning("get_theme_icon::desktop_file: the object has not been created.");

            return false;
        }
        // set the output parameter.
        desktop_file = desktopfile;

        Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
            Gio::DesktopAppInfo::create_from_filename(desktop_file);

        if (!appinfo) {
            g_warning("get_theme_icon::Gio::DesktopAppInfo: the object has not been created.");
            return false;
        }

        Glib::ustring iconname = appinfo->get_string("Icon");

        if (iconname.empty()) {
            g_warning(
                "get_theme_icon::g_desktop_app_info_get_string, icon_name : the object has not "
                "been created.");

            return false;
        }

        // set the out parameter.
        icon_name = iconname;
        // This is the name taken from the desktop file.
        title_name = appinfo->get_string("Name");

        // get the default theme
        Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
        if (!theme) {
            g_warning("get_theme_icon::Gtk::IconTheme: the object has not been created.");
            return false;
        }
        try {
            // Always get the icon scaled to the
            // requested size.
            pixbuf = theme->load_icon(icon_name, 128, Gtk::IconLookupFlags::ICON_LOOKUP_FORCE_SIZE);
        } catch (...) {
            g_warning("get_theme_icon::pixbuf: Exception the object has not been created. (%s)",
                      icon_name);
            return false;
        }

        return pixbuf ? true : false;
    }

    bool DockItemContainer::insert(guint32 xid, const Glib::ustring& instance_name,
                                   const Glib::ustring& group_name,
                                   const Glib::ustring& window_name)
    {
        if (is_exist(xid)) return false;

        // set the members values in DockItem
        const Glib::RefPtr<DockItem> dockitem =
            Glib::RefPtr<DockItem>(new DockItem(xid, instance_name, group_name));

        guint32 instance_hash = dockitem->get_hash();

        dockitem->set_xid(xid);
        dockitem->set_window_name(window_name);

        Glib::RefPtr<Gdk::Pixbuf> pixbuf;

        Glib::ustring title_name;
        Glib::ustring desktop_file;
        Glib::ustring icon_name;

        if (get_theme_icon(xid, pixbuf, title_name, desktop_file, icon_name)) {
            dockitem->set_title(title_name);
            dockitem->set_desktop_file(desktop_file);
            dockitem->set_icon_name(icon_name);
            dockitem->set_icon(pixbuf);

            if (is_exist(xid, group_name)) {
                dockitem->add_child(dockitem);
            } else {
                m_appmap.insert({xid, dockitem});
            }
        }

        return true;
    }

}  // namespace docklight
