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
#include "utils/factory.h"
// clang-format on

namespace docklight
{
    class MyClass
    {
      public:
    };

    /*template <typename T>
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
    }*/

    /*template <typename T>
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
    }*/

    /*template <typename T>
    void dockitem_any_cast(std::any a, T target)
    {
        target = nullptr;
        if (!a.has_value()) return;

        try {
            if (a.type() == typeid(std::shared_ptr<DockItemIcon>)) {
                target = std::any_cast<std::shared_ptr<DockItemIcon>>(a);
                //                target = std::any_cast<T>(a);
            }
            [>if constexpr (std::is_pointer_v<T>) {
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
            }<]
        } catch (const std::bad_any_cast& e) {
            // Handle exception
            target = nullptr;

            g_critical(" !!!!!!!!!!!!!!!!!!dockitem_any_cast: %s\n", e.what());
        }
    }*/

    // WORKS LA BUENA +++
    template <typename T>
    bool dockitem_any_cast(std::any a, T& target)
    {
        bool result = false;
        if (!a.has_value()) return result;

        if (a.type() == typeid(T)) {
            target = std::any_cast<T>(a);

            result = target ? true : false;
        }

        return false;

        // try {
        //[>if constexpr (std::is_pointer_v<T>) {
        //// Handle pointer types
        //// target = std::any_cast<typename std::remove_pointer_t<T>>(a);
        //// target = std::any_cast<typename std::remove_pointer_t<T>&>(a);
        ////                target = std::any_cast<std::remove_pointer_t<T>>(a);
        // g_critical("  in side: \n");
        // target = std::any_cast<std::shared_ptr<DockItemIcon>>(a);
        ////       target = std::any_cast<std::shared_ptr<docklight::MyClass>>(a);
        // A
        // } else {
        //// Handle non-pointer types
        // target = std::any_cast<T>(a);
        //// target = std::any_cast<std::shared_ptr<docklight::MyClass>>(a);
        //}*/
        //} catch (const std::bad_any_cast& e) {
        //// Handle exception
        // target = nullptr;

        // g_critical(" !!!!!!!!!!!!!!!!!!dockitem_any_cast: %s\n", e.what());
        //}
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

    enum window_action_t { OPEN, CLOSE, UPDATE };
    typedef sigc::signal<void, window_action_t, gint> type_signal_update;

    constexpr const guint DEF_MAX_ICON_SIZE = 128;

    // bool byxid(std::pair<gulong, std::any> p)
    //{
    // g_print("%lu %lu\n", p.first, m_xid);
    // return true;  //]p.first == xid;

    //// return ((i%2)==1);
    //}
    // template <class T>
    class Container  //: public SingletonBase<Container>
    {
      public:
        guint size()
        {
            //
            return m_dockitems.size();
        }

        template <typename T>
        guint count()
        {
            guint count = m_dockitems.size();
            std::shared_ptr<T> dockitem;

            for (auto it = m_dockitems.begin(); it != m_dockitems.end(); it++) {
                std::any a = it->second;
                if (!factory::any_cast<std::shared_ptr<T>>(a, dockitem)) continue;

                count += dockitem->get_container_size();
            }
            return count;
            // auto byxid = [&xid](std::pair<gulong, std::any> p) { return p.first == xid; };
            // if (auto it = std::find_if(begin(m_dockitems), end(m_dockitems), byxid);
            // it != std::end(m_dockitems)) {
            ////
            //*it.
            ////   std::cout << "w contains an even number " << *it << '\n';
            //}
            // if ((auto it = std::find_if(m_dockitems.begin(), m_dockitems.end(), byxid)))
            // std::cout << "The first value is: " << *it << '\n';

            // if (auto std::vector<std::pair<gulong, std::any>>::iterator it =
            // std::find_if(m_dockitems.begin(), m_dockitems.end(), byxid)) {
            ////               std::cout << "The first value is: " << *it << '\n';
            //}
        }

        template <typename T>
        void get(gulong xid)
        {
            std::shared_ptr<T> dockitem;  // = get_map().at(cxid);
            std::any a;                   // std::shared_ptr<T> dockitem;  // = get_map().at(cxid);
            auto byxid = [&xid, &a](std::pair<gulong, std::any> p) {
                if (p.first != xid) return false;

                a = p.second;

                return a.has_value() ? true : false;
            };

            if (auto it = std::find_if(begin(m_dockitems), end(m_dockitems), byxid);
                it != std::end(m_dockitems)) {
                std::cout << " FOUNDS !!!!!!!!!!!dn get oes contain  the xid   \n" << std::endl;
                return;
            } else {
                g_print("NOT FOUND \n");
            }
        }

        void add(gulong xid, std::any a)
        {
            // creates the pair and the insert it to the list.
            m_dockitems.push_back(std::make_pair(xid, a));
        }

        bool exist(gulong xid)
        {
            auto byxid = [&xid](std::pair<gulong, std::any> p) { return p.first == xid; };
            // if (auto it = std::find_if(begin(w), end(w), is_even); it != std::end(w))
            // std::cout << "w contains an even number " << *it << '\n';
            // else
            // std::cout << "w does not contain even numbers\n";

            if (auto it = std::find_if(begin(m_dockitems), end(m_dockitems), byxid);
                it != std::end(m_dockitems)) {
                std::cout << " does contain  the xid   \n" << std::endl;
                return true;
            } else {
                g_print("NOT FOUND \n");
            }
            // if( auto std::vector<std::pair<gulong, std::any>>::iterator it =
            //            std::find_if(m_dockitems.begin(), m_dockitems.end(), byxid);
            //    std::cout << "The first value is: " << *it.first << '\n';

            //  std::cout << "The first odd value is " << *it.first << '\n';
            //
            return false;
        }

      private:
        std::vector<std::pair<gulong, std::any>> m_dockitems;
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
        //          const std::map<gulong, std::any> get_appmap();
        int remove(gulong xid);
        gulong exist(const Glib::ustring& group);

        bool exist(gulong xid);
        bool insert(WnckWindow* window);

        guint items_count();
        // signal accessor:
        type_signal_update signal_update();

        std::map<gulong, std::any>& get_map()
        {
            //  return m_container.m_map;
            return m_map;
            //  return Container::getInstance()->m_map;
        }

      private:
        sigc::connection m_sigc;

        void on_theme_changed();
        bool get_window_icon(GdkPixbuf* gdkpixbuf, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf);
        bool get_theme_icon(guint xid, Glib::RefPtr<Gdk::Pixbuf>& pixbuf, Glib::ustring& title_name,
                            Glib::ustring& desktop_file, Glib::ustring& icon_name);

        bool insert(gulong xid, GdkPixbuf* gdkpixbuf, const Glib::ustring instance_name,
                    std::string group_name, const Glib::ustring window_name,
                    const Glib::ustring window_icon_name, bool icon_is_fallback,
                    WnckWindowType wintype);

      private:
        Container m_container;
        // std::unordered_map<gulong, std::any> m_map;
        std::map<gulong, std::any> m_map;
        //         std::map<gulong, std::any> m_appmap;
        type_signal_update m_signal_update;
        BamfMatcher* m_matcher = nullptr;
    };

    Glib::RefPtr<DockItemContainer> get_dockcontainer();

    //    DockItemContainer* get_dockcontainer();

}  // namespace docklight
