#include "components/appupdater.h"
#include "components/device.h"
#include "components/dockitem.h"
#include "utils/launcher.h"

DL_NS_BEGIN

AppUpdater::type_signal_update AppUpdater::m_signal_update;

AppUpdater::AppUpdater()
{
    WnckScreen *wnckscreen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened",
                     G_CALLBACK(AppUpdater::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed",
                     G_CALLBACK(AppUpdater::on_window_closed), NULL);
    g_signal_connect(wnckscreen, "active_window_changed",
                     G_CALLBACK(AppUpdater::on_active_window_changed_callback),
                     NULL);

    /*
       // theme change
        GdkScreen* screen =
       device::monitor::get_current()->get_screen().gobj()); auto icon_theme =
       g_object_get_data(G_OBJECT(screen), "gtk-icon-theme"); auto settings =
       gtk_settings_get_for_screen(screen); g_signal_connect(settings,
       "notify::gtk-icon-theme-name",
                         G_CALLBACK(AppUpdater::on_theme_changed),
       icon_theme);*/
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
        g_print("desktop-file: %s\n", info.m_desktop_file.c_str());
        g_print("error: %d\n", info.m_error);

        m_signal_update.emit(false, 5);
    } else {
        m_signal_update.emit(false, 5);
    }
}

DL_NS_END
