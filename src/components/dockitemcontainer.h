#pragma once
//  Copyright (c) 2018-2024 Juan R. González
//
//
//  This file is part of Docklight.
//
//  Docklight is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Docklight is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  public Glib::Object GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  identification number, along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <any>
#include <gdkmm/pixbuf.h>
#include <sigc++/sigc++.h>
#include <libbamf/libbamf.h>
#include <libwnck/libwnck.h>
#include <giomm/desktopappinfo.h>
#include <gtkmm/icontheme.h>

//#include "components/dockitem.h"
#include "components/dockitemicon.h"
#include "components/config.h"
#include "utils/singletonbase.h"
#include "utils/pixbuf.h"
// clang-format on

namespace docklight
{
    class MyClass
    {
      public:
    };

    template <typename T>
    void dockitem_any_cast_(std::any a, T* target)
    {
        if (!a.has_value()) return;

        try {
            if constexpr (std::is_pointer_v<T>) {
                *target = std::any_cast<typename std::remove_pointer_t<T>&>(a);
            } else {
                *target = std::any_cast<T&>(a);
            }
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Failed to cast value from std::any");
        }
    }

    template <typename T>
    void dockitem_any_cast_NOT_WORK2(std::any a, std::add_lvalue_reference_t<T>* target)
    {
        if (!a.has_value()) return;

        try {
            if constexpr (std::is_pointer_v<T>) {
                //                *target = std::any_cast<typename std::remove_pointer_t<T>&>(a);
                //                target = std::any_cast<std::shared_ptr<typename
                //                std::remove_pointer_t<T>>>(a);
                target = std::any_cast<std::shared_ptr<docklight::DockItemIcon>>(a);
            } else {
                *target = std::any_cast<T&>(a);
            }
        } catch (const std::bad_any_cast& e) {
            throw std::runtime_error("Failed to cast value from std::any");
        }
    }

    template <typename T>
    void dockitem_any_cast(std::any a, T target)
    {
        target = nullptr;
        if (!a.has_value()) return;

        try {
            if (a.type() == typeid(std::shared_ptr<DockItemIcon>)) {
                target = std::any_cast<std::shared_ptr<DockItemIcon>>(a);
                //                target = std::any_cast<T>(a);
            }
            /*if constexpr (std::is_pointer_v<T>) {
                // Handle pointer types
                // target = std::any_cast<typename std::remove_pointer_t<T>>(a);
                // target = std::any_cast<typename std::remove_pointer_t<T>&>(a);
                //                target = std::any_cast<std::remove_pointer_t<T>>(a);
                g_critical("  in side: \n");
                target = std::any_cast<std::shared_ptr<DockItemIcon>>(a);
                //       target = std::any_cast<std::shared_ptr<docklight::MyClass>>(a);
            A
            } else {
                // Handle non-pointer types
                target = std::any_cast<T>(a);
                // target = std::any_cast<std::shared_ptr<docklight::MyClass>>(a);
            }*/
        } catch (const std::bad_any_cast& e) {
            // Handle exception
            target = nullptr;

            g_critical(" !!!!!!!!!!!!!!!!!!dockitem_any_cast: %s\n", e.what());
        }
    }

    // template <typename T>
    // void dockitem_any_cast1(std::any a, T* target)
    //{
    // if (!a.has_value()) return;

    // try {
    // if constexpr (std::is_same_v<T, MyClass>) {
    //// Handle MyClass case
    // target = std::any_cast<MyClass*>(a);
    //} else if constexpr (std::is_same_v<T, Glib::RefPtr<MyClass>>) {
    //// Handle shared_ptr<MyClass> case
    // target = std::any_cast<Glib::RefPtr<MyClass>>(a);
    //}
    //} catch (const std::bad_any_cast& e) {
    //// Handle exception
    // target = nullptr;
    //}
    //}

