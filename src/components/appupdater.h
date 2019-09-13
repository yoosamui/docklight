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
    ~AppUpdater();

    void init();

    // signal accessor:
    typedef sigc::signal<void> type_signal_update;
    type_signal_update signal_update();

    static vector<shared_ptr<DockItem>> m_dockitems;

    void on_theme_changed();
    bool attach_item(int index);
    bool detach_item(const int index);
    bool remove_item(const int index);
    bool save();
    bool load();
    int get_required_size();
    void swap_item(const int next_position);

  private:
    typedef struct {
        dock_item_type_t dock_item_type;
        char name[60];
        char title[60];
        char lang[12];
        char comment[512];
        char desktop_file[512];
        char icon_name[128];
        int separator_length = 0;
        guint8 pixbuff[1024 << 4] = {0};
    } attach_rec_t;

    static type_signal_update m_signal_update;

    static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);

    static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);

    static void on_active_window_changed_callback(WnckScreen* screen,
                                                  WnckWindow* previously_active_window,
                                                  gpointer user_data);

    static void Update(WnckWindow* window, window_action_t actiontype);

    string get_filepath();
};

DL_NS_END
