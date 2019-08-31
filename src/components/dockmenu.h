#pragma once

#include <glibmm/i18n.h>
#include <gtkmm/checkmenuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/separatormenuitem.h>
#include "common.h"

DL_NS_BEGIN

class DockMenu
{
  protected:
    DockMenu()
    {  // Home Menu
        m_home_menu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_home_menu_quit_item.set_label(_("Quit"));
        m_home_menu_showseparatorline_item.set_label(_("Show separator line"));
        m_home_menu_addseparator_item.set_label(_("Add separator"));

        m_home_menu.append(m_separatorMenuHome1);
        m_home_menu.append(m_home_menu_showseparatorline_item);
        m_home_menu.append(m_home_menu_addseparator_item);
        m_home_menu.append(m_separatorMenuHome0);
        m_home_menu.append(m_home_menu_quit_item);
        m_home_menu.show_all();

        // items menu
        m_item_menu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_item_menu_new.set_label(_("Open new"));
        m_item_menu_attach.set_label(_("Attach"));

        m_item_menu.append(m_item_menu_attach);
        m_item_menu.append(m_separatorMenuItem0);
        m_item_menu.append(m_item_menu_new);
        m_item_menu.show_all();

        // separator
        m_separator_menu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_separator_menu_attach.set_label(_("Attach"));
        m_separator_menu.append(m_separator_menu_attach);
        m_separator_menu.show_all();

        // m_item_menu_windowlist.set_halign(Gtk::Align::ALIGN_CENTER);
        // Home Menu
        /*m_HomeMenu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_AboutMenuItem.set_label(_("About"));
        m_AutohideMenuItem.set_label(_("Autohide"));
        m_HomeUnMinimizeAllWindowsMenuItem.set_label(_("Show all"));
        m_HomeMinimizeAllWindowsExceptActiveMenuItem.set_label(_("Minimize all
        except active")); m_HomeMinimizeAllWindowsMenuItem.set_label(_("Minimize
        all")); m_HomeCloseAllWindowsExceptActiveMenuItem.set_label(_("Close all
        except active")); m_HomeCloseAllWindowsMenuItem.set_label(_("Close
        all")); m_preferencesMenuItem.set_label(_("Preferences"));
        m_HomeAddSeparatotMenuItem.set_label(_("Add separator"));
        m_homeSessionGrp.set_label(_("Session-group"));
        m_HelpMenuItem.set_label(_("Help"));
        m_QuitMenuItem.set_label(_("Quit"));

        m_HomeMenu.append(m_AutohideMenuItem);
        m_HomeMenu.append(m_separatorMenuItem3);
        m_HomeMenu.append(m_AboutMenuItem);
        m_HomeMenu.append(m_HelpMenuItem);
        m_HomeMenu.append(m_separatorMenuItem2);
        m_HomeMenu.append(m_HomeAddSeparatotMenuItem);
        m_HomeMenu.append(m_homeSessionGrp);
        m_HomeMenu.append(m_separatorMenuItem1);
        m_HomeMenu.append(m_HomeUnMinimizeAllWindowsMenuItem);
        m_HomeMenu.append(m_HomeMinimizeAllWindowsExceptActiveMenuItem);
        m_HomeMenu.append(m_HomeMinimizeAllWindowsMenuItem);
        m_HomeMenu.append(m_HomeCloseAllWindowsExceptActiveMenuItem);
        m_HomeMenu.append(m_HomeCloseAllWindowsMenuItem);
        m_HomeMenu.append(m_separatorMenuItem0);
        m_HomeMenu.append(m_preferencesMenuItem);
        m_HomeMenu.append(m_QuitMenuItem);

        m_HomeMenu.show_all();

        // Item Menu
        m_ItemMenu.set_halign(Gtk::Align::ALIGN_CENTER);
        m_MenuItemDetach.set_label(_("Detach from Dock"));
        m_MenuItemAttach.set_label(_("Attach to Dock"));
        m_MenuItemNewApp.set_label(_("Open new"));
        m_MenuItemCloseAll.set_label(_("Close all"));
        m_MenuItemCloseAllExceptActive.set_label(_("Close all except active"));
        m_MenuItemMinimizedAll.set_label(_("Minimize all"));
        m_MenuItemMinimizedAllExceptActive.set_label(_("Minimize all except
        active")); m_MenuItemUnMinimizedAll.set_label(_("Show all"));

        m_ItemMenu.append(m_MenuItemUnMinimizedAll);
        m_ItemMenu.append(m_MenuItemMinimizedAllExceptActive);
        m_ItemMenu.append(m_MenuItemMinimizedAll);
        m_ItemMenu.append(m_MenuItemCloseAllExceptActive);
        m_ItemMenu.append(m_MenuItemCloseAll);
        m_ItemMenu.append(m_separatorMenuItem5);
        m_ItemMenu.append(m_MenuItemDetach);
        m_ItemMenu.append(m_MenuItemAttach);
        m_ItemMenu.append(m_separatorMenuItem4);
        m_ItemMenu.append(m_MenuItemNewApp);

        m_ItemMenu.show_all();


        m_previewLimitMenu.set_halign(Gtk::Align::ALIGN_CENTER);*/
    }

    // Home menu
    Gtk::Menu m_home_menu;
    Gtk::MenuItem m_home_menu_quit_item;
    Gtk::CheckMenuItem m_home_menu_showseparatorline_item;
    Gtk::MenuItem m_home_menu_addseparator_item;

    // Item menu
    Gtk::Menu m_item_menu;
    Gtk::Menu* m_item_menu_windowlist = nullptr;

    Gtk::MenuItem m_item_menu_new;
    Gtk::CheckMenuItem m_item_menu_attach;

    // User separator
    Gtk::Menu m_separator_menu;
    Gtk::CheckMenuItem m_separator_menu_attach;

    Gtk::SeparatorMenuItem m_separatorMenuHome0;
    Gtk::SeparatorMenuItem m_separatorMenuHome1;

    Gtk::SeparatorMenuItem m_separatorMenuItem0;

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
