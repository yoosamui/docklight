#pragma once

// clang-format off
#include <gdkmm/pixbuf.h>
#include <sigc++/sigc++.h>
#include <libbamf/libbamf.h>

#include <giomm/desktopappinfo.h>
#include <gtkmm/icontheme.h>
#include "components/dockitem.h"
#include "utils/singletonbase.h"
// clang-format on

namespace docklight
{
    namespace pixbuf
    {

        // TODO move to turils
        Glib::RefPtr<Gdk::Pixbuf> convert_gioicon(const Glib::RefPtr<Gio::Icon>& icon,
                                                  guint size = 128);
    }  // namespace pixbuf

    class DockItemContainer : public SingletonBase<DockItemContainer>
    {
      public:
        DockItemContainer();
        virtual ~DockItemContainer();

        const std::map<int, Glib::RefPtr<DockItem>> get_appmap() const;

        int remove_entry(gint32 xid);
        int count_items_by_title(const Glib::ustring& title);

        bool is_exist(gint32 xid) const;
        bool get_dockitem_by_xid(gint32 xid, Glib::RefPtr<DockItem>& item);
        bool add_entry(gint32 xid, const Glib::ustring& window_name,
                       const Glib::ustring& instance_name, const Glib::ustring& group_name);

      private:
        std::map<int, Glib::RefPtr<DockItem>> m_appmap;
        BamfMatcher* m_matcher = nullptr;
    };

}  // namespace docklight
