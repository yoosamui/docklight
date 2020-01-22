#include "components/appupdater.h"
#include <gdkmm/pixbufloader.h>
#include <glibmm/fileutils.h>
#include <gtkmm/icontheme.h>
#include <stdio.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <thread>
#include "components/config.h"
#include "components/device.h"
#include "components/dockitem.h"
#include "utils/desktopfile.h"
#include "utils/launcher.h"
#include "utils/pixbuf.h"
#include "utils/position.h"
#include "utils/system.h"
#include "utils/wnck.h"

#define DEF_ATTACHMENTS_FILENAME "docklight.dat"

DL_NS_BEGIN

AppUpdater::type_signal_update AppUpdater::m_signal_update;
vector<shared_ptr<DockItem>> AppUpdater::m_dockitems;
queue<WnckWindow *> AppUpdater::m_image_queue;
bool AppUpdater::m_bck_thread_run;

AppUpdater::AppUpdater() {}
void AppUpdater::init()
{
    this->load();

    WnckScreen *wnckscreen = wnck_screen_get_default();

    // clang-format off
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(AppUpdater::on_window_opened), nullptr);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(AppUpdater::on_window_closed), nullptr);


    auto const icon_theme = Gtk::IconTheme::get_default();
    icon_theme->signal_changed().connect(sigc::mem_fun(*this, &AppUpdater::on_theme_changed));

    // clang-format on

    if (!system_util::is_mutter_window_manager()) {
        g_signal_connect(wnckscreen, "active_window_changed",
                         G_CALLBACK(AppUpdater::on_active_window_changed_callback), nullptr);

        g_signal_connect(wnckscreen, "active-workspace-changed",
                         G_CALLBACK(AppUpdater::on_active_workspace_changed_callback), nullptr);

        // start the thread
        AppUpdater::m_bck_thread_run = true;
        m_bck_thread = new thread(cache_async);
    }

    g_print("AppUpdater init done.\n");
}

AppUpdater::~AppUpdater()
{
    // tell the background thread to terminate.
    if (m_bck_thread) {
        m_bck_thread_run = false;

        // Detach
        m_bck_thread->detach();

        // free memory
        delete m_bck_thread;

        // pointed dangling to ptr NULL
        m_bck_thread = nullptr;
    }

    g_print("Free AppUpdater\n");
}
void AppUpdater::on_theme_changed()
{
    int icon_size = config::get_icon_size();
    for (size_t i = 1; i < m_dockitems.size(); i++) {
        auto const item = m_dockitems[i];
        auto icon = pixbuf_util::get_window_icon(item->get_wnckwindow(),
                                                 item->get_desktop_icon_name(), icon_size);

        if (icon == (Glib::RefPtr<Gdk::Pixbuf>)nullptr) {
            continue;
        }
        item->set_image(icon);
    }

    m_signal_update.emit(window_action_t::UPDATE, 0);
    g_print("Theme change\n");
}

void AppUpdater::on_active_workspace_changed_callback(WnckScreen *screen,
                                                      WnckWorkspace *previously_active_space,
                                                      gpointer user_data)
{
    WnckWindow *window = wnck_screen_get_active_window(screen);
    if (!window) {
        return;
    }

    m_image_queue.push(window);

    /*GList *window_l;
    for (window_l = wnck_screen_get_windows(screen); window_l != nullptr;
         window_l = window_l->next) {
        WnckWindow *window = WNCK_WINDOW(window_l->data);
        if (!wnck_util::is_valid_window_type(window)) continue;

        m_image_queue.push(window);
    }*/
}

void AppUpdater::cache_async()
{
    while (m_bck_thread_run) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (m_image_queue.empty()) continue;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        auto const window = m_image_queue.front();
        m_image_queue.pop();

        set_image_cache(window);
    }
}

void AppUpdater::on_active_window_changed_callback(WnckScreen *screen,
                                                   WnckWindow *previously_active_window,
                                                   gpointer user_data)
{
    WnckWindow *window = wnck_screen_get_active_window(screen);
    if (!window) {
        return;
    }

    m_image_queue.push(window);
    m_image_queue.push(previously_active_window);
}

GdkPixbuf *AppUpdater::get_image_from_cache(long xid)
{
    GError **error = nullptr;
    auto filename = get_ramdisk_filename(xid);
    if (!system_util::file_exists(filename)) return nullptr;

    return gdk_pixbuf_new_from_file(filename.c_str(), error);
}

