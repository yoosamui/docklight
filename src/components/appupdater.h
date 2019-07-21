#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>
#include "common.h"
#include "components/dockitem.h"

DL_NS_BEGIN

class AppUpdater
{
    enum window_action_t { OPEN, CLOSE };

  public:
    AppUpdater();

    // signal accessor:
    typedef sigc::signal<void, bool, int> type_signal_update;
    type_signal_update signal_update();

    static vector<shared_ptr<DockItem>> m_dock_items;

    void on_theme_changed();

  private:
    static type_signal_update m_signal_update;

    static void on_window_opened(WnckScreen* screen, WnckWindow* window,
                                 gpointer data);

    static void on_window_closed(WnckScreen* screen, WnckWindow* window,
                                 gpointer data);

    static void on_active_window_changed_callback(
        WnckScreen* screen, WnckWindow* previously_active_window,
        gpointer user_data);

    static void Update(WnckWindow* window, window_action_t actiontype);
};

DL_NS_END
