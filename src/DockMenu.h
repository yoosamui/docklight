
/*
 * File:   DockPanel.h
 * Author: yoo
 *
 * Created on November 6, 2018, 7:31 PM
 */

#ifndef DOCKMENU_H
#define	DOCKMENU_H

#include <gtkmm.h>
#include <glibmm/i18n.h>

class DockMenu
{
    protected:
        DockMenu()
        {
            // Home Menu
            m_HomeMenu.set_halign(Gtk::Align::ALIGN_CENTER);
            m_AboutMenuItem.set_label(_("About"));
            m_AutohideMenuItem.set_label(_("Autohide"));
            m_HomeUnMinimizeAllWindowsMenuItem.set_label(_("Show all"));
            m_HomeMinimizeAllWindowsExceptActiveMenuItem.set_label(_("Minimize all except active"));
            m_HomeMinimizeAllWindowsMenuItem.set_label(_("Minimize all"));
            m_HomeCloseAllWindowsExceptActiveMenuItem.set_label(_("Close all except active"));
            m_HomeCloseAllWindowsMenuItem.set_label(_("Close all"));
            m_preferencesMenuItem.set_label(_("Preferences"));
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
            m_MenuItemMinimizedAllExceptActive.set_label(_("Minimize all except active"));
            m_MenuItemUnMinimizedAll.set_label(_("Show all"));

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


            m_previewLimitMenu.set_halign(Gtk::Align::ALIGN_CENTER);

        }

        // Home menu
        Gtk::Menu m_HomeMenu;
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
       // Gtk::MenuItem m_previewLimitMenuItem;

};

#endif	/* DOCKMENU_H */