void AppUpdater::set_image_cache(WnckWindow *window)
{
    if (!wnck_util::is_window_on_current_desktop(window)) {
        return;
    }

    if (system_util::is_mutter_window_manager()) {
        return;
    }

    if (wnck_window_is_minimized(window)) {
        return;
    }

    if (!wnck_util::is_valid_window_type(window)) {
        return;
    }

    long xid = wnck_window_get_xid(window);
    auto win_pixbuf = pixbuf_util::get_pixbuf_from_window_raw(xid);
    if (win_pixbuf) {
        string filename = get_ramdisk_filename(xid);

        gdk_pixbuf_save(win_pixbuf, filename.c_str(), "png", nullptr, nullptr);
        g_object_unref(win_pixbuf);

        //  g_print("Create cache file for xid = %ld\n", xid);
    }

    return;
}

string AppUpdater::get_ramdisk_filename(long xid)
{
    char filename[128];
    sprintf(filename, "/tmp/docklight/%010ld.png", xid);

    return filename;
}

void AppUpdater::on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    Update(window, window_action_t::OPEN);
}

void AppUpdater::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    Update(window, window_action_t::CLOSE);
}

AppUpdater::type_signal_update AppUpdater::signal_update()
{
    return m_signal_update;
}

void AppUpdater::Update(WnckWindow *window, window_action_t actiontype)
{
    // clang-format off
    /*
    enum WnckWindowType

    WNCK_WINDOW_NORMAL the window is a normal window.
    WNCK_WINDOW_DESKTOP the window is a desktop.
    WNCK_WINDOW_DOCK the window is a dock or a panel.
    WNCK_WINDOW_DIALOG the window is a dialog window.
    WNCK_WINDOW_TOOLBAR the window is a tearoff toolbar.
    WNCK_WINDOW_MENU the window is a tearoff menu.
    WNCK_WINDOW_UTILITY the window is a small persistent utility window, such as a palette or toolbox.
    WNCK_WINDOW_SPLASHSCREEN the window is a splash screen displayed as an application is starting up.
    */


    if(!wnck_util::is_valid_window_type(window)){
    return;
    }


    static const string ignore_instances[] =
        {
            {DOCKLIGHT_INSTANCENAME},
            {"wrapper-2.0"}            //xcfe menu
        };

    // clang-format on
    for (auto const instance : ignore_instances) {
        const char *instance_name = wnck_window_get_class_instance_name(window);
        if (instance_name != nullptr && strcasecmp(instance_name, instance.c_str()) == 0) {
            return;
        }
    }

    if (actiontype == window_action_t::OPEN) {
        appinfo_t info;
        desktopfile_util::get_app_info(window, info);

        info.m_wnckwindow = window;
        info.m_xid = wnck_window_get_xid(window);

        // g_print("[------Application --------]\n");
        // g_print("app-name: %s\n", info.m_name.c_str());
        // g_print("instance-name: %s\n", info.m_instance.c_str());
        // g_print("group-name: %s\n", info.m_group.c_str());
        // g_print("title-name: %s\n", info.m_title.c_str());
        // g_print("comment: %s\n", info.m_comment.c_str());
        // g_print("desktop-icon-name: %s\n", info.m_icon_name.c_str());
        // g_print("desktop-file: %s\n", info.m_desktop_file.c_str());
        // g_print("locale: %s\n", info.m_locale.c_str());
        // g_print("cache: %d\n", (int)info.m_cache);
        // g_print("items: %d\n", (int)m_dockitems.size());
        // g_print("xid: %d\n", (int)info.m_xid);

        if (info.m_name == "Untitled-window") {
            return;
        }

        vector<shared_ptr<DockItem>>::iterator it;
        it = std::find_if(
            m_dockitems.begin(), m_dockitems.end(),
            [info](shared_ptr<DockItem> const &o) { return o->get_name() == info.m_name; });

        if (it != m_dockitems.end()) {
            // Add Child
            int index = distance(m_dockitems.begin(), it);
            auto const item = m_dockitems[index];

            // update the owner item in case that has been attached
            // or language has been changed
            // item->get_appinfo()->m_title = info.m_title;

            item->get_appinfo()->m_wnckwindow = window;
            item->get_appinfo()->m_xid = wnck_window_get_xid(window);

            item->m_items.push_back(shared_ptr<DockItem>(new DockItem(info)));

        } else {
            auto const first_item = m_dockitems[0];

            if (first_item->get_width() >= DEF_MIN_ITEM_SIZE) {
                // Add new
                m_dockitems.push_back(shared_ptr<DockItem>(new DockItem(info)));
                auto const new_item = m_dockitems.back();

                new_item->set_image(pixbuf_util::get_window_icon(window, info.m_icon_name,
                                                                 config::get_icon_size()));

                // Add child
                new_item->m_items.push_back(shared_ptr<DockItem>(new DockItem(info)));

                m_signal_update.emit(window_action_t::OPEN, (int)m_dockitems.size());
            }
        }

        // set_image_cache(window);
        return;

    } else {
        // Iterate over all child elements in Vector
        for (size_t i = 0; i < m_dockitems.size(); i++) {
            auto const item = m_dockitems[i];
            for (size_t c = 0; c < item->m_items.size(); c++) {
                auto const citem = item->m_items[c];
                if (citem->get_wnckwindow() == window) {
                    // delete child
                    item->m_items.erase(item->m_items.begin() + c);

                    // delete file
                    long xid = wnck_window_get_xid(window);
                    string filename = get_ramdisk_filename(xid);
                    remove(filename.c_str());

                    // delete owner if is not attached
                    if (!item->is_attached() && (int)item->m_items.size() == 0) {
                        m_dockitems.erase(m_dockitems.begin() + i);
                    }

                    m_signal_update.emit(window_action_t::CLOSE, (int)m_dockitems.size());
                    return;
                }
            }
        }
    }
}

