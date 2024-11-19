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
#include <glibmm/fileutils.h>
// clang-format on

namespace docklight
{

    Glib::RefPtr<DockItemProvider> m_provider;
    Glib::RefPtr<DockItemProvider> create_provider()
    {
        if (!m_provider) {
            m_provider = Glib::RefPtr<DockItemProvider>(new DockItemProvider());
        }
        return m_provider;
    }

    Glib::RefPtr<DockItemProvider> Provider()
    {
        g_assert(m_provider);
        return m_provider;
    }

    DockItemProvider::DockItemProvider()
    {
        m_matcher = bamf_matcher_get_default();
        g_assert(BAMF_IS_MATCHER(m_matcher));

        std::shared_ptr<DockItemIcon> dockitem = std::shared_ptr<DockItemIcon>(
            new DockItemIcon(0, nullptr, DOCKLIGHT_INSTANCENAME, DOCKLIGHT_INSTANCENAME, 0));

        std::string filename = "data/images/docklight-home.svg";
        try {
            auto size = Config()->get_icon_max_size();
            auto pixbuf = Gdk::Pixbuf::create_from_file(filename, size, size, true);
            dockitem->set_icon(pixbuf);

        } catch (const Glib::FileError& ex) {
            g_critical("get_from file: %s FileError: %s", filename.c_str(), ex.what().c_str());
        } catch (const Gdk::PixbufError& ex) {
            g_critical("get_from file: %s PixbufError: %s", filename.c_str(), ex.what().c_str());
        }

        dockitem->set_attached();
        m_container.add(0, dockitem);

        auto const icon_theme = Gtk::IconTheme::get_default();
        icon_theme->signal_changed().connect(
            sigc::mem_fun(*this, &DockItemProvider::on_theme_changed));

        m_sigc_timer =
            Glib::signal_timeout().connect(sigc::mem_fun(this, &DockItemProvider::on_timeout), 100);

        load();

        g_message("Create DockItemProvider.");
    }

    DockItemProvider::~DockItemProvider()
    {
        m_sigc.disconnect();
        g_object_unref(m_matcher);
    }

    bool DockItemProvider::attach(guint index, bool attach)
    {
        if (index < 1 || index > data().size()) return false;

        std::shared_ptr<DockItemIcon> dockitem;
        if (!get_dockitem_by_index(index, dockitem)) return false;

        dockitem->set_attached(attach);
        save();

        if (!attach && dockitem->get_childmap().size() == 0) {
            remove(0);
        }

        return true;
    }

    bool DockItemProvider::on_timeout()
    {
        m_startup_time_set = true;
        on_theme_changed();
        m_signal_update.emit(window_action_t::UPDATE, data().size());

        m_sigc.disconnect();
        return false;
    }

    type_signal_update DockItemProvider::signal_update()
    {
        return m_signal_update;
    }

    bool DockItemProvider::exist(gulong xid)
    {
        return m_container.exist<DockItemIcon>(xid);
    }

    bool DockItemProvider::get_dockitem_by_xid(gulong xid, std::shared_ptr<DockItemIcon>& dockitem)
    {
        for (auto& item : data()) {
            for (auto it : item->get_childmap()) {
                auto child = it.second;
                if (child->get_xid() != xid) continue;

                dockitem = child->clone();
                break;
            }
        }
        return dockitem ? true : false;
    }

    bool DockItemProvider::get_dockitem_by_index(guint index,
                                                 std::shared_ptr<DockItemIcon>& dockitem)
    {
        auto v = data();
        if (index > (guint)v.size()) return false;

        dockitem = v.at(index);
        return dockitem ? true : false;
    }

    std::vector<std::shared_ptr<DockItemIcon>>& DockItemProvider::data()
    {
        return m_container.data<DockItemIcon>();
    }

    guint DockItemProvider::required_size(guint additional_size)
    {
        guint count = additional_size;
        count += Config()->get_dock_area() * data().size();

        return count;
    }

    int DockItemProvider::remove(gulong xid)
    {
        m_window_images.erase(xid);

        auto count = m_container.remove<DockItemIcon>(xid);
        m_signal_update.emit(window_action_t::UPDATE, count);

        return count;
    }

    guint DockItemProvider::DockItemProvider::count()
    {
        guint count = data().size();
        for (auto& dockitem : data()) {
            count += dockitem->get_childmap().size();
        }

        return count;
    }

