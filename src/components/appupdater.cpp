#include "appupdater.h"

#include <dirent.h>
#include <giomm/appinfo.h>
#include <giomm/desktopappinfo.h>
#include <giomm/liststore.h>
//#include <glib-object.h>
#include <iostream>
#include <sstream>
#include <thread>

//#include "gio/gdesktopappinfo.h"
/*Thanks, but I couldn't create a working solution so I came up with this one: Code :

    Glib::RefPtr<Gdk::Pixbuf> Info::getPixbuf(File* f)
{
    // File is a custom class
    static Glib::RefPtr<Gtk::IconTheme> iconTheme = Gtk::IconTheme::get_default();
    Glib::ustring sPath = Glib::build_filename(f->getDirPath(), f->getName());
    Glib::RefPtr<Gio::File> gioFile = Gio::File::create_for_path(sPath);
    Glib::RefPtr<Gio::FileInfo> info = gioFile->query_info();
    Glib::RefPtr<Gio::Icon> icon = info->get_icon();
    // getIconSize() a custom function returning the desired size
    Gtk::IconInfo iconInfo =
        iconTheme->lookup_icon(icon, getIconSize(), Gtk::ICON_LOOKUP_USE_BUILTIN);
    return iconInfo.load_icon();
}*/

namespace docklight
{
    std::vector<appinfo_t> m_applist;
    appinfo_t m_appinfo;
    std::map<char*, char*> m_wnck_names;

