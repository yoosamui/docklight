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
        //   const std::map<guint32, Glib::RefPtr<DockIcon>> get_iconmap() const;

        int remove(guint32 xid);

        //   bool exist_child(guint32 xid) const;
        bool exist(guint32 xid) const;
        guint32 exist(const Glib::ustring& group) const;

        bool get_dockitem_by_xid(gint32 xid, Glib::RefPtr<DockItem>& item);

        bool insert(guint32 xid, GdkPixbuf* gdkpixbuf, const Glib::ustring& instance_name,
                    const Glib::ustring& group_name, const Glib::ustring& window_name,
                    const Glib::ustring& window_icon_name);
        void test(guint32 xid)
        {
            //
        }
        void test(guint32 xid, GdkPixbuf* gdkpixbuf, const Glib::ustring& instance_name,
                  const Glib::ustring& group_name, const Glib::ustring& window_name)
        {
            //
            /*if (exist(xid)) return;

            // set the members values in DockItem
            const Glib::RefPtr<DockItem> dockitem =
                Glib::RefPtr<DockItem>(new DockItem(xid, instance_name, group_name));

            guint32 instance_hash = dockitem->get_hash();

            dockitem->set_xid(xid);
            dockitem->set_window_name(window_name);

            Glib::RefPtr<Gdk::Pixbuf> pixbuf;

            Glib::ustring title_name;
            Glib::ustring desktop_file;
            Glib::ustring icon_name;
            // Handles desktop files icons and Name for the app.
            if (get_theme_icon(xid, pixbuf, title_name, desktop_file, icon_name)) {
                dockitem->set_title(title_name);
                dockitem->set_desktop_file(desktop_file);
                dockitem->set_icon_name(icon_name);
                dockitem->set_icon(pixbuf);

                // Group the items by group_name.
                auto cxid = exist(group_name);
                if (cxid) {
                    auto owner = m_appmap.at(cxid);
                    owner->add_child(dockitem);

                } else {
                    // Adds a new item.
                    m_appmap.insert({xid, dockitem});
                }
            }

            // Handles the window icons
            if (get_window_icon(gdkpixbuf, pixbuf)) {
                //  if        not exist, we need to add this.
                // if (!exist(xid)) {
                // dockitem->set_title(group_name);
                // dockitem->set_icon_name(icon_name);
                // dockitem->set_icon(pixbuf);
                // g_print("-------------%s %s\n", icon_name.c_str(), group_name.c_str());
                // m_appmap.insert({xid, dockitem});
                //}
            }*/
            return;
        }

      private:
        int count_items_by_title(const Glib::ustring& title);
        void on_theme_changed();
        bool get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf, Glib::ustring& title_name,
                            Glib::ustring& desktop_file, Glib::ustring& icon_name);

      private:
        std::map<guint32, Glib::RefPtr<DockItem>> m_appmap;

        BamfMatcher* m_matcher = nullptr;
    };

    DockItemContainer* get_dockcontainer();

}  // namespace docklight