    void DockItemProvider::on_theme_changed()
    {
        bool updated = false;
        GError* error = nullptr;
        GtkIconTheme* icon_theme = gtk_icon_theme_get_default();
        guint icon_max_size = Config()->get_icon_max_size();

        for (auto& dockitem : data()) {
            GdkPixbuf* gdkpixbuf =
                gtk_icon_theme_load_icon(icon_theme,
                                         dockitem->get_icon_name().c_str(),  // icon name
                                         icon_max_size,                      // icon size
                                         GTK_ICON_LOOKUP_FORCE_SIZE,         // flags //
                                         &error);
            if (error) {
                std::string error_message = error->message;
                g_error_free(error);
                error = nullptr;
                continue;
            }

            if (gdkpixbuf) {
                auto icon = Glib::wrap(gdkpixbuf, true)
                                ->scale_simple(icon_max_size, icon_max_size, Gdk::INTERP_BILINEAR);
                dockitem->set_icon(icon);
            }
        }

        if (updated) {
            g_message("Icon theme updated.");
            m_signal_update.emit(window_action_t::UPDATE, 0);
        }
    }

    bool DockItemProvider::get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf)
    {
        if (!gdkpixbuf) return false;

        auto maxicon_size = Config()->get_icon_max_size();
        pixbuf = Glib::wrap(gdkpixbuf, true)
                     ->scale_simple(maxicon_size, maxicon_size, Gdk::INTERP_BILINEAR);

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
            g_warning("get_theme_icon::bamfMatcher: the object has not been created.");
            return false;
        }

        BamfApplication* bamfapp = bamf_matcher_get_application_for_xid(matcher, xid);
        if (!bamfapp) {
            // g_warning("get_theme_icon::BamfApplication: the object has not been created.");
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
            // Always get the icon scaled to the requested size.
            pixbuf = theme->load_icon(icon_name, Config()->get_icon_max_size(),
                                      Gtk::IconLookupFlags::ICON_LOOKUP_FORCE_SIZE);

        } catch (...) {
            g_warning("get_theme_icon::pixbuf: Exception the object has not been created. (%s)",
                      icon_name.c_str());
            return false;
        }

        return pixbuf ? true : false;
    }

    Glib::RefPtr<Gdk::Pixbuf> DockItemProvider::get_window_image(gulong xid)
    {
        Glib::RefPtr<Gdk::Pixbuf> image;

        if (m_window_images.count(xid)) {
            image = m_window_images.at(xid);
        }

        return image;
    }

    void DockItemProvider::set_window_image(WnckWindow* window, bool initial)
    {
        if (!WNCK_IS_WINDOW(window)) return;

        Glib::RefPtr<Gdk::Pixbuf> image;

        gint32 xid = wnck_window_get_xid(window);
        bool restore = false;

        if (initial && wnck_window_is_minimized(window)) {
            int event_time = gtk_get_current_event_time();
            //   wnck::unminimize(window);
            // wnck::bring_above_window(window);

            wnck_window_activate(window, 1 /*event_time*/);
            wnck_window_make_below(window);

            restore = true;
        }

        // if (wnck_window_is_pinned(window)) {
        // wnck_window_unpin(window);
        //}

        int max = initial ? 4 : 3;

        for (int i = 0; i < max; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (pixbuf::get_window_image(xid, image, Config()->get_preview_image_size())) {
                m_window_images[xid] = image;
            }
        }

        if (restore) {
            //            wnck::minimize(window);
            //            wnck_window_unmake_below(window);
        }
    }

    bool DockItemProvider::insert(WnckWindow* window)
    {
        if (!WNCK_IS_WINDOW(window)) {
            return false;
        }

        // return if the window don't has a name.
        if (!wnck_window_has_name(window)) return false;
        m_wnckwindow = window;

        // return if the DockItem exist.
        gint32 xid = wnck_window_get_xid(window);

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

        // group name must hast to be lower case.
        std::string groupname(group_name);

        std::locale loc("en_US.UTF-8");
        for (auto& c : groupname) {
            c = std::tolower(c, loc);
        }

        auto window_icon_name = wnck_window_get_icon_name(window);
        if (!window_icon_name) {
            window_icon_name = window_name;
        }

        // const char* window_name = wnck_window_get_name(window);
        bool result = createFromDesktopFile(xid, gdkpixbuf, instance_name, groupname, window_name,
                                            window_icon_name, icon_is_fallback, wintype);

        if (!result) {
            result = createFromWindow(xid, gdkpixbuf, instance_name, groupname, window_name,
                                      window_icon_name, icon_is_fallback, wintype);
        }

        if (result && m_startup_time_set) {
            m_signal_update.emit(window_action_t::UPDATE, data().size());
        }

        set_window_image(window, true);
        return result;
    }

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

        if (!get_theme_icon(xid, pixbuf, title_name, desktop_file, icon_name)) {
            return false;
        }

        std::shared_ptr<DockItemIcon> dockitem = std::shared_ptr<DockItemIcon>(
            new DockItemIcon(xid, m_wnckwindow, instance_name, groupname, wintype));

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

        } else {
            dockitem->set_title(title_name);
            m_container.add(xid, dockitem);

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

        if (!get_window_icon(gdkpixbuf, pixbuf)) return false;

        std::shared_ptr<DockItemIcon> dockitem = std::shared_ptr<DockItemIcon>(
            new DockItemIcon(xid, m_wnckwindow, instance_name, groupname, wintype));

        std::shared_ptr<DockItemIcon> owner;
        if (m_container.exist<DockItemIcon>(groupname, owner)) {
            dockitem->set_title(window_icon_name);
            dockitem->set_icon_name(icon_name);
            dockitem->set_icon(pixbuf);

            if (icon_is_fallback) {
                dockitem->set_icon(owner->get_icon());
            }

            if (owner->get_childmap().count(xid)) {
                m_container.remove<DockItemIcon>(xid);

            } else {
                dockitem->set_icon(owner->get_icon());
            }

            // add the new child to the owner.
            owner->add_child(dockitem);

        } else if (!m_container.exist<DockItemIcon>(xid, true) && !icon_is_fallback) {
            dockitem->set_title(groupname);
            dockitem->set_icon_name(icon_name);
            dockitem->set_icon_name(window_icon_name);
            dockitem->set_icon(pixbuf);

            // add a new DockItem
            m_container.add(xid, dockitem);

            dockitem->set_title(window_name);
            if (wintype == WnckWindowType::WNCK_WINDOW_DIALOG) {
                dockitem->set_title(window_icon_name);
            }

            // always add a replica DockTtem clone
            dockitem->add_child(dockitem->clone());
        }

        return true;
    }

    Glib::ustring DockItemProvider::get_config_filepath()
    {
        Glib::ustring user_name = system::get_current_user();

        char config_dir[200];
        sprintf(config_dir, "/home/%s/.config/docklight", user_name.c_str());

        system::create_directory_if_not_exitst(config_dir);

        char buff[PATH_MAX];
        sprintf(buff, "%s/%s", config_dir, "docklight5.dat");

        return buff;
    }

    bool DockItemProvider::load()
    {
        auto file_name = get_config_filepath();
        if (file_name == "") {
            g_warning("provider::load file not available.");
            return false;
        }

        attach_rec_t rec;
        FILE* file_reader;

        file_reader = fopen(file_name.c_str(), "rb");

        if (!file_reader) {
            g_critical("provider::load: can't open file.");
            return false;
        }

        while (true) {
            auto sn = fread(&rec, sizeof(rec), 1, file_reader);
            if (feof(file_reader) != 0) break;
            if (sn == 0) continue;

            std::shared_ptr<DockItemIcon> dockitem = std::shared_ptr<DockItemIcon>(
                new DockItemIcon(0, nullptr, rec.instance, rec.group, 0));

            try {
                auto loader = Gdk::PixbufLoader::create();
                loader->write(rec.pixbuff, sizeof(rec.pixbuff));
                dockitem->set_icon(loader->get_pixbuf());
                loader->close();

            } catch (...) {
                g_warning("provider::PixbufError \n");
            }

            dockitem->set_attached(true);
            dockitem->set_icon_name(rec.icon_name);
            dockitem->set_desktop_file(rec.desktop_file);

            std::shared_ptr<DockItemIcon> owner;
            if (!m_container.exist<DockItemIcon>(rec.group, owner)) {
                m_container.add(0, dockitem);
                continue;
            }
        }

        fclose(file_reader);

        return true;
    }

    bool DockItemProvider::save()
    {
        auto file_name = get_config_filepath();
        if (file_name.empty()) {
            g_critical("provider::save: can't open file.");
            return false;
        }

        FILE* file_writer;
        gchar* iconBuffer;
        gsize buffer_size;
        attach_rec_t rec;

        file_writer = fopen(file_name.c_str(), "wb");

        if (!file_writer) {
            g_critical("provider::save: can't create file.");
            return false;
        }

        for (auto& dockitem : data()) {
            if (!dockitem->get_attached() ||
                dockitem->get_instance_name() == DOCKLIGHT_INSTANCENAME) {
                continue;
            }

            if (!dockitem->get_icon()) continue;

            try {
                dockitem->get_icon()->save_to_buffer(iconBuffer, buffer_size);
                memcpy(rec.pixbuff, iconBuffer, buffer_size);
                delete[](gchar*) iconBuffer;

            } catch (...) {
                g_critical("provider::save: Gdk::PixbufError ");
                return false;
            }

            strncpy(rec.group, dockitem->get_group_name().c_str(), sizeof(rec.group) - 1);
            strncpy(rec.instance, dockitem->get_instance_name().c_str(), sizeof(rec.instance) - 1);
            strncpy(rec.icon_name, dockitem->get_icon_name().c_str(), sizeof(rec.icon_name) - 1);
            strncpy(rec.desktop_file, dockitem->get_desktop_file().c_str(),
                    sizeof(rec.desktop_file) - 1);

            size_t result = fwrite(&rec, sizeof(rec), 1, file_writer);
            if (result == 0) g_critical("provider::save:: Error writing file fwrite\n");
        }

        fclose(file_writer);
        return true;
    }
}  // namespace docklight