    template <class T>
    // void dockitem_any_cast(std::any a, Glib::RefPtr<T>& target)
    //..void dockitem_any_cast(std::any a, Glib::RefPtr<T>& target)
    void dockitem_any_castX(std::any a, Glib::RefPtr<T>& target)
    {
        if (a.type() != typeid(T)) return;

        try {
            // auto refStore = std::any_cast<T>(a);
            // target = &RefPtr;  // std::dynamic_pointer_cast<T>(std::any_cast<T>(a));

            target = std::any_cast<T>(a);

            // target->reset(std::any_cast<T>(a));
            //  displaySingleResult(a);
            //             std::cout << "succeeded id: " << typeid(T).name << "\n";
            std::cout << "...........has type: " << typeid(T).name() << '\n';
            // std::cout << "succeeded in casting std::any of " << type_to_console(a.type())
            //<< "to: " << type_to_console(typeid(T));
        } catch (const std::bad_any_cast& e) {
            std::cout << "ANY ERROR....has type: " << typeid(T).name() << '\n';
            // std::cout << "ERROR....has type a: " << a.type() << std::endl;
            // std::cout << e.what() << ": tried to cast to " << typeid(T)
            // << std::endl;

            //          << "BUT actual type is: " << a.type() << std::endl;
        }
    }

    template <class T>
    void try_any_cast(std::any a)
    {
        try {
            std::any_cast<T>(a);
            // displaySingleResult(a);
            //            std::cout << "succeeded id: " << typeid(T).name << "\n";
            std::cout << "...........has type: " << typeid(T).name() << '\n';
            // std::cout << "succeeded in casting std::any of " << type_to_console(a.type())
            //<< "to: " << type_to_console(typeid(T));
        } catch (const std::bad_any_cast& e) {
            std::cout << "ERROR....has type: " << typeid(T).name() << '\n';
            // std::cout << "ERROR....has type a: " << a.type() << std::endl;
            // std::cout << e.what() << ": tried to cast to " << typeid(T)
            // << std::endl;

            //          << "BUT actual type is: " << a.type() << std::endl;
        }
    }
    enum window_action_t { OPEN, CLOSE, UPDATE };
    typedef sigc::signal<void, window_action_t, gint> type_signal_update;

    constexpr const guint DEF_MAX_ICON_SIZE = 128;

    class Container : public SingletonBase<Container>
    {
      public:
        std::map<gulong, std::any> m_map;
    };

    class DockItemContainer : public Glib::Object  //: public SingletonBase<DockItemContainer>
    {
      public:
        DockItemContainer();
        virtual ~DockItemContainer();

        void request_update_signal();
        //    guint count(guint additional_size) const
        guint required_size(guint additional_size);
        // const std::map<gulong, Glib::RefPtr<DockItemIcon>> get_appmap() const;
        const std::map<gulong, std::any> get_appmap();
        int remove(gulong xid);
        gulong exist(const Glib::ustring& group);

        bool exist(gulong xid);
        bool insert(WnckWindow* window);

        guint items_count();
        // signal accessor:
        type_signal_update signal_update();

      private:
        sigc::connection m_sigc;

        void on_theme_changed();
        bool get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf, Glib::ustring& title_name,
                            Glib::ustring& desktop_file, Glib::ustring& icon_name);

        bool insert(gulong xid, GdkPixbuf* gdkpixbuf, const Glib::ustring instance_name,
                    Glib::ustring group_name, const Glib::ustring window_name,
                    const Glib::ustring window_icon_name, bool icon_is_fallback,
                    WnckWindowType wintype);

        std::map<gulong, std::any> get_map()
        {
            //
            return Container::getInstance()->m_map;
        }

      private:
        //   std::map<gulong, std::any> m_map;
        //        std::map<gulong, std::any> m_appmap;
        type_signal_update m_signal_update;
        BamfMatcher* m_matcher = nullptr;
    };

    Glib::RefPtr<DockItemContainer> get_dockcontainer();

    //    DockItemContainer* get_dockcontainer();

}  // namespace docklight
