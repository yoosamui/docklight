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

    class DockItemContainer : public SingletonBase<DockItemContainer>
    {
      public:
        DockItemContainer();
        virtual ~DockItemContainer();

        const std::map<int, Glib::RefPtr<DockItem>> get_appmap() const;
        const std::map<Glib::ustring, Glib::RefPtr<Gdk::Pixbuf>> get_iconmap() const;

        int remove_entry(gint32 xid);

        bool is_exist(gint32 xid) const;
        bool get_dockitem_by_xid(gint32 xid, Glib::RefPtr<DockItem>& item);
        bool add_entry(gint32 xid, const Glib::ustring& window_name,
                       const Glib::ustring& instance_name, const Glib::ustring& group_name,
                       GdkPixbuf* window_icon);

      private:
        int count_items_by_title(const Glib::ustring& title);

      private:
        std::map<gint32, Glib::RefPtr<DockItem>> m_appmap;
        std::map<Glib::ustring, Glib::RefPtr<Gdk::Pixbuf>> m_iconmap;

        BamfMatcher* m_matcher = nullptr;
    };

    DockItemContainer* get_dockcontainer();

}  // namespace docklight
