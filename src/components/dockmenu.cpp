#include "dockmenu.h"
#include "components/device.h"
#include "utils/position.h"
DL_NS_BEGIN

Dock_menu::Dock_menu()
{
    m_home_menu.set_halign(Gtk::Align::ALIGN_CENTER);
    m_home_menu_quit_item.set_label(_("Quit"));
    m_home_menu_addseparator_item.set_label(_("Add separator"));
    m_home_menu_addexpander_item.set_label(_("Add expander"));
    m_home_menu_close_all_item.set_label(_("Close all"));

    m_home_menu.append(m_home_menu_close_all_item);
    m_home_menu.append(m_separatorMenuHome1);
    m_home_menu.append(m_home_menu_addexpander_item);
    m_home_menu.append(m_home_menu_addseparator_item);
    m_home_menu.append(m_separatorMenuHome0);
    m_home_menu.append(m_home_menu_quit_item);
    m_home_menu.show_all();

    // items menu
    m_item_menu.set_halign(Gtk::Align::ALIGN_CENTER);
    m_item_menu_unminimize_all.set_label(_("Show all"));
    m_item_menu_minimize_all.set_label(_("Minimize all"));
    m_item_menu_close_all.set_label(_("Close all"));
    m_item_menu_new.set_label(_("Open new"));
    m_item_menu_attach.set_label(_("Attach"));

    m_item_menu.append(m_item_menu_unminimize_all);
    m_item_menu.append(m_item_menu_minimize_all);
    m_item_menu.append(m_item_menu_close_all);
    m_item_menu.append(m_separatorMenuItem1);
    m_item_menu.append(m_item_menu_attach);
    m_item_menu.append(m_separatorMenuItem0);
    m_item_menu.append(m_item_menu_new);
    m_item_menu.show_all();

    // separator
    m_separator_menu.set_halign(Gtk::Align::ALIGN_CENTER);
    m_separator_menu_attach.set_label(_("Attach"));
    m_separator_menu.append(m_separator_menu_attach);
    m_separator_menu.show_all();

    // clang-format off
        // home menu
        // m_home_menu.attach_to_widget(*this);
        // m_home_menu.accelerate(*this);
        m_home_menu.signal_show().connect(sigc::mem_fun(*this, &Dock_menu::on_menu_show_event));
        m_home_menu.signal_hide().connect(sigc::mem_fun(*this, &Dock_menu::on_menu_hide_event));

        m_home_menu_quit_item.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_home_menu_quit_event));
        m_home_menu_addseparator_item.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_home_menu_addseparator_event));
        m_home_menu_addexpander_item.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_home_menu_addexpander_event));
        m_home_menu_close_all_item.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_home_menu_close_all_event));


        // user separator
        //    m_separator_menu.attach_to_widget(*this);
        // m_separator_menu.accelerate(*this);
        //
        m_separator_menu.signal_show().connect(sigc::mem_fun(*this, &Dock_menu::on_menu_show_event));
        m_separator_menu.signal_hide().connect(sigc::mem_fun(*this, &Dock_menu::on_menu_hide_event));

        m_separator_menu_attach.set_active(false);
        m_separator_menu_attach.signal_toggled().connect(sigc::mem_fun(*this, &Dock_menu::on_separator_menu_attach_event));

        // items menu
        // m_item_menu.attach_to_widget(*this);
        // m_item_menu.accelerate(*this);
        m_item_menu.signal_show().connect(sigc::mem_fun(*this, &Dock_menu::on_menu_show_event));
        m_item_menu.signal_hide().connect(sigc::mem_fun(*this, &Dock_menu::on_menu_hide_event));

        m_item_menu_close_all.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_item_menu_close_group_event));

        m_item_menu_attach.set_active(false);
        m_item_menu_attach.signal_toggled().connect(sigc::mem_fun(*this, &Dock_menu::on_item_menu_attach_event));
        m_item_menu_new.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_item_menu_new_event));
        m_item_menu_minimize_all.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_item_menu_minimize_all_event));
        m_item_menu_unminimize_all.signal_activate().connect(sigc::mem_fun(*this, &Dock_menu::on_item_menu_unminimize_all_event));

    // clang-format on
    //
    // m_item_menu_windowlist.set_halign(Gtk::Align::ALIGN_CENTER);
    // Home Menu
    /*m_HomeMenu.set_halign(Gtk::Align::ALIGN_CENTER);
    m_AboutMenuItem.set_label(_("About"));
    m_AutohideMenuItem.set_label(_("Autohide"));
    m_HomeUnMinimizeAllWindowsMenuItemMinimizeAllWindowsMenuItem.set_label(_("Show all"));
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

void Dock_menu::on_item_menu_minimize_all_event()
{
    int index = Panel_base::get_current_index();
    if (index == -1) return;

    wnck_util::minimize_group(index);
}

void Dock_menu::on_item_menu_unminimize_all_event()
{
    int index = Panel_base::get_current_index();
    if (index == -1) return;

    wnck_util::unminimize_group(index);
}

void Dock_menu::on_home_menu_close_all_event()
{
    wnck_util::close_all_windows();
}

void Dock_menu::on_separator_menu_position(int& x, int& y, bool& push_in)
{
    int monitor_number = device::monitor::get_monitor_number();
    m_home_menu.set_monitor(monitor_number);

    position_util::get_center_position(m_current_index, x, y, m_separator_menu.get_width(),
                                       m_separator_menu.get_height());
}

void Dock_menu::on_home_menu_position(int& x, int& y, bool& push_in)
{
    int monitor_number = device::monitor::get_monitor_number();
    m_home_menu.set_monitor(monitor_number);

    position_util::get_center_position(m_current_index, x, y, m_home_menu.get_width(),
                                       m_home_menu.get_height());
}

void Dock_menu::on_item_menu_position(int& x, int& y, bool& push_in)
{
    int monitor_number = device::monitor::get_monitor_number();
    m_item_menu.set_monitor(monitor_number);

    position_util::get_center_position(m_current_index, x, y, m_item_menu.get_width(),
                                       m_item_menu.get_height());
}

void Dock_menu::on_item_menu_windowlist_position(int& x, int& y, bool& push_in)
{
    int monitor_number = device::monitor::get_monitor_number();
    m_item_menu_windowlist->set_monitor(monitor_number);

    position_util::get_center_position(m_current_index, x, y, m_item_menu_windowlist->get_width(),
                                       m_item_menu_windowlist->get_height());
}

DL_NS_END