    BamfMatcher* m_matcher = nullptr;
    void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data)
    {
        if (!window) {
            g_warning("wck-window is null.\n");
            return;
        }
        /*

        For int %d

        For long int %ld

        For long long int %lld

        For unsigned long long int %llu
        */
        m_appinfo.m_window_name = wnck_window_get_name(window);
        m_appinfo.m_group = wnck_window_get_class_group_name(window);
        m_appinfo.m_instance_name = wnck_window_get_class_instance_name(window);
        m_appinfo.m_xid = wnck_window_get_xid(window);
        if (!m_appinfo.m_window_name) {
            g_warning("wck-window name is null.\n");
            m_appinfo.m_window_name = "unknown";
        }

        if (!m_appinfo.m_group) {
            g_warning("wck-window group is null.\n");
            m_appinfo.m_group = "unknown";
        }

        g_print("(%lu)\n)[%s](%s)\n%s\n", m_appinfo.m_xid, m_appinfo.m_instance_name,
                m_appinfo.m_window_name, m_appinfo.m_group);

        // return;
        BamfApplication* bamf_app =
            bamf_matcher_get_application_for_xid(m_matcher, m_appinfo.m_xid);

        if (bamf_app) {
            m_appinfo.m_desktop_file = bamf_application_get_desktop_file(bamf_app);
            if (m_appinfo.m_desktop_file) {
                Glib::RefPtr<Gio::DesktopAppInfo> gio_app =
                    Gio::DesktopAppInfo::create_from_filename(m_appinfo.m_desktop_file);
                if (gio_app) {
                    m_appinfo.m_title = gio_app->get_name();

                    g_print("(%lu)\n%s\n%s\n\n", m_appinfo.m_xid, m_appinfo.m_title.c_str(),
                            m_appinfo.m_desktop_file);
                }
            } else {
                g_message("NO DESKTOP %lu - %s\n", m_appinfo.m_xid, m_appinfo.m_window_name);
            }
        }
        return;
        /*DIR* dir;

        struct dirent* diread;
        char* dirpath = "/usr/share/applications/";
        if ((dir = opendir(dirpath)) != nullptr) {
            char buff[512];
            while ((diread = readdir(dir)) != nullptr) {
                // i..Glib::ustring desktopfile(dirpath + diread->d_name);
                sprintf(buff, "%s%s", dirpath, diread->d_name);
                // g_print("%s\n", buff);
                //  auto app = Gio::DesktopAppInfo::create_from_filename(buff);
                //  if (app) {
                //                    g_print("%s\n", app->get_name().c_str());  //
                //                    displayname;;
                // }
            }

            closedir(dir);
        }*/
        // auto store = Gio::ListStore<Gio::AppInfo>::create();
        //    for (auto&& app : Gio::AppInfo::get_all()) {
        // store->append(app);
        //
        // g_print("%s\n", app->get_display_name().c_str());  // displayname;;
        // g_print("%s\n", app->get_name().c_str());
        // Glib::RefPtr<Gio::Icon> icon = app->get_icon();
        //
        // std::string desktopfile = "/usr/share/applications/" + app->get_id();
        // std::string desktopfile = "/usr/local/share/applications/" + app->get_id();
        // g_print("%s\n", desktopfile.c_str());  // desktop files
        /*key_file = g_key_file_new();
        g_key_file_load_from_file(key_file, desktopfile.c_str(),
        GKeyFileFlags::G_KEY_FILE_NONE, &error); if (error) {
            g_warning("desktopfile:get_key_file:%s ERROR: %s", desktopfile.c_str(),
                      error->message);
            g_error_free(error);
            error = nullptr;
            continue;
        }*/

        /* WORKS
         * auto di = Gio::DesktopAppInfo::create_from_filename(desktopfile);
        auto di = Gio::DesktopAppInfo::create(app->get_id());
        if (di) {
            g_print("%s\n", di->get_generic_name().c_str());
        }

        // GDesktopAppInfo* app_info = g_desktop_app_info_new_from_keyfile(key_file);
        // g_print("%s\n", app->get_id().c_str());  // desktop files
        //
        */
        //  }
        return;

        GList* applist = g_app_info_get_all();

        // GAppInfo* info = g_app_info_get_all();
        //  for (elem = applist; elem; elem = elem->next) {
        ////
        //}
        GList *list, *elem;
        GDesktopAppInfo* item;

        for (elem = g_app_info_get_all(); elem; elem = elem->next) {
            item = (GDesktopAppInfo*)elem->data;
            /* do something with item */
            //  item->get_filename();
        }
        for (applist = g_app_info_get_all(); applist != NULL; applist = applist->next) {
            //            get-display-name
            // print(app.get_display_name())
            //  GAppInfo app = (GAppInfo)applist->data;  //->get_display_name();
            GDesktopAppInfo* app = static_cast<GDesktopAppInfo*>(applist->data);

            // app->get_filename();

            // a.get_display_name();
            //            g_print("%s\n", app->get_display_name());  // filename;  //
            //            data->header.length;

            //  g_print("AAAAAAAAAAAAAAAAAAAAAAAA\n");
        }
        // std::cout << m_appinfo << std::endl;

        //  g_print("Group: %s\nName:%s\n", m_appinfo.m_group, m_appinfo.m_name);
        /*const char* group = wnck_window_get_class_group_name(window);
        if (!group) {
            // We assume that this application don't have any desktop file.
            // --------------------------------------------------------------
            // Gets the name of window ,
            // Always returns some value, even if window has no name set;
            group = wnck_window_get_name(window);
        } else {
            if (group) g_print("Group: %s\n", group);
        }*/
    }
    // void bamf_matcher_on_view_opened(BamfDBusMatcher* proxy, const char* path, const char*
    // type, BamfMatcher* matcher)
    //{
    ////
    //}
    // static void on_urgent_changed(BamfApplication* application, gboolean result, gpointer
    // data)
    //{
    //// signal_seen = TRUE;
    //// signal_result = result;
    //}
    std::map<Glib::ustring, Glib::ustring> m_map;

    AppProvider::AppProvider()
    {
        // WnckHandle *handle = ;  // wnck_get_handle();
        // auto a = wnck_handle_get_default_screen(handle);

        // deprecated
        WnckScreen* wnckscreen = wnck_screen_get_default();
        m_matcher = bamf_matcher_get_default();
        g_assert(BAMF_IS_MATCHER(m_matcher));

        g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(&on_window_opened),
                         nullptr);
        return;
        // test BAMF allocaion
        BamfMatcher* matcher;
        matcher = bamf_matcher_get_default();
        g_assert(BAMF_IS_MATCHER(matcher));
        // const char* names = bamf_matcher_get_active_window((BamfMatcher*)matcher);
        GList* list = nullptr;
        BamfApplication* app;

        //  Glist* testlist; DONT WORK
        // std::thread iotaThread([&matcher]() {
        ////            auto testlist = bamf_matcher_get_applications(matcher);
        ////
        //});
        return;
        // for (list = bamf_matcher_get_applications(matcher); list != nullptr; list =
        // list->next) {
        for (list = bamf_matcher_get_running_applications(matcher); list != nullptr;
             list = list->next) {
            app = (BamfApplication*)list->data;
            if (!app) continue;

            const gchar* desktop_file = bamf_application_get_desktop_file(app);
            if (!desktop_file) continue;

            Glib::RefPtr<Gio::DesktopAppInfo> appinfo =
                Gio::DesktopAppInfo::create_from_filename(desktop_file);
            if (!appinfo) continue;

            g_print("%s\n", appinfo->get_name().c_str());

            g_message("%s\n", desktop_file);
            m_map.insert({appinfo->get_name(), desktop_file});

            GArray* xids = bamf_matcher_get_xids_for_application(matcher, desktop_file);
            int i;
            for (i = 0; i < xids->len; i++) {
                guint32* xid = (guint32*)xids->data;
                // if (!xid) continue;

                g_print("XID %d = %lu\n", i, (unsigned long)*xid);
            }
            g_array_free(xids, TRUE);
        }
        /*for (elem = list; elem; elem = elem->next) {
            item = elem->data;
            [> do something with item <]
        }*/

        // m_map.insert({desk, desk});

        g_object_unref(matcher);
        /*
                BamfView* view;
                BamfApplication* application;
                application = bamf_application_new();*/

        // BamfWindow* window;

        // view = g_object_new(BAMF_TYPE_VIEW, NULL);

        // g_signal_connect(NULL, "view-opened", G_CALLBACK(bamf_matcher_on_view_opened), self);

        // g_assert(BAMF_IS_VIEW(view));
        // g_object_unref(G_OBJECT(view));

        //        g_signal_connect(G_OBJECT(view), #prop "_changed",
        //        (GCallback)on_boolean_event, NULL);
        //////////////////////////////////
        //
        //
        // test BAMF application test
        /*BamfApplication* application;
        application = (BamfApplication*)g_object_new(BAMF_TYPE_APPLICATION, NULL);
        const char* path = "/usr/share/applications/nemo.desktop";
        // application = bamf_application_new_favorite(path);
        // TEST
        BamfApplication* app;
        //        const char* application = "nemo";
        app = bamf_matcher_get_application_for_desktop_file(matcher);
        g_object_unref(application);*/

        //        auto app = g_object_new(BAMF_TYPE_APPLICATION, NULL);
        // bamf_application_get_xids(application);

        //        const gchar* desktop_file = "/unity-control-center.desktop";
        //      auto array = bamf_application_get_xids(application);
        //        application = bamf_application_new_favorite(desktop_file);

        //    g_object_unref(app);
        // BamfApplication* application;
        // application = g_object_new(BAMF_TYPE_APPLICATION, NULL);
        //         application =
        //         bamf_application_new_favorite("/usr/share/applications/nemo.desktop");
    }
}  // namespace docklight
