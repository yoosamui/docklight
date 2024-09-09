#pragma once
//#define WNCK_I_KNOW_THIS_IS_UNSTABLE 1
// clang-format off
#include <glibmm/main.h>
#include <glibmm/object.h>
#include <gdkmm/pixbuf.h>
#include <libwnck/libwnck.h>
#include <sigc++/sigc++.h>
//#include "constants.h"
#include <X11/X.h>
#include <gdk/gdkx.h>
#include <gtkmm/icontheme.h>
#include <giomm/appinfo.h>
#include <giomm/desktopappinfo.h>
#include <memory>
#include <vector>
//#include "/usr/include/libbamf3/libbamf/libbamf.h"
#include <glib.h>
#include <glib-object.h>
#include <libbamf/libbamf.h>

#include "utils/singletonbase.h"
#include <iostream>


//#include "bamf-view.h"
//#include <libbamf/bamf-view-private.h>
//#include <libbamf/bamf-application.h>
//
//#include <libbamf.h>
//#include "bamf-application.h"
//#include "bamf-window.h"
//#include "bamf-matcher.h"
//#include "bamf-legacy-window.h"
//#include "bamf-legacy-screen.h"
//#include "bamf-tab.h"
//#include <string.h>
//#include <gio/gdesktopappinfo.h>
// clang-format on
//
#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)nullptr

/*template <typename T>
class Singleton
{
  public:
    static T& getInstance()
    {
        static std::unique_ptr<T> instance;
        if (!instance) {
            instance.reset(new T());
        }
        return *instance;
    }

  protected:
    Singleton() {}
    ~Singleton() {}

  private:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

};  // namespace Singleton
*/

namespace docklight
{

    class DockItem;

    class IDockItem : public Glib::Object
    {
      public:
        IDockItem()
        {
            //
            // g_print("Construct\n");
        }

        virtual ~IDockItem()
        {
            //
            // g_print("Free\n");
        }

      protected:
        gint32 m_xid = 0;     // 32-bit identification number,
        gint32 m_refxid = 0;  // the xid for group reference.,
        bool m_has_desktop_file = true;

        Glib::ustring m_title = {};
        Glib::ustring m_window_name = {};
        Glib::ustring m_instance_name = {};
        Glib::ustring m_group_name = {};
        Glib::ustring m_desktop_file = {};

      protected:
        virtual void set_xid(gint32 xid) = 0;

        virtual void set_title(const Glib::ustring& title) = 0;
        virtual void set_window_name(const Glib::ustring& window_name) = 0;
        virtual void set_instance_name(const Glib::ustring& instance_name) = 0;
        virtual void set_group_name(const Glib ::ustring& group_name) = 0;
        virtual void set_desktop_file(const Glib ::ustring& m_desktop_file) = 0;

        virtual const Glib::ustring& get_title() const = 0;
        virtual const Glib::ustring& get_desktop_file() const = 0;
    };

    class DockItem : public IDockItem
    {
      public:
        DockItem() {}

        bool has_desktop_file() { return m_has_desktop_file; }

        void set_has_dsktop_file(bool has) { m_has_desktop_file = has; }
        void set_xid(gint32 xid) { m_xid = xid; };
        void set_title(const Glib::ustring& title)
        {
            //
            m_title = title;
        }
        void set_window_name(const Glib::ustring& window_name) { m_window_name = window_name; };
        void set_instance_name(const Glib::ustring& instance_name)
        {
            m_instance_name = instance_name;
        };
        void set_group_name(const Glib ::ustring& group_name) { m_group_name = group_name; }
        void set_desktop_file(const Glib ::ustring& desktop_file)
        {
            m_desktop_file = desktop_file;
        };

        // Getters
        const Glib::ustring& get_title() const { return m_title; }

        const Glib::ustring& get_desktop_file() const
        {
            //
            return m_desktop_file;
        };
        // void add_entry(gint32 xid, const Glib::ustring& title, const Glib::ustring& window_name,
        // const Glib::ustring& instance_name, const Glib::ustring& group,
        // const Glib::ustring& desktop_file = {},
        // const Glib::RefPtr<Gdk::Pixbuf> icon = {}){

        //};

        // implementation of the pure virtual functions
    };

    class DockItemControler : public SingletonBase<DockItemControler>
    {
      public:
        DockItemControler()
        {
            m_matcher = bamf_matcher_get_default();
            g_assert(BAMF_IS_MATCHER(m_matcher));
        }

        bool is_exist(gint32 xid) { return m_appmap.count(xid) > 0; }

        const std::map<int, Glib::RefPtr<DockItem>>& get_dock_containe() const { return m_appmap; }

        int remove_entry(const gint32& xid)
        {
            return m_appmap.erase(xid);
            //
        }