void AppUpdater::swap_item(const int next_position)
{
    static int position = 0;

    if (next_position < 0 || next_position > (int)m_dockitems.size()) {
        return;
    }

    if (position == 0 || next_position == 0) {
        position = next_position;
        return;
    }

    if (position == next_position) {
        return;
    }

    iter_swap(m_dockitems.begin() + position, m_dockitems.begin() + next_position);
    position = next_position;
}

string AppUpdater::get_filepath()
{
    string user_name = system_util::get_current_user();

    char config_dir[120];
    sprintf(config_dir, "/home/%s/.config/docklight", user_name.c_str());

    system_util::create_directory_if_not_exitst(config_dir);

    char buff[PATH_MAX];
    sprintf(buff, "%s/%s", config_dir, DEF_ATTACHMENTS_FILENAME);

    return buff;
}

bool AppUpdater::save()
{
    string file_name = get_filepath();
    if (file_name == "") {
        return false;
    }

    gchar *iconBuffer;
    gsize buffer_size;
    attach_rec_t rec;
    FILE *file_writer;
    file_writer = fopen(file_name.c_str(), "wb");
    if (!file_writer) {
        g_critical("AppUpdater::save: can't create file.");
        return false;
    }

    for (size_t idx = 1; idx < m_dockitems.size(); idx++) {
        auto const item = AppUpdater::m_dockitems[idx];
        if (!item->is_attached()) {
            continue;
        }

        auto const info = item->get_appinfo();

        if (info->m_image) {
            try {
                info->m_image->save_to_buffer(iconBuffer, buffer_size);
                memcpy(rec.pixbuff, iconBuffer, buffer_size);
                delete[](gchar *) iconBuffer;

            } catch (...) {
                g_critical("Attachments::save: Gdk::PixbufError %s\n", info->m_name.c_str());
            }
        }

        strncpy(rec.name, info->m_name.c_str(), sizeof(rec.name) - 1);
        strncpy(rec.group, info->m_group.c_str(), sizeof(rec.group) - 1);
        strncpy(rec.instance, info->m_instance.c_str(), sizeof(rec.instance) - 1);
        strncpy(rec.title, info->m_title.c_str(), sizeof(rec.title) - 1);
        strncpy(rec.comment, info->m_comment.c_str(), sizeof(rec.comment) - 1);
        strncpy(rec.locale, info->m_locale.c_str(), sizeof(rec.locale) - 1);
        strncpy(rec.icon_name, info->m_icon_name.c_str(), sizeof(rec.icon_name) - 1);
        strncpy(rec.desktop_file, info->m_desktop_file.c_str(), sizeof(rec.desktop_file) - 1);

        rec.dock_item_type = info->m_dock_item_type;

        size_t result = fwrite(&rec, sizeof(rec), 1, file_writer);
        if (result == 0) g_critical("Attachments::save:: Error writing file> fwrite\n");
    }

    fclose(file_writer);
    return true;
}
bool AppUpdater::load()
{
    string file_name = this->get_filepath();
    if (file_name == "") {
        return false;
    }

    attach_rec_t rec;
    appinfo_t info;
    FILE *file_reader;
    file_reader = fopen(file_name.c_str(), "rb");
    if (!file_reader) {
        g_critical("AppUpdater::load: can't open file.");
        return false;
    }

    while (true) {
        auto sn = fread(&rec, sizeof(rec), 1, file_reader);
        if (feof(file_reader) != 0) break;
        if (sn == 0) continue;

        info.m_dock_item_type = static_cast<dock_item_type_t>(rec.dock_item_type);

        if (rec.pixbuff != nullptr && info.m_dock_item_type == dock_item_type_t::launcher) {
            try {
                auto loader = Gdk::PixbufLoader::create();
                loader->write(rec.pixbuff, sizeof(rec.pixbuff));
                info.m_image = loader->get_pixbuf();
                loader->close();

            } catch (...) {
                g_warning("AppUpdater Load: Gdk::PixbufError %s \n", rec.name);
                info.m_image = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
            }
        }

        info.m_wnckwindow = 0;
        info.m_xid = 0;

        info.m_name = rec.name;
        info.m_group = rec.group;
        info.m_instance = rec.instance;
        info.m_title = rec.title;
        info.m_comment = rec.comment;
        info.m_icon_name = rec.icon_name;
        info.m_locale = rec.locale;
        info.m_desktop_file = rec.desktop_file;

        // check if locale has been changed and desktop file still exists.
        // if the desktop file could not be found then the app has been removed
        if (!desktopfile_util::get_app_info(info)) {
            continue;
        }

        // Add new
        auto item = new DockItem(info, info.m_dock_item_type);
        item->set_attach(true);

        m_dockitems.push_back(shared_ptr<DockItem>(item));
    }
    fclose(file_reader);

    on_theme_changed();
    return true;
}

