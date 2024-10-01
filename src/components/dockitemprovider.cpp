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
#include "components/dockitemprovider.h"
// clang-format on

namespace docklight
{
    // DockItemProvider* get_dockcontainer()
    //{
    // return DockItemProvider::getInstance();
    //}

    Glib::RefPtr<DockItemProvider> m_provider;
    Glib::RefPtr<DockItemProvider> get_dockitem_provider()
    {
        if (!m_provider) {
            m_provider = Glib::RefPtr<DockItemProvider>(new DockItemProvider());
        }
        return m_provider;
    }

    DockItemProvider::DockItemProvider()
    {
        //        Glib::RefPtr<Container> m_container = Glib::RefPtr<Contcainer>( new
        //        Glib::RefPtr<Container>

        m_matcher = bamf_matcher_get_default();
        g_assert(BAMF_IS_MATCHER(m_matcher));

        auto const icon_theme = Gtk::IconTheme::get_default();

        m_sigc = icon_theme->signal_changed().connect(
            sigc::mem_fun(*this, &DockItemProvider ::on_theme_changed));

        g_message("Create DockItemProvider.");
    }

    void DockItemProvider::request_update_signal()
    {
        g_print("EMIT");
        // m_signal_update.emit(window_action_t::UPDATE, 0);
    }

    type_signal_update DockItemProvider::signal_update()
    {
        return m_signal_update;
    }

    DockItemProvider::~DockItemProvider()
    {
        g_object_unref(m_matcher);

        m_sigc.disconnect();
    }
    // deprecated
    bool DockItemProvider::exist(gulong xid)
    {
        // if (get_map().count(xid)) return true;

        //// std::any a = std::make_shared<MyClass>(new MyClass());

        //// std::shared_ptr<MyClass> target;
        //// dockitem_any_cast<std::shared_ptr<MyClass>>(a, target);  // Works for
        /// shared_ptr<MyClass>

        //// Glib::RefPtr<DockItemIcon> dockitem;

        // for (auto it = get_map().begin(); it != get_map().end(); it++) {
        // std::shared_ptr<DockItemIcon> dockitem;
        // dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);

        // if (!dockitem) continue;
        // if (dockitem->get_childmap().count(xid)) return true;
        //}

        return false;
    }

    // depreated
    gulong DockItemProvider::exist(const Glib::ustring& group)
    {
        /*auto m = get_map();
        g_print("START LOOP[ %lu  ]\n", m.size());
        for (auto it = m.begin(); it != m.end(); it++) {
            std::shared_ptr<DockItemIcon> dockitem;
            dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);
            std::any a = it->second;

            if (a.type() == typeid(std::shared_ptr<DockItemIcon>)) {
                g_print("-------- is good !!!!!!!!!!!!!!!!!!!!!! VNUUUUUUUUUUL\n");
                dockitem = std::any_cast<std::shared_ptr<DockItemIcon>>(a);
            }
            //            g_print(" xxxxxxxxxxxxxxxxx -----LOOP \n");
            if (!dockitem) {
                g_print("--------  GROUP VNUUUUUUUUUUL\n");
                exit(1);
                continue;
            }

            // std::any a = it->second;
            // if (a.type() == typeid(std::shared_ptr<DockItemIcon>)) {
            // dockitem = std::any_cast<std::shared_ptr<DockItemIcon>>(a);
            ////                target = std::any_cast<T>(a);
            //}

            gulong xid = dockitem->get_xid();
            g_print("-------- FOUND A GROUP \n");
            if (dockitem->get_group_name() == group) return xid;
        }*/

        return 0;
    }

    guint DockItemProvider::required_size(guint additional_size)
    {
        guint count = additional_size;
        count += config::get_dock_area() * data().size();

        /*for (auto& dockitem : m_container.data()) {
            // TODO don't whant config here
            //        count += config::get_icon_size();
            // TODO: use the area of the window for now.
            // instead use the value from a DockItemCell IDockItem.
            // This should implemented after the architecture is done
            // use the area as value of m_cell cairo::surface
            count += config::get_dock_area();
        }*/

        /*for (auto it = appmap.begin(); it != appmap.end(); it++) {
            std::shared_ptr<DockItemIcon> dockitem;
            dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);

            // TODO don't whant config here
            //        count += config::get_icon_size();
            // TODO: use the area of the window for now.
            // instead use the value from a DockItemCell IDockItem.
            // This should implemented after the architecture is done
            // use the area as value of m_cell cairo::surface
            count += config::get_dock_area();
        }*/

        return count;
    }

    // const std::unordered_map<gulong, std::any> DockItemProvider::get_appmap()
    //{
    // return get_map();
    //}

