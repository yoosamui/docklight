
#pragma once

// clang-format off
#include <glibmm/object.h>
#include <gdkmm/pixbuf.h>
#include <sstream>

// clang-format on

namespace docklight
{
    class DockItem;
    class IDockItem : public Glib::Object
    {
      protected:
        guint32 m_xid = 0;  // 32-bit identification number,
        guint32 m_hash = 0;

        bool m_has_desktop_file = true;

        Glib::ustring m_title = {};
        Glib::ustring m_window_name = {};
        Glib::ustring m_instance_name = {};
        Glib::ustring m_group_name = {};
        Glib::ustring m_desktop_file = {};
        Glib::ustring m_icon_name = {};
        Glib::ustring m_description = {};

      protected:
        // setters
        virtual void set_xid(guint32 xid) = 0;
        virtual void set_title(Glib::ustring title) = 0;
        virtual void set_window_name(Glib::ustring window_name) = 0;
        virtual void set_instance_name(Glib::ustring instance_name) = 0;
        virtual void set_group_name(Glib ::ustring group_name) = 0;
        virtual void set_desktop_file(Glib ::ustring m_desktop_file) = 0;
        virtual void set_icon_name(Glib::ustring icon_name) = 0;
        virtual void set_description(Glib::ustring icon_name) = 0;
        //  virtual void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon) = 0;
        virtual void set_has_desktop_file(bool has) = 0;

        // getters
        virtual const guint32 get_xid() const = 0;
        virtual const guint32 get_hash() const = 0;
        virtual const Glib::ustring& get_window_name() const = 0;
        virtual const Glib::ustring& get_title() const = 0;
        virtual const Glib::ustring& get_instance_name() const = 0;
        virtual const Glib::ustring& get_group_name() const = 0;
        virtual const Glib::ustring& get_description() const = 0;
        virtual const Glib::ustring& get_desktop_file() const = 0;
        virtual const Glib::ustring& get_icon_name() const = 0;
        // virtual const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const = 0;
    };

    class DockItem : public IDockItem
    {
      public:
        DockItem(const Glib::ustring& instance_name, const Glib::ustring& group_name);
        ~DockItem();

        // implementations
        bool const has_desktop_file() const;

        // setters
        void set_xid(guint32 xid);
        void set_icon_name(Glib::ustring icon_name);
        //     void set_icon(Glib::RefPtr<Gdk::Pixbuf> icon);
        void set_has_desktop_file(bool has);
        void set_title(Glib::ustring title);
        void set_window_name(Glib::ustring window_name);
        void set_instance_name(Glib::ustring instance_name);
        void set_group_name(Glib::ustring group_name);
        void set_description(Glib::ustring description);
        void set_desktop_file(Glib::ustring desktop_file);

        // Getters
        const guint32 get_xid() const;
        const guint32 get_hash() const;
        const Glib::ustring& get_window_name() const;
        const Glib::ustring& get_title() const;
        const Glib::ustring& get_group_name() const;
        const Glib::ustring& get_description() const;
        const Glib::ustring& get_instance_name() const;
        const Glib::ustring& get_desktop_file() const;
        const Glib::ustring& get_icon_name() const;
        const Glib::RefPtr<Gdk::Pixbuf>& get_icon() const;

        const Glib::ustring to_string();
    };
}  // namespace docklight
