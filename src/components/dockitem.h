
#pragma once

// clang-format off
#include <glibmm/object.h>
#include <gdkmm/pixbuf.h>
// clang-format on

#define NULLPB (Glib::RefPtr<Gdk::Pixbuf>)nullptr

namespace docklight
{
    class DockItem;
    class IDockItem : public Glib::Object
    {
      protected:
        gint32 m_xid = 0;     // 32-bit identification number,
        gint32 m_refxid = 0;  // the xid for group reference.,

        bool m_has_desktop_file = true;

        Glib::ustring m_title = {};
        Glib::ustring m_window_name = {};
        Glib::ustring m_instance_name = {};
        Glib::ustring m_group_name = {};
        Glib::ustring m_desktop_file = {};
        Glib::RefPtr<Gdk::Pixbuf> m_icon = NULLPB;  // TODO remove this macro;

      protected:
        // setters
        virtual void set_xid(gint32 xid) = 0;
        virtual void set_title(Glib::ustring title) = 0;
        virtual void set_window_name(Glib::ustring window_name) = 0;
        virtual void set_instance_name(Glib::ustring instance_name) = 0;
        virtual void set_group_name(Glib ::ustring group_name) = 0;
        virtual void set_desktop_file(Glib ::ustring m_desktop_file) = 0;
        virtual void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon) = 0;

        // getters
        virtual const Glib::ustring& get_window_name() const = 0;
        virtual const Glib::ustring& get_title() const = 0;
        virtual const Glib::ustring& get_instance_name() const = 0;
        virtual const Glib::ustring& get_group_name() const = 0;
        virtual const Glib::ustring& get_desktop_file() const = 0;
        virtual const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const = 0;
    };

    class DockItem : public IDockItem
    {
      public:
        DockItem();
        ~DockItem();

        // implementations
        bool const has_desktop_file() const;

        // setters
        void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon);
        void set_has_desktop_file(bool has);
        void set_xid(gint32 xid);
        void set_title(Glib::ustring title);
        void set_window_name(Glib::ustring window_name);
        void set_instance_name(Glib::ustring instance_name);
        void set_group_name(Glib::ustring group_name);
        void set_desktop_file(Glib::ustring desktop_file);

        // Getters
        const Glib::ustring& get_window_name() const;
        const Glib::ustring& get_title() const;
        const Glib::ustring& get_group_name() const;
        const Glib::ustring& get_instance_name() const;
        const Glib::ustring& get_desktop_file() const;
        const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const;
    };
}  // namespace docklight