    int DockItemProvider::remove(gulong xid)
    {
        auto count = m_container.remove<DockItemIcon>(xid);
        m_signal_update.emit(window_action_t::UPDATE, 0);

        return count;

        // for (auto it = get_map().begin(); it != get_map().end(); it++) {
        // std::shared_ptr<DockItemIcon> dockitem;
        // dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);

        // if (!dockitem) continue;

        // if (dockitem->get_childmap().count(xid)) {
        // result += dockitem->remove_child(xid);
        //}

        // if (dockitem->get_childmap().size() == 0) {
        // result += get_map().erase(dockitem->get_xid());
        //}

        // if (result) break;
        //}

        // if (result) {
        // m_signal_update.emit(window_action_t::UPDATE, 0);
        //}
    }
    void DockItemProvider::on_theme_changed()
    {
        /*bool updated = false;
        Glib::RefPtr<Gdk::Pixbuf> pixbuf;

        for (auto it = get_map().begin(); it != get_map().end(); it++) {
            std::shared_ptr<DockItemIcon> dockitem;
            dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);
            if (!dockitem) continue;

            auto xid = dockitem->get_xid();
            if (get_theme_icon(xid, pixbuf)) {
                dockitem->set_icon(pixbuf);
                if (!updated) updated = true;
            }
        }

        if (updated) {
            g_message("Icon theme updated.");
            m_signal_update.emit(window_action_t::UPDATE, 0);
        }*/
    }

    bool DockItemProvider::get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    {
        if (!gdkpixbuf) return false;

        pixbuf = Glib::wrap(gdkpixbuf, true)
                     ->scale_simple(DEF_MAX_ICON_SIZE, DEF_MAX_ICON_SIZE, Gdk::INTERP_BILINEAR);

        return pixbuf ? true : false;
    }

    bool DockItemProvider::get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    {
        Glib::ustring title_name;
        Glib::ustring desktop_file;
        Glib::ustring icon_name;

        return get_theme_icon(xid, pixbuf, title_name, desktop_file, icon_name);
    }

