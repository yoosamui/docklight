#include "components/dockitemcontainer.h"

namespace docklight
{

    namespace pixbuf
    {

        Glib::RefPtr<Gdk::Pixbuf> convert_gioicon(const Glib::RefPtr<Gio::Icon>& icon, guint size)
        {
            Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();

            Gtk::IconInfo iconInfo = theme->lookup_icon(icon, size, Gtk::ICON_LOOKUP_USE_BUILTIN);

            Glib::RefPtr<Gdk::Pixbuf> pixbuf = iconInfo.load_icon();
            return pixbuf;
        }
    }  // namespace pixbuf

    DockItemContainer::DockItemContainer()
    {
        m_matcher = bamf_matcher_get_default();
        g_assert(BAMF_IS_MATCHER(m_matcher));
    }

    DockItemContainer::~DockItemContainer()
    {
        g_object_unref(m_matcher);
    }

    inline bool DockItemContainer::is_exist(gint32 xid) const
    {
        return m_appmap.count(xid) > 0;
    }

    inline const std::map<int, Glib::RefPtr<DockItem>>& DockItemContainer::get_appmap() const
    {
        //
        return m_appmap;
    }

    // TODO make it thread save
    int DockItemContainer::remove_entry(gint32 xid)
    {
        //
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

        for (auto& kp : m_appmap) {
            Glib::RefPtr<DockItem> item = kp.second;
            if (item->get_title() == title) count++;
        }

        return count;
    }

    bool DockItemContainer::add_entry(gint32 xid, const Glib::ustring& window_name,
                                      const Glib::ustring& instance_name,
                                      const Glib::ustring& group_name)
    {
        if (is_exist(xid)) return false;

        const Glib::RefPtr<DockItem> dockitem = Glib::RefPtr<DockItem>(new DockItem());
        dockitem->set_xid(xid);
        dockitem->set_window_name(window_name);
        dockitem->set_instance_name(instance_name);
        dockitem->set_group_name(group_name);

        BamfApplication* bamfapp = bamf_matcher_get_application_for_xid(m_matcher, xid);
        if (!bamfapp) {
            g_object_unref(bamfapp);
            return false;
        }

        const gchar* desktop_file = bamf_application_get_desktop_file(bamfapp);
        if (desktop_file) {
            dockitem->set_desktop_file(desktop_file);

            Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
                Gio::DesktopAppInfo::create_from_filename(desktop_file);

            if (appinfo) {
                Glib::ustring title = appinfo->get_name();
                // TOD test USE THIS TO CREATE GROUPS
                if (count_items_by_title(title) > 1) {
                    // TODO add xid from first;;
                } else {
                    dockitem->set_icon(pixbuf::convert_gioicon(appinfo->get_icon()));
                }

                dockitem->set_title(title);
                m_appmap.insert({xid, dockitem});
            }
        } else {
            dockitem->set_title(window_name);
            dockitem->set_desktop_file("n/a");
            dockitem->set_has_desktop_file(false);
            m_appmap.insert({xid, dockitem});
        }

        g_object_unref(bamfapp);
        return true;
    }

}  // namespace docklight
