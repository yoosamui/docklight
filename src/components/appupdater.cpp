#include "components/appupdater.h"
#include <gtkmm/icontheme.h>
#include <algorithm>
#include <functional>
#include <thread>
#include "components/config.h"
#include "components/device.h"
#include "components/dockitem.h"
#include "utils/launcher.h"
#include "utils/pixbuf.h"
DL_NS_BEGIN

AppUpdater::type_signal_update AppUpdater::m_signal_update;
vector<shared_ptr<DockItem>> AppUpdater::m_dockitems;

AppUpdater::AppUpdater()
{
    WnckScreen *wnckscreen = wnck_screen_get_default();

    // clang-format off
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(AppUpdater::on_window_opened), NULL);

    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(AppUpdater::on_window_closed), NULL);

    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(AppUpdater::on_active_window_changed_callback),
                     NULL);


     auto const icon_theme = Gtk::IconTheme::get_default();
     icon_theme->signal_changed().connect(sigc::mem_fun(*this, &AppUpdater::on_theme_changed));

    // clang-format on
}

AppUpdater::~AppUpdater()
{
    g_print("Free AppUpdater\n");
}
void AppUpdater::on_theme_changed()
{
    int icon_size = config::get_icon_size();

    for (auto item : m_dockitems) {
        auto icon = pixbuf_util::get_window_icon(
            item->get_wnckwindow(), item->get_desktop_icon_name(), icon_size);

        item->set_image(icon);
    }
    m_signal_update.emit(false, 5);
}

void AppUpdater::on_active_window_changed_callback(
    WnckScreen *screen, WnckWindow *previously_active_window,
    gpointer user_data)
{
}

void AppUpdater::on_window_opened(WnckScreen *screen, WnckWindow *window,
                                  gpointer data)
{
    Update(window, window_action_t::OPEN);
}

void AppUpdater::on_window_closed(WnckScreen *screen, WnckWindow *window,
                                  gpointer data)
{
    Update(window, window_action_t::CLOSE);
}

AppUpdater::type_signal_update AppUpdater::signal_update()
{
    return m_signal_update;
}

void AppUpdater::Update(WnckWindow *window, window_action_t actiontype)
{
    WnckWindowType wt = wnck_window_get_window_type(window);

    if (wt == WNCK_WINDOW_DESKTOP || wt == WNCK_WINDOW_DOCK ||
        wt == WNCK_WINDOW_MENU || wt == WNCK_WINDOW_SPLASHSCREEN) {
        return;
    }

    if (actiontype == window_action_t::OPEN) {
        appinfo_t info;

        launcher_util::get_app_info(window, info);

        g_print("App =>\n");
        g_print("app-name: %s\n", info.m_name.c_str());
        g_print("instance-name: %s\n", info.m_instance.c_str());
        g_print("group-name: %s\n", info.m_group.c_str());
        g_print("title-name: %s\n", info.m_title.c_str());
        g_print("desktop-name: %s\n", info.m_desktop_name.c_str());
        g_print("desktop-icon-name: %s\n", info.m_desktop_icon_name.c_str());
        g_print("desktop-file: %s\n", info.m_desktop_file.c_str());
        g_print("error: %d\n", info.m_error);

        vector<shared_ptr<DockItem>>::iterator it;
        it = std::find_if(m_dockitems.begin(), m_dockitems.end(),
                          [info](shared_ptr<DockItem> const &o) {
                              return o->get_name() == info.m_name;
                          });

        if (it != m_dockitems.end()) {
            // Add Child
            int index = distance(m_dockitems.begin(), it);
            auto const item = m_dockitems[index];
            item->m_items.push_back(shared_ptr<DockItem>(new DockItem(info)));

        } else {
            // Add new
            m_dockitems.push_back(shared_ptr<DockItem>(new DockItem(info)));
            auto const new_item = m_dockitems.back();

            new_item->set_image(pixbuf_util::get_window_icon(
                window, info.m_desktop_icon_name, config::get_icon_size()));

            // Add child
            new_item->m_items.push_back(
                shared_ptr<DockItem>(new DockItem(info)));
        }

        m_signal_update.emit(false, 5);
        return;

    } else {
        // Iterate over all child elements in Vector
        for (size_t i = 0; i < m_dockitems.size(); i++) {
            auto const item = m_dockitems[i];
            for (size_t c = 0; c < item->m_items.size(); c++) {
                auto const citem = item->m_items[c];
                if (citem->get_wnckwindow() == window) {
                    item->m_items.erase(item->m_items.begin() + c);

                    if (!item->is_attached() && item->m_items.size() == 0) {
                        m_dockitems.erase(m_dockitems.begin() + i);
                    }
                    m_signal_update.emit(false, 5);
                    return;
                }
            }
        }
    }
}

DL_NS_END
