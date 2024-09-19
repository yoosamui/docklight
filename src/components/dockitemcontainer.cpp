// clang-format off
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

    bool DockItemContainer::exist(guint32 xid) const
    {
        if (m_appmap.count(xid)) return true;

        for (auto it = m_appmap.begin(); it != m_appmap.end(); it++) {
            auto dockitem = it->second;

            if (dockitem->get_childmap().count(xid)) return true;
        }

        return false;
    }

    guint32 DockItemContainer::exist(const Glib::ustring& group) const
    {
        for (const auto& it : m_appmap) {
            Glib::RefPtr<DockItem> dockitem = it.second;
            guint32 xid = dockitem->get_xid();

            if (dockitem->get_group_name() == group) return xid;
        }

        return 0;
    }

    const std::map<guint32, Glib::RefPtr<DockItem>> DockItemContainer::get_appmap() const
    {
        return m_appmap;
    }

    int DockItemContainer::remove(guint32 xid)
    {
        if (m_appmap.count(xid) == 1) {
            return m_appmap.erase(xid);
        }

        for (auto it = m_appmap.begin(); it != m_appmap.end(); it++) {
            Glib::RefPtr<DockItem> dockitem = it->second;
            auto result = dockitem->remove_child(xid);

            if (result) return result;
        }

        return 0;
    }

    void DockItemContainer::on_theme_changed()
    {
        return;
        /*for (const auto& item : m_iconmap) {
            Glib::RefPtr<DockIcon> dockicon = item.second;
            Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
                Gio::DesktopAppInfo::create_from_filename(dockicon->get_desktop_file());

            if (!appinfo) continue;

            char* icon_name = g_desktop_app_info_get_string(appinfo->gobj(), "Icon");
            if (!icon_name) continue;

            // ..if (m_iconmap.count(icon_name) != 1) continue;

            // TODO fix this GtkWindow* need to opass            Glib::RefPtr<Gdk::Pixbuf> icon
            // = pixbuf::get_icon(icon_name);
            //             if (!icon) continue;

            // dockicon->set_icon(icon);
            // m_iconmap.at(icon_name) = dockicon;
        }*/
    }

    bool DockItemContainer::get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    {
        if (!gdkpixbuf) return false;

        pixbuf = Glib::wrap(gdkpixbuf, true)->scale_simple(128, 128, Gdk::INTERP_BILINEAR);

        return pixbuf ? true : false;
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
                      icon_name.c_str());
            return false;
        }

        return pixbuf ? true : false;
    }

    bool DockItemContainer::insert(WnckWindow* window)
    {
        // reurn if the window don't has a name.
        if (!wnck_window_has_name(window)) return false;

        gint32 xid = wnck_window_get_xid(window);

        // return if the DockItem exist.
        if (exist(xid)) return false;

        const char* window_name = wnck_window_get_name(window);
        if (!window_name) return false;

        auto gdkpixbuf = wnck_window_get_icon(window);
        auto wintype = wnck_window_get_window_type(window);
        auto icon_is_fallback = wnck_window_get_icon_is_fallback(window);

        auto instance_name = wnck_window_get_class_instance_name(window);
        if (!instance_name) {
            instance_name = window_name;
        }

        auto group_name = wnck_window_get_class_group_name(window);
        if (!group_name) {
            group_name = window_name;
        }

        auto window_icon_name = wnck_window_get_icon_name(window);
        if (!window_icon_name) {
            window_icon_name = window_name;
        }

        return insert(xid, gdkpixbuf, instance_name, group_name, window_name, window_icon_name,
                      icon_is_fallback, wintype);
    }

    bool DockItemContainer::insert(guint32 xid, GdkPixbuf* gdkpixbuf,
                                   const Glib::ustring instance_name,
                                   const Glib::ustring group_name, const Glib::ustring window_name,
                                   const Glib::ustring window_icon_name, bool icon_is_fallback,
                                   WnckWindowType wintype)
    {
        //   if (group_name != "Xfce4-settings-manager") return false;
        //        if (group_name != "Xlet-settings.py" && group_name != "Firefox-esr") return false;
        // if (group_name != "Geany" && group_name != "Gedit" && group_name !=
        // "Nm-connection-editor")
        //    return false;

        Glib::RefPtr<Gdk::Pixbuf> pixbuf;
        Glib::ustring title_name;
        Glib::ustring desktop_file;
        Glib::ustring icon_name;

        // Handles desktop files icons and Names for the app.
        if (get_theme_icon(xid, pixbuf, title_name, desktop_file, icon_name)) {
            // create the DockItem.
            const auto dockitem =
                Glib::RefPtr<DockItem>(new DockItem(xid, instance_name, group_name, wintype));

            dockitem->set_desktop_file(desktop_file);
            // TODO: handle by app TYPE
            //  dockitem->set_window_name(window_name);
            dockitem->set_icon_name(icon_name);
            dockitem->set_icon(pixbuf);

            // Group the items by group_name.
            auto cxid = exist(group_name);
            if (cxid) {
                auto owner = m_appmap.at(cxid);
                dockitem->set_title(window_name);
                if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                    dockitem->set_title(window_icon_name);
                }

                if (get_window_icon(gdkpixbuf, pixbuf)) {
                    dockitem->set_icon(pixbuf);
                }

                if (icon_is_fallback) {
                    dockitem->set_icon(owner->get_icon());
                }

                owner->add_child(dockitem);

            } else {
                // add a new DockItem
                dockitem->set_title(title_name);
                m_appmap.insert({xid, dockitem});

                // always add a replica Dockitem child clone
                auto child = dockitem->clone();
                child->set_title(window_name);
                if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                    child->set_title(window_icon_name);
                }

                if (get_window_icon(gdkpixbuf, pixbuf)) {
                    child->set_icon(pixbuf);
                }

                if (icon_is_fallback) {
                    child->set_icon(dockitem->get_icon());
                }

                dockitem->add_child(child);
            }
        } else {
            // Handles the window icons
            if (get_window_icon(gdkpixbuf, pixbuf)) {
                const auto dockitem =
                    Glib::RefPtr<DockItem>(new DockItem(xid, instance_name, group_name, wintype));

                // create groups setion.
                auto cxid = exist(group_name);
                if (cxid) {
                    const auto dockitem = Glib::RefPtr<DockItem>(
                        new DockItem(xid, instance_name, group_name, wintype));
                    dockitem->set_title(window_icon_name);
                    dockitem->set_icon_name(icon_name);
                    dockitem->set_icon(pixbuf);

                    // if a child exist then remove it and
                    // add a new child. Is the icon is fallback the
                    // set the icon from owner.
                    // finally add the child item.
                    auto owner = m_appmap.at(cxid);
                    if (icon_is_fallback) {
                        dockitem->set_icon(owner->get_icon());
                    }

                    if (owner->get_childmap().count(xid)) {
                        remove(xid);
                        if (wintype == 3) dockitem->set_title(window_icon_name);  // wintype 3
                        owner->add_child(dockitem);
                    } else {
                        // if the icon is fallback
                        // replace the icon with the owner icon.
                        dockitem->set_icon(owner->get_icon());

                        // add the new child icon to the owner.
                        if (wintype == 3) dockitem->set_title(window_icon_name);  // wintype 3
                        owner->add_child(dockitem);
                    }
                } else if (!exist(xid) && !icon_is_fallback) {
                    const auto dockitem = Glib::RefPtr<DockItem>(
                        new DockItem(xid, instance_name, group_name, wintype));

                    // TODO fix with window type
                    // group_name or  windown name
                    /*std::string group(group_name);
                    if (group.find('.') != std::string::npos) {
                        group = window_icon_name;
                    }*/
                    dockitem->set_title(group_name);
                    dockitem->set_icon_name(icon_name);
                    dockitem->set_icon_name(window_icon_name);
                    dockitem->set_icon(pixbuf);

                    // add a new DockItem
                    m_appmap.insert({xid, dockitem});

                    // always add a replica DockTtem child clone
                    dockitem->set_title(window_name);
                    if (wintype == 3) dockitem->set_title(window_icon_name);  // wintype 3
                    dockitem->add_child(dockitem->clone());
                }
            }
        }
        return true;
    }

}  // namespace docklight
