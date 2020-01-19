#pragma once

//#include "utils/position.h"
#include <glibmm/i18n.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/separatormenuitem.h>
#include "common.h"
#include "components/panelbase.h"
//#include "utils/position.h"
#include "utils/wnck.h"
DL_NS_BEGIN

class Dock_menu : public Panel_base
{
  public:
    Dock_menu();

  protected:
    bool m_context_menu_open = false;
    // Home menu
    Gtk::Menu m_home_menu;
    Gtk::MenuItem m_home_menu_quit_item;
    Gtk::MenuItem m_home_menu_addseparator_item;
    Gtk::MenuItem m_home_menu_addexpander_item;
    Gtk::MenuItem m_home_menu_close_all_item;

    // Item menu
    Gtk::Menu m_item_menu;
    Gtk::Menu* m_item_menu_windowlist = nullptr;

    Gtk::MenuItem m_item_menu_new;
    Gtk::CheckMenuItem m_item_menu_attach;
    Gtk::MenuItem m_item_menu_close_all;
    Gtk::MenuItem m_item_menu_minimize_all;
    Gtk::MenuItem m_item_menu_unminimize_all;

    // User separator
    Gtk::Menu m_separator_menu;
    Gtk::CheckMenuItem m_separator_menu_attach;

    Gtk::SeparatorMenuItem m_separatorMenuHome0;
    Gtk::SeparatorMenuItem m_separatorMenuHome1;

    Gtk::SeparatorMenuItem m_separatorMenuItem0;
    Gtk::SeparatorMenuItem m_separatorMenuItem1;

    // events
    virtual void on_menu_hide_event() {}
    virtual void on_menu_show_event() {}
    virtual void on_home_menu_quit_event() {}
    virtual void on_item_menu_new_event() {}

    void on_home_menu_close_all_event();

    virtual void on_home_menu_addseparator_event()
    {
        appinfo_t info;
        info.m_separator_length = config::get_separator_size();
        info.m_name = "separator";
        info.m_title = _("Separator");

        m_appupdater.m_dockitems.push_back(
            shared_ptr<DockItem>(new DockItem(info, dock_item_type_t::separator)));
        auto const new_item = m_appupdater.m_dockitems.back();
        new_item->set_attach(true);

        m_appupdater.save();
    }

    virtual void on_home_menu_addexpander_event()
    {
        appinfo_t info;
        info.m_separator_length = config::get_separator_size();
        info.m_name = "expander";
        info.m_title = _("Expander");

        m_appupdater.m_dockitems.push_back(
            shared_ptr<DockItem>(new DockItem(info, dock_item_type_t::expander)));
        auto const new_item = m_appupdater.m_dockitems.back();

        new_item->set_attach(true);

        m_appupdater.save();
    }

    void on_item_menu_attach_event()
    {
        bool attached = m_item_menu_attach.get_active();
        if (attached) {
            m_appupdater.attach_item(m_current_index);
        } else {
            m_appupdater.detach_item(m_current_index);
        }
    }

    void on_separator_menu_attach_event()
    {
        bool attached = m_separator_menu_attach.get_active();
        if (attached) {
            m_appupdater.attach_item(m_current_index);
        } else {
            m_appupdater.detach_item(m_current_index);
        }
    }

    void on_item_menu_close_group_event()
    {
        // close group
        wnck_util::close_group_windows(m_current_index);
    }

    void on_item_menu_minimize_all_event();
    void on_item_menu_unminimize_all_event();

    virtual void on_separator_menu_position(int& x, int& y, bool& push_in);
    virtual void on_home_menu_position(int& x, int& y, bool& push_in);
    virtual void on_item_menu_position(int& x, int& y, bool& push_in);
    virtual void on_item_menu_windowlist_position(int& x, int& y, bool& push_in);

    // Home menu
    /*Gtk::Menu m_HomeMenu;
    Gtk::CheckMenuItem m_AutohideMenuItem;
    Gtk::MenuItem m_QuitMenuItem;
    Gtk::MenuItem m_HomeCloseAllWindowsMenuItem;
    Gtk::MenuItem m_HomeCloseAllWindowsExceptActiveMenuItem;
    Gtk::MenuItem m_HomeMinimizeAllWindowsMenuItem;
    Gtk::MenuItem m_HomeUnMinimizeAllWindowsMenuItem;
    Gtk::MenuItem m_HomeMinimizeAllWindowsExceptActiveMenuItem;
    Gtk::MenuItem m_homeSessionGrp;
    Gtk::MenuItem m_HelpMenuItem;
    Gtk::MenuItem m_AboutMenuItem;
    Gtk::MenuItem m_preferencesMenuItem;
    Gtk::MenuItem m_HomeAddSeparatotMenuItem;

    // Item menu
    Gtk::Menu m_ItemMenu;
    Gtk::MenuItem m_MenuItemNewApp;
    Gtk::MenuItem m_MenuItemAttach;
    Gtk::MenuItem m_MenuItemDetach;
    Gtk::MenuItem m_MenuItemCloseAll;
    Gtk::MenuItem m_MenuItemCloseAllExceptActive;
    Gtk::MenuItem m_MenuItemUnMinimizedAll;
    Gtk::MenuItem m_MenuItemMinimizedAllExceptActive;
    Gtk::MenuItem m_MenuItemMinimizedAll;

    Gtk::SeparatorMenuItem m_separatorMenuItem0;
    Gtk::SeparatorMenuItem m_separatorMenuItem1;
    Gtk::SeparatorMenuItem m_separatorMenuItem2;
    Gtk::SeparatorMenuItem m_separatorMenuItem3;
    Gtk::SeparatorMenuItem m_separatorMenuItem4;
    Gtk::SeparatorMenuItem m_separatorMenuItem5;
    Gtk::SeparatorMenuItem m_separatorMenuItem6;
    Gtk::SeparatorMenuItem m_separatorMenuItem7;
    Gtk::SeparatorMenuItem m_separatorMenuItem8;
    Gtk::SeparatorMenuItem m_separatorMenuItem9;


    Gtk::Menu m_previewLimitMenu;
    // Gtk::MenuItem m_previewLimitMenuItem;*/
};

DL_NS_END
