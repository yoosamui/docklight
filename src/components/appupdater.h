#pragma once
#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1

#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>
#include <queue>
#include <thread>
#include "common.h"
#include "components/dockitem.h"

DL_NS_BEGIN

enum window_action_t { OPEN, CLOSE, UPDATE };

class AppUpdater
{
  public:
    AppUpdater();
    ~AppUpdater();

    void init();

    // signal accessor:
    typedef sigc::signal<void, window_action_t, int> type_signal_update;
    type_signal_update signal_update();

    static vector<shared_ptr<DockItem>> m_dockitems;
    static GdkPixbuf* get_image_from_cache(long xid);
    static void set_image_cache(WnckWindow* window);

    void on_theme_changed();
    bool attach_item(int index);
    bool detach_item(const int index);
    bool attach_all();
    bool remove_item(const int index);
    static bool save();
    bool load();
    int get_required_size();
    void swap_item(const int next_position);
    bool is_exists_expander();

  private:
    thread* m_bck_thread = nullptr;
    static bool m_bck_thread_run;
    static void cache_async();
    static queue<WnckWindow*> m_image_queue;

    typedef struct {
        dock_item_type_t dock_item_type;
        char name[60];
        char title[60];
        char group[60];
        char instance[60];
        char locale[20];
        char comment[512];
        char desktop_file[512];
        char icon_name[128];
        guint8 pixbuff[16384 * 3] = {0};  //  128 x 128 max
    } attach_rec_t;

    static string get_ramdisk_filename(long xid);

    static type_signal_update m_signal_update;

    static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);

    static void on_window_closed(WnckScreen* screen, WnckWindow* window, gpointer data);

    static void on_active_window_changed_callback(WnckScreen* screen,
                                                  WnckWindow* previously_active_window,
                                                  gpointer user_data);

    static void on_active_workspace_changed_callback(WnckScreen* screen,
                                                     WnckWorkspace* previously_active_space,
                                                     gpointer user_data);

    static void Update(WnckWindow* window, window_action_t actiontype);

    static string get_filepath();
};

DL_NS_END