    bool DockItemProvider::get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf,
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
            pixbuf = theme->load_icon(icon_name, DEF_MAX_ICON_SIZE,
                                      Gtk::IconLookupFlags::ICON_LOOKUP_FORCE_SIZE);
        } catch (...) {
            g_warning("get_theme_icon::pixbuf: Exception the object has not been created. (%s)",
                      icon_name.c_str());
            return false;
        }

        return pixbuf ? true : false;
    }

    // inline guint DockItemProvider::items_count()
    //{
    // guint count = 0;
    // for (auto it = get_map().begin(); it != get_map().end(); it++) {
    // std::shared_ptr<DockItemIcon> dockitem;
    // dockitem_any_cast<std::shared_ptr<DockItemIcon>>(it->second, dockitem);

    // if (!dockitem) continue;

    // count += dockitem->get_childmap().size() + 1;
    //}

    // return count;
    //}

    bool DockItemProvider::insert(WnckWindow* window)
    {
        // reurn if the window don't has a name.
        if (!wnck_window_has_name(window)) return false;

        // return if the DockItem exist.
        gint32 xid = wnck_window_get_xid(window);

        // TODO should i goo traversal or acheck the main map only
        // if (get_map().count(xid)) return false;
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

        // gorup name mus by in lower case.
        std::string groupname(group_name);

        std::locale loc("en_US.UTF-8");
        for (auto& c : groupname) {
            c = std::tolower(c, loc);
        }

        auto window_icon_name = wnck_window_get_icon_name(window);
        if (!window_icon_name) {
            window_icon_name = window_name;
        }

        // if (groupname != "bleachbit") return false;
        // oi
        bool result = createFromDesktopFile(xid, gdkpixbuf, instance_name, groupname, window_name,
                                            window_icon_name, icon_is_fallback, wintype);

        //        if (!result) {
        if (!result)
            result = createFromWindow(xid, gdkpixbuf, instance_name, groupname, window_name,
                                      window_icon_name, icon_is_fallback, wintype);
        //      }

        if (result) {
            m_signal_update.emit(window_action_t::UPDATE, 0);
        }
        // int count = m_container.size();
        // g_print("container %d %s\n", count, groupname.c_str());
        //   g_print("childs %d %s\n",  count, groupname.c_str());
        return result;
    }
    // bool IsOdd(std::pair<gulong, std::any> p)
    //{
    // g_print("%lu\n", p.first);
    // return true;  //]p.first == xid;

    //// return ((i%2)==1);
    //}
    bool DockItemProvider::createFromDesktopFile(gulong xid, GdkPixbuf* gdkpixbuf,
                                                 Glib::ustring instance_name, std::string groupname,
                                                 Glib::ustring window_name,
                                                 Glib::ustring window_icon_name,
                                                 bool icon_is_fallback, WnckWindowType wintype)
    {
        Glib::RefPtr<Gdk::Pixbuf> pixbuf;
        Glib::ustring title_name;
        Glib::ustring desktop_file;
        Glib::ustring icon_name;

        if (!get_theme_icon(xid, pixbuf, title_name, desktop_file, icon_name)) return false;

        // TODO: simplify constrctor only xid and icon. the rest via setters
        std::shared_ptr<DockItemIcon> dockitem =
            std::shared_ptr<DockItemIcon>(new DockItemIcon(xid, instance_name, groupname, wintype));

        dockitem->set_desktop_file(desktop_file);
        dockitem->set_icon_name(icon_name);
        dockitem->set_icon(pixbuf);

        std::shared_ptr<DockItemIcon> owner;
        if (m_container.exist<DockItemIcon>(groupname, owner)) {
            dockitem->set_title(window_name);

            if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                dockitem->set_title(window_icon_name);
            }

            if (icon_is_fallback) {
                dockitem->set_icon(owner->get_icon());
            }

            if (get_window_icon(gdkpixbuf, pixbuf)) {
                dockitem->set_icon(pixbuf);
            }

            owner->add_child(dockitem);
            //                g_print("****G[%lu] %s\n", dockitem->get_xid(),
            //                dockitem->get_group_name().c_str());
            // g_print("****G[%lu] %s childscount : %d\n", owner->get_xid(),
            // dockitem->get_group_name().c_str(), owner->get_container_size());

        } else {
            dockitem->set_title(title_name);
            m_container.add(xid, dockitem);
            // g_print("[%lu] %s  %d\n", xid, dockitem->get_group_name().c_str(),
            // m_container.size());

            // always add a replica Dockitem child clone
            std::shared_ptr<DockItemIcon> child = dockitem->clone();
            child->set_title(window_icon_name);

            if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                child->set_title(window_icon_name);
            }

            if (icon_is_fallback) {
                child->set_icon(dockitem->get_icon());
            }

            if (get_window_icon(gdkpixbuf, pixbuf)) {
                child->set_icon(pixbuf);
            }

            dockitem->add_child(child);
            // g_print("****R[%lu] %s childscount : %d\n", child->get_xid(),
            // dockitem->get_group_name().c_str(), dockitem->get_container_size());
        }

        return true;
    }

    bool DockItemProvider::createFromWindow(gulong xid, GdkPixbuf* gdkpixbuf,
                                            Glib::ustring instance_name, std::string groupname,
                                            Glib::ustring window_name,
                                            Glib::ustring window_icon_name, bool icon_is_fallback,
                                            WnckWindowType wintype)
    {
        Glib::RefPtr<Gdk::Pixbuf> pixbuf;
        Glib::ustring title_name;
        Glib::ustring icon_name = window_icon_name;

        // if (groupname != "mullvad vpn") return false;

        if (!get_window_icon(gdkpixbuf, pixbuf)) return false;

        // g_print("IN  %s \n", groupname.c_str());
        std::shared_ptr<DockItemIcon> dockitem =
            std::shared_ptr<DockItemIcon>(new DockItemIcon(xid, instance_name, groupname, wintype));

        std::shared_ptr<DockItemIcon> owner;
        if (m_container.exist<DockItemIcon>(groupname, owner)) {
            //  g_print("IN GROUP %s \n", groupname.c_str());
            dockitem->set_title(window_icon_name);
            dockitem->set_icon_name(icon_name);
            dockitem->set_icon(pixbuf);

            if (icon_is_fallback) {
                dockitem->set_icon(owner->get_icon());
            }

            if (owner->get_childmap().count(xid)) {
                m_container.remove<DockItemIcon>(xid);
                if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                    owner->add_child(dockitem);
                    // g_print("****G[%lu] %s childscount : %d\n", owner->get_xid(),
                    // owner->get_group_name().c_str(), owner->get_container_size());
                }

            } else {
                // if the icon is fallback
                // replace the icon with the owner icon.
                dockitem->set_icon(owner->get_icon());

                // add the new child icon to the owner.
                if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                    owner->add_child(dockitem);
                }
                // g_print("****G[%lu] %s childscount : %d\n", owner->get_xid(),
                // owner->get_group_name().c_str(), owner->get_container_size());
            }
        } else if (!m_container.exist<DockItemIcon>(xid, true) && !icon_is_fallback) {
            // g_print("IN NEW %s %lu\n", groupname.c_str(), xid);

            dockitem->set_title(groupname);
            dockitem->set_icon_name(icon_name);
            dockitem->set_icon_name(window_icon_name);
            dockitem->set_icon(pixbuf);

            // add a new DockItem
            m_container.add(xid, dockitem);
            // g_print("[%lu] %s  %d\n", xid, dockitem->get_group_name().c_str(),
            // m_container.size());

            // always add a replica DockTtem child clone
            dockitem->set_title(window_name);
            if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                dockitem->set_title(window_icon_name);
            }
            dockitem->add_child(dockitem->clone());
            // g_print("****R[%lu] %s childscount : %d\n", dockitem->get_xid(),
            // dockitem->get_group_name().c_str(), dockitem->get_container_size());
        }

        return false;
    }

}  // namespace docklight
