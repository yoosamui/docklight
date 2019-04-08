
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
    public:
        DockMenu()
        {
            // Home Menu
            m_HomeMenu.set_halign(Gtk::Align::ALIGN_CENTER);
            m_QuitMenuItem.set_label(_("Quit"));
            m_HomeMenu.append(m_QuitMenuItem);
            m_HomeMenu.show_all();
        
            // Item Menu 
            m_ItemMenu.set_halign(Gtk::Align::ALIGN_CENTER);
            m_MenuItemDetach.set_label(_("Detach from Dock"));
            m_MenuItemAttach.set_label(_("Attach to Dock"));
            m_MenuItemNewApp.set_label(_("Open new"));
            m_ItemMenu.append(m_MenuItemDetach);
            m_ItemMenu.append(m_MenuItemAttach);
            m_ItemMenu.append(m_separatorMenuItem0);
            m_ItemMenu.append(m_MenuItemNewApp);
            m_ItemMenu.show_all();
        
        }

    protected:

        // Home menu
        Gtk::Menu m_HomeMenu;
        Gtk::MenuItem m_QuitMenuItem;

        // Item menu
        Gtk::Menu m_ItemMenu;
        Gtk::MenuItem m_MenuItemNewApp;
        Gtk::MenuItem m_MenuItemAttach;
        Gtk::MenuItem m_MenuItemDetach;

        Gtk::SeparatorMenuItem m_separatorMenuItem0;
};

#endif	/* DOCKMENU_H */