        inline int count_items_by_title(Glib::ustring& title)
        {
            int count = 0;

            for (auto& kp : m_appmap) {
                Glib::RefPtr<DockItem> item = kp.second;
                if (item->get_title() == title) count++;
            }

            return count;
        }

        bool add_entry(gint32 xid, Glib::ustring window_name, Glib::ustring instance_name,
                       Glib::ustring group_name)
        {
            if (is_exist(xid)) return false;

            const Glib::RefPtr<DockItem> dockitem = Glib::RefPtr<DockItem>(new DockItem());
            dockitem->set_xid(xid);
            dockitem->set_window_name(window_name);
            dockitem->set_instance_name(instance_name);
            dockitem->set_group_name(group_name);

            BamfApplication* bamf_app = bamf_matcher_get_application_for_xid(m_matcher, xid);
            if (!bamf_app) {
                return false;
            }

            const gchar* desktop_file = bamf_application_get_desktop_file(bamf_app);
            if (desktop_file) {
                dockitem->set_desktop_file(desktop_file);

                Glib::RefPtr<Gio::DesktopAppInfo> gio_app =
                    Gio::DesktopAppInfo::create_from_filename(desktop_file);

                if (gio_app) {
                    Glib::ustring title = gio_app->get_name();
                    // TODO handle Pixbuff;
                    //     Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::create();
                    //

                    // TOD test USE THIS TO CREATE GROUPS
                    if (count_items_by_title(title) > 1) {
                        // TODO add xid from first;;
                    }
                    // if (count_items_by_title(title) > 1) {
                    // Glib::RefPtr<Gio::Icon> icon = gio_app->get_icon();
                    Glib::RefPtr<Gio::Icon> icon = gio_app->get_icon();
                    Glib::RefPtr<Gtk::IconTheme> theme = Gtk::IconTheme::get_default();
                    Gtk::IconInfo iconInfo = theme->lookup_icon(
                        icon, 128, Gtk::ICON_LOOKUP_USE_BUILTIN);  // TODO calculate icon size
                    Glib::RefPtr<Gdk::Pixbuf> pixbuf = iconInfo.load_icon();

                    // pixbuf->save("/home/yoo/TEMP/" + gio_app->get_id(), "png");
                    pixbuf->save("/home/yoo/TEMP/" + std::to_string(xid), "png");

                    //                    Glib::RefPtr<Gtk::IconTheme> iconTheme =
                    //                    Gtk::IconTheme::get_default();
                    //  if (icon == (Glib::RefPtr<Gdk::Pixbuf>)nullptr) {
                    //  Glib::RefPtr< Icon > Gio::AppInfo::get_icon 	( 		)
                    // if (count_items_by_title(title) > 1) {
                    // dockitem->set_desktop_file("removed");
                    //}

                    dockitem->set_title(title);
                    m_appmap[xid] = dockitem;
                }
            } else {
                dockitem->set_title(window_name);
                dockitem->set_desktop_file("n/a");
                dockitem->set_has_dsktop_file(false);
                m_appmap[xid] = dockitem;
            }

            return true;
        }
        const std::map<int, Glib::RefPtr<DockItem>>& get_appmap() const { return m_appmap; }

      private:
        std::map<int, Glib::RefPtr<DockItem>> m_appmap;
        BamfMatcher* m_matcher = nullptr;
    };

    ////////////////////////////////////////////////////

    const std::map<int, Glib::ustring>& get_appmap();

    typedef struct {
        // Glib::ustring m_name;
        // Glib::ustring m_group;
        Glib::ustring m_title;
        const gchar* m_window_name = nullptr;
        const gchar* m_group = nullptr;
        const gchar* m_instance_name = nullptr;
        const gchar* m_desktop_file = nullptr;
        int m_xid = 0;

        // std::string m_name;
        // std::string m_group;
        //  string m_group;
        //  string m_instance;
        //  string m_title;
        //  string m_comment;
        //  string m_locale;
        //   bool m_error = false;
        //   string m_desktop_file;
        //   string m_icon_name;
        //   bool m_cache = false;
        //   WnckWindow* m_wnckwindow = nullptr;
        //   dock_item_type_t m_dock_item_type;
        //   unsigned long m_xid = 0;
        //   Glib::RefPtr<Gdk::Pixbuf> m_image = (Glib::RefPtr<Gdk::Pixbuf>)nullptr;
        //   int m_separator_length = 0;
        //   bool m_separartor_expands = true;
        //   int m_width;
        //   int m_height;
    } appinfo_t;

    class AppProvider : public Glib::Object
    {
      public:
        AppProvider();

      private:
        // appinfo_t m_appinfo;

        //  static void on_window_opened(WnckScreen* screen, WnckWindow* window, gpointer data);
    };

}  // namespace docklight
