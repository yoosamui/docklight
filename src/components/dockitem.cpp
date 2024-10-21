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

#include "components/dockitem.h"

namespace docklight
{
    DockItem::DockItem(gulong xid, WnckWindow* window, const Glib::ustring& instance_name,
                       const Glib::ustring& group_name, guint wintype)
    {
        m_xid = xid;
        m_wintype = wintype;
        m_instance_name = instance_name;
        m_group_name = group_name;

        m_wnckwindow = window;
    }

    DockItem::~DockItem()
    {
        g_message("Dockitem removed  %s [%lu]", m_instance_name.c_str(), m_xid);
    }

    inline void DockItem::set_attached(bool attached)
    {
        if (attached) {
            m_xid = 0;
        }

        m_attached = attached;
    }

    WnckWindow* DockItem::get_wnckwindow() const
    {
        //   g_assert(m_wnckwindow);
        return m_wnckwindow;
    }
    void DockItem::set_tag(guint tag)
    {
        m_tag = tag;
    }

    inline const bool DockItem::has_desktop_file() const
    {
        return m_has_desktop_file;
    }

    // Setters
    // inline const void DockItem::add_child(Glib::RefPtr<DockItem> child)
    //{
    // m_childmap.insert({child->get_xid(), child});
    //}

    inline void DockItem::set_has_desktop_file(bool has)
    {
        m_has_desktop_file = has;
    }

    inline void DockItem::set_icon_name(Glib::ustring icon_name)
    {
        m_icon_name = icon_name;
    }

    inline void DockItem::set_title(Glib::ustring title)
    {
        m_title = title;
    }

    inline void DockItem::set_window_name(Glib::ustring window_name)
    {
        m_window_name = window_name;
    };

    inline void DockItem::set_instance_name(Glib::ustring instance_name)
    {
        m_instance_name = instance_name;
    };

    inline void DockItem::set_group_name(Glib ::ustring group_name)
    {
        m_group_name = group_name;
    }

    inline void DockItem::set_description(Glib::ustring description)
    {
        m_description = description;
    }

    inline void DockItem::set_desktop_file(Glib::ustring desktop_file)
    {
        m_desktop_file = desktop_file;
    }

    // Getters
    inline const guint DockItem::get_tag() const
    {
        return m_tag;
    }

    inline const gulong DockItem::get_xid() const
    {
        return m_xid;
    }

    inline const guint DockItem::get_wintype() const
    {
        return m_wintype;
    }

    inline const bool DockItem::get_attached() const
    {
        return m_attached;
    }

    const Glib::ustring& DockItem::get_window_name()
    {
        if (m_wnckwindow) m_window_name = wnck_window_get_name(m_wnckwindow);

        return m_window_name;
    }

    inline const Glib::ustring& DockItem::get_title() const
    {
        return m_title;
    }

    inline const Glib::ustring& DockItem::get_group_name() const
    {
        return m_group_name;
    }

    inline const Glib::ustring& DockItem::get_description() const
    {
        return m_description;
    }

    inline const Glib::ustring& DockItem::get_instance_name() const
    {
        return m_instance_name;
    }

    inline const Glib::ustring& DockItem::get_desktop_file() const
    {
        return m_desktop_file;
    };

    inline const guint DockItem::get_width() const
    {
        return m_width;
    }

    inline const guint DockItem::get_height() const
    {
        return m_height;
    }

    inline const Glib::ustring& DockItem::get_icon_name() const
    {
        return m_icon_name;
    }

    void DockItem::launch()
    {
        GAppLaunchContext* context = nullptr;
        GAppInfo* app_info = nullptr;
        GKeyFile* key_file = g_key_file_new();
        GError* error = nullptr;

        if (!m_desktop_file.empty()) {
            gboolean found = g_key_file_load_from_file(key_file, m_desktop_file.c_str(),
                                                       GKeyFileFlags::G_KEY_FILE_NONE, nullptr);

            if (found) {
                app_info = (GAppInfo*)g_desktop_app_info_new_from_keyfile(key_file);
                if (app_info) {
                    char* uri = nullptr;
                    GFile* file = nullptr;
                    GList* glist_parameters = nullptr;

                    GdkDisplay* display = gdk_display_get_default();
                    context = (GAppLaunchContext*)gdk_display_get_app_launch_context(display);

                    gboolean launched =
                        g_app_info_launch_uris(app_info, glist_parameters, context, &error);

                    if (error) {
                        g_warning("Launcher: Error %s %s \n", m_instance_name.c_str(),
                                  error->message);

                        g_error_free(error);
                        error = nullptr;
                    }

                    g_object_unref(app_info);
                    g_object_unref(context);

                    if (uri) g_free(uri);

                    if (file) g_object_unref(file);

                    if (launched) {
                        g_key_file_free(key_file);
                        return;
                    }
                }
            }
        }

        // desktop file could not be found
        // launch via command line
        g_spawn_command_line_async(m_instance_name.c_str(), &error);

        if (error) {
            g_warning("Lauch via command line: Error (%s) %s \n", m_instance_name.c_str(),
                      error->message);
            g_error_free(error);
            error = nullptr;
            g_key_file_free(key_file);
        }

        g_key_file_free(key_file);
    }

    const Glib::ustring DockItem::to_string()
    {
        // clang-format off
            std::stringstream ss;
            ss << "xid: " << m_xid << std::endl
               << "wintype: " << std::to_string(m_wintype)<< std::endl
               << "attached: " << (m_attached ? "yes":"no") << std::endl
               << "title: " << m_title << std::endl
               << "group: " << m_group_name << std::endl
               << "instance: " << m_instance_name << std::endl
               << "icon_name: " << m_icon_name << std::endl
               << "deaktop_file: " << m_desktop_file << std::endl
               << "window_name: " << m_window_name<< std::endl
               << "description: " << m_description<< std::endl
               << "width: " << m_width<< std::endl
               << "height: " << m_height<< std::endl
               << std::endl;
        // clang-format on

        return ss.str();
    }
}  // namespace docklight
   // namespace docklight
