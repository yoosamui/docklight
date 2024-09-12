#pragma once

// clang-format off
#include <gdkmm/pixbuf.h>
#include <sigc++/sigc++.h>
#include <libbamf/libbamf.h>

#include <giomm/desktopappinfo.h>
#include <gtkmm/icontheme.h>
#include "components/dockitem.h"
#include "utils/singletonbase.h"
#include "utils/pixbuf.h"

// clang-format on

namespace docklight
{

    class DockIcon : public Glib::Object
    {
      public:
        DockIcon(Glib::RefPtr<Gdk::Pixbuf> icon, Glib::ustring title, Glib::ustring desktop_file)
        {
            std::string thetitle(title);
            std::replace(thetitle.begin(), thetitle.end(), '.', '-');
            m_icon = icon;
            m_title = thetitle;
            m_desktop_file = desktop_file;
        }

        const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const { return m_icon; }
        const Glib::ustring& get_desktop_file() const { return m_desktop_file; }

        const void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon)
        {
            //
            m_icon = icon;
        }

        const Glib::ustring& get_title() const
        {
            //
            return m_title;
        }

      private:
        Glib::RefPtr<Gdk::Pixbuf> m_icon;
        Glib::ustring m_desktop_file = {};
        Glib::ustring m_title = {};
    };

    class DockItemContainer : public SingletonBase<DockItemContainer>
    {
      public:
        DockItemContainer();
        virtual ~DockItemContainer();

        const std::map<guint32, Glib::RefPtr<DockItem>> get_appmap() const;
        const std::map<guint32, Glib::RefPtr<DockIcon>> get_iconmap() const;

        int remove_entry(guint32 xid);

        bool is_exist(guint32 xid) const;
        bool get_dockitem_by_xid(gint32 xid, Glib::RefPtr<DockItem>& item);
        // clang-format off
        bool insert(guint32 xid,
                const Glib::ustring& window_name,
                const Glib::ustring& instance_name,
                const Glib::ustring& group_name,
                const gchar* window_icon_name,
                GdkPixbuf* window_icon);

        // clang-format on

        // clang-format off
        bool insertX(gint32 xid,
                const Glib::ustring& window_name,
                const Glib::ustring& instance_name,
                const Glib::ustring& group_name,
                const gchar* window_icon_name,
                GdkPixbuf* window_icon);

        // clang-format on
      private:
        int count_items_by_title(const Glib::ustring& title);
        void on_theme_changed();

      private:
        std::map<guint32, Glib::RefPtr<DockItem>> m_appmap;
        std::map<guint32, Glib::RefPtr<DockIcon>> m_iconmap;

        BamfMatcher* m_matcher = nullptr;
    };

    DockItemContainer* get_dockcontainer();

}  // namespace docklight
