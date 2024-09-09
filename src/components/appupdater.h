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
            g_print("Construct\n");
        }

        virtual ~IDockItem()
        {
            //
            g_print("Free\n");
        }

      protected:
        gint32 m_xid = 0;  // 32-bit identification number,
        Glib::ustring m_window_name = {};
        Glib::ustring m_instance_name = {};
        Glib::ustring m_group_name = {};

      protected:
        virtual void set_xid(gint32 xid) = 0;

        virtual void set_window_name(Glib::ustring& window_name) = 0;
        virtual void set_instance_name(Glib::ustring& instance_name) = 0;
        virtual void set_group_name(Glib ::ustring& group_name) = 0;
    };

    class DockItem : public IDockItem
    {
      public:
        DockItem() {}

        void set_xid(gint32 xid) { m_xid = xid; };
        void set_window_name(Glib::ustring& window_name) { m_window_name = window_name; };
        void set_instance_name(Glib::ustring& instance_name) { m_instance_name = instance_name; };
        void set_group_name(Glib ::ustring& group_name) { m_group_name = group_name; }

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
        bool is_exist(gint32 xid) { return m_appmap.count(xid) > 0; }

        bool add_entry(gint32 xid, Glib::ustring window_name, Glib::ustring instance_name,
                       Glib::ustring group_name)
        {
            if (is_exist(xid)) return false;

            const Glib::RefPtr<DockItem> dockitem = Glib::RefPtr<DockItem>(new DockItem());
            dockitem->set_xid(xid);
            dockitem->set_window_name(window_name);
            dockitem->set_instance_name(instance_name);
            dockitem->set_group_name(gnnnroup_name);

            BamfMatcher* matcher;
            matcher = bamf_matcher_get_default();
            g_assert(BAMF_IS_MATCHER(matcher));

            BamfMatcher* matcher = nullptr;
            BamfApplication* bamf_app = bamf_matcher_get_application_for_xid(matcher, xid);

            if (!bamf_app) {
                return;
            }
            return true;
        }

      private:
        std::map<int, Glib::RefPtr<DockItem>> m_appmap;
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