bool AppUpdater::attach_all()
{
    for (size_t i = 1; i < m_dockitems.size(); i++) {
        auto const item = m_dockitems[i];
        if (!item) continue;

        item->set_attach(true);
    }

    if (!this->save()) {
        g_critical("Error: attach_all: could not save data.");
    }

    return true;
}

bool AppUpdater::attach_item(int index)
{
    if (index < 0 || index > (int)this->m_dockitems.size()) {
        return false;
    }

    auto const item = m_dockitems[index];

    if (item->is_attached()) {
        return false;
    }

    //  item->set_index(index);
    item->set_attach(true);

    if (!this->save()) {
        g_critical("Error: attach_item: could not save data.");
        return false;
    }

    return true;
}

bool AppUpdater::detach_item(const int index)
{
    if (index < 0 || index > (int)m_dockitems.size()) {
        return false;
    }

    auto const item = m_dockitems[index];
    item->set_attach(false);

    if (item->m_items.size() > 0) {
        this->save();
        return true;
    }

    return this->remove_item(index);
}
bool AppUpdater::remove_item(const int index)
{
    if (index < 0 || index > (int)m_dockitems.size()) {
        return false;
    }

    m_dockitems.erase(m_dockitems.begin() + index);
    this->save();

    m_signal_update.emit(window_action_t::CLOSE, 0);
    return true;
}

bool AppUpdater::is_exists_expander()
{
    size_t items_count = m_dockitems.size();
    for (size_t i = 0; i < items_count; i++) {
        auto const item = m_dockitems[i];
        if (item->get_dock_item_type() == dock_item_type_t::expander) {
            return true;
        }
    }

    return false;
}

int AppUpdater::get_required_size()
{
    size_t items_count = m_dockitems.size();
    int size = (items_count - 1) * config::get_separator_margin();
    Gtk::Orientation orientation = config::get_dock_orientation();

    int expander_index = -1;

    for (size_t i = 0; i < items_count; i++) {
        auto const item = m_dockitems[i];

        int item_size =
            orientation == Gtk::ORIENTATION_HORIZONTAL ? item->get_width() : item->get_height();

        if (item_size < 1) {
            continue;
        }

        if (item->get_dock_item_type() == dock_item_type_t::expander) {
            expander_index = i;
            continue;
        }

        size += item_size;
    }

    if (expander_index != -1) {
        int max_size = position_util::get_workarea_max_size();
        int expander_size = max_size - size - config::get_window_start_end_margin();

        auto const item = m_dockitems[expander_index];

        if (expander_size > 1) {
            item->set_expander_size(expander_size);
            size += expander_size;
        } else {
            item->set_expander_size(1);
            size += 1;
        }
    }

    return size;

    DL_NS_END
}
