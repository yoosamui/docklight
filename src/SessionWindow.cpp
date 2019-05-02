//*****************************************************************
//
//  Copyright (C) 2015-2017 Juan R. Gonzalez
//  Created on February 21, 2017
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//****************************************************************

#include "SessionWindow.h"
#include "DockPanel.h"
#include "Launcher.h"
#include "DockWindow.h"
#include <glibmm/i18n.h>
#include <cstdio>

#include <fstream>
#include <iostream>


WnckWindow* SessionWindow::m_activeWindow;
SessionWindow::type_signal_getactive SessionWindow::m_signal_getactive;
bool SessionWindow::m_deleteSet;

ListRow::ListRow(
        const Glib::ustring& appname,
        const Glib::ustring& parameters,
        const Glib::ustring& titlename,
        Glib::RefPtr<Gdk::Pixbuf> appIcon)
:
m_HBox(Gtk::ORIENTATION_HORIZONTAL),
m_VBox(Gtk::ORIENTATION_VERTICAL),
m_HSeparator(Gtk::ORIENTATION_HORIZONTAL),
m_appname(appname),
m_image(appIcon),
m_frame(),
m_launchButton(_("Launch")),
m_removeButton(_("Remove")),
m_grid(),
m_titlename(titlename)
{
    if (!appname.empty()) {


        m_parameters.set_text(parameters);
        m_parameters.set_max_length(MAX_INPUT);


        m_HBox.set_margin_left(6);
        m_HBox.set_margin_right(6);
        m_HBox.set_margin_top(6);
        m_HBox.set_margin_bottom(6);

        m_HSeparator.set_margin_left(6);
        m_HSeparator.set_margin_right(6);

        m_grid.set_row_spacing(4);
        m_grid.set_column_spacing(4);

        this->add(m_frame);
        this->m_frame.add(m_HBox);

        this->m_HBox.add(m_image);
        this->m_HBox.add(m_grid);
        this->m_HBox.add(m_HSeparator);
        this->m_HBox.add(m_VBox);


        this->m_VBox.add(m_launchButton);
        this->m_VBox.add(m_removeButton);


        this->m_parameters.set_hexpand(true);
        this->m_appname.set_halign(Gtk::Align::ALIGN_START);
        this->m_appname.set_markup("<big>" + this->m_appname.get_text() + "</big>");

        this->m_titlename.set_halign(Gtk::Align::ALIGN_START);


        this->m_grid.attach(m_appname, 0, 0, 1, 1);
        this->m_grid.attach(m_titlename, 0, 1, 1, 1);
        this->m_grid.attach(m_parameters, 0, 2, 2, 1);

        m_launchButton.signal_clicked().connect(sigc::bind<ListRow&>(
                sigc::mem_fun(*this, &ListRow::on_launch_button_clicked), *this));

        m_removeButton.signal_clicked().connect(sigc::bind<ListRow&>(
                sigc::mem_fun(*this, &ListRow::on_remove_button_clicked), *this));



    }
}

void ListRow::on_launch_button_clicked(ListRow& row)
{
    Launcher::Launch(row.get_appname().c_str(),
            row.get_parameters().c_str());
}

void ListRow::on_remove_button_clicked(ListRow& row)
{
    row.remove();
    delete &row;
}

SessionWindow::SessionWindow()
: Gtk::Window(Gtk::WindowType::WINDOW_TOPLEVEL),
m_VBox(Gtk::ORIENTATION_VERTICAL),
m_HBox(Gtk::ORIENTATION_HORIZONTAL),
m_HBoxBottom(Gtk::ORIENTATION_HORIZONTAL),
m_labelAddActive(_("Application")),
m_AddToGroupButton(_("Add to list")),
m_OkButton(_("Save")),
m_CancelButton(_("Cancel")),
m_labelSessionName(_("Session Name")),
m_ScrolledWindow(),
m_ListBox()
{

    m_deleteSet = false;

    set_border_width(10);
    set_default_size(640, 550);

    this->set_resizable(true);
    this->set_keep_above(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);

    m_VBox.set_margin_left(6);
    m_VBox.set_margin_right(6);
    m_VBox.set_margin_top(6);
    m_VBox.set_margin_bottom(6);

    m_EntryAppName.set_max_length(60);
    m_EntryAppName.set_hexpand(true);
    m_EntryAppName.set_sensitive(false);

    m_grid.set_row_spacing(4);
    m_grid.set_column_spacing(4);
    m_grid.set_hexpand(true);

    m_ListBox.set_vexpand_set(true);
    m_ScrolledWindow.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_ScrolledWindow.add(m_ListBox);

    add(m_VBox);
    m_VBox.add(m_grid);
    m_VBox.add(m_HBox);

    m_VBox.pack_start(m_ScrolledWindow, true, true, Gtk::PACK_EXPAND_WIDGET);
    m_VBox.add(m_HBoxBottom);

    m_HBoxBottom.set_margin_top(10);
    m_HBoxBottom.set_margin_bottom(10);

    m_HBox.set_margin_top(10);
    m_HBox.set_margin_bottom(10);


    m_AddToGroupButton.set_halign(Gtk::Align::ALIGN_END);
    m_HBox.pack_start(m_AddToGroupButton, Gtk::PACK_EXPAND_WIDGET, 0);

    m_grid.attach(m_labelAddActive, 0, 0, 1, 1);
    m_grid.attach(m_EntryAppName, 1, 0, 1, 1);

    m_grid.attach(m_labelSessionName, 0, 1, 1, 1);
    m_SessionName.set_max_length(60);
    m_grid.attach(m_SessionName, 1, 1, 1, 1);


    m_HBoxBottom.pack_start(m_OkButton, true, true, Gtk::PACK_EXPAND_WIDGET);
    m_HBoxBottom.pack_start(m_CancelButton, true, true, Gtk::PACK_EXPAND_WIDGET);


    m_AddToGroupButton.signal_clicked().connect(sigc::bind<guint>(
            sigc::mem_fun(*this, &SessionWindow::on_button_clicked), DEF_BUTTON_ADDTOLIST));

    m_OkButton.signal_clicked().connect(sigc::bind<guint>(
            sigc::mem_fun(*this, &SessionWindow::on_button_clicked), DEF_BUTTON_OK));

    m_CancelButton.signal_clicked().connect(sigc::bind<guint>(
            sigc::mem_fun(*this, &SessionWindow::on_button_clicked), DEF_BUTTON_CANCEL));


    // Gets the default WnckScreen on the default display.
    WnckScreen *wnckscreen = wnck_screen_get_default();
    g_signal_connect(wnckscreen, "active_window_changed",
            G_CALLBACK(SessionWindow::on_active_window_changed_callback), NULL);

    signal_getactive().connect(sigc::mem_fun(*this,
            &SessionWindow::on_signal_getactive));

}

SessionWindow::~SessionWindow()
{
    auto children = m_ListBox.get_children();
    for (int i = children.size() - 1; i >= 0; i--) {
        ListRow* row = (ListRow*) m_ListBox.get_row_at_index(i);
        delete row;
    }
    g_print("SessionWindow relesed\n");
}

void SessionWindow::init(DockPanel& panel, DockItem* dockitem, const int id)
{
    this->m_panel = &panel;
    this->m_dockitem = dockitem;

    char buff[100];
    sprintf(buff, _("Session group %d"), id);
    this->set_title(buff);

    sprintf(buff, "Session-group-%d", id);
    m_sessiongrpname = buff;
    m_deleteSet = false;
    std::vector<ListRow*> rows;
    size_t result;
    FILE* f;
    f = fopen(getFilePath().c_str(), "rb");
    if (!f) {
        return;
    }

    struct sessionGrpData st;


    while (1) {
        result = fread(&st, sizeof (st), 1, f);

        if (feof(f) != 0)
            break;

        if (result == 0)
            g_critical("SessionWindow::init: Error reading file> fread\n");

        auto loader = Gdk::PixbufLoader::create();
        loader->write(st.pixbuff, sizeof (st.pixbuff));
        Glib::RefPtr< Gdk::Pixbuf > pixbuf = loader->get_pixbuf();

        auto row = Gtk::manage(new ListRow(
                st.appname,
                st.parameters,
                st.titlename,
                pixbuf));

        rows.push_back(row);


        try {
            loader->close();
        } catch (Gdk::PixbufError pe) {
            g_critical("SessionWindow::init: Gdk::PixbufError\n");
        } catch (Glib::FileError fe) {
            g_critical("SessionWindow::init: Glib::FileError\n");
        }

    }

    fclose(f);


    std::ifstream file(getTitleFilePath());
    std::string title;
    if(file.is_open())
    {
        std::getline(file,title);
        m_SessionName.set_text( title );
        file.close();
    }


    // Sort by appname
    int size = (int) rows.size();
    int i, m, j;

    for (i = 0; i < size - 1; i = i + 1) {
        m = i;
        for (j = i + 1; j < size; j = j + 1) {

            std::string s1 = rows.at(j)->get_appname().c_str();
            std::string s2 = rows.at(m)->get_appname().c_str();

            std::string a = Utilities::stringToLower(s1.c_str());
            std::string b = Utilities::stringToLower(s2.c_str());

            if (a < b) {
                m = j;
            }
        }
        std::swap(rows.at(i), rows.at(m));
    }

    for(auto row : rows)
        m_ListBox.append(*row);

    m_ListBox.show_all();
}

bool SessionWindow::on_delete_event(GdkEventAny* event)
{
    m_deleteSet = true;

   // this->m_panel->m_sessionWindow = nullptr;
   // delete(this);

    return false;
}

void SessionWindow::on_button_clicked(guint buttonId)
{
    switch (buttonId)
    {
        case DEF_BUTTON_ADDTOLIST: this->addToList();
            break;
        case DEF_BUTTON_OK: this->save();
            break;
        case DEF_BUTTON_CANCEL: this->close();
            break;
    }
}

void SessionWindow::addToList()
{

    if( !WNCK_IS_WINDOW(m_window))
        return;

    Glib::RefPtr<Gdk::Pixbuf> appIcon = NULLPB;

    std::string the_appname;
    std::string the_instancename;
    std::string the_groupname;
    std::string the_titlename;

    if (Launcher::getAppNameByWindow(m_window,
            the_appname,
            the_instancename,
            the_groupname,
            the_titlename) == FALSE) {
        return;
    }

    int iconsize = Configuration::get_dockWindowSize() - CELL_WIDTH_MARGIN;
    appIcon = IconLoader::GetWindowIcon(m_window);
    appIcon = appIcon->scale_simple(iconsize,
            iconsize, Gdk::INTERP_BILINEAR);

    auto row = Gtk::manage(new ListRow(
            m_EntryAppName.get_text(), "", the_appname, appIcon));

    m_ListBox.append(*row);
    m_ListBox.show_all();
}

Glib::ustring SessionWindow::getFilePath()
{
    char filename[PATH_MAX];
    std::string thispath = Utilities::getExecPath();
    sprintf(filename, "%s/%s/%s.bin", thispath.c_str(),
            DEF_ATTACHMENTDIR, m_sessiongrpname.c_str());

    return filename;
}

Glib::ustring SessionWindow::getTitleFilePath()
{
    char filename[PATH_MAX];
    std::string thispath = Utilities::getExecPath();
    sprintf(filename, "%s/%s/%s.txt", thispath.c_str(),
            DEF_ATTACHMENTDIR, m_sessiongrpname.c_str());

    return filename;
}

void SessionWindow::save()
{
    if (m_SessionName.get_text().length() > 5) {
        std::ofstream outfile(getTitleFilePath());
        outfile << m_SessionName.get_text() << std::endl;
        outfile.close();

        m_dockitem->m_titlename = m_SessionName.get_text();
    }

    size_t result;
    FILE* f;
    f = fopen(getFilePath().c_str(), "wb");
    if (!f) {
        g_critical("SessionWindow::save: can't create file.");
        return;
    }

    gchar* iconBuffer;
    gsize buffer_size;
    struct sessionGrpData st;
    int i;
    int maxrows = m_ListBox.get_children().size();
    for (i = 0; i < maxrows; i++) {
        ListRow* row = (ListRow*) m_ListBox.get_row_at_index(i);
        Glib::RefPtr<Gdk::Pixbuf> appIcon = row->get_pixbuf();
        try {
            appIcon->save_to_buffer(iconBuffer, buffer_size);
        } catch (Gdk::PixbufError pe) {
            g_critical("SessionWindow::save: Gdk::PixbufError\n");
        } catch (Glib::FileError fe) {
            g_critical("SessionWindow::save: Glib::FileError\n");
        }

        if (buffer_size > DEF_PIXBUFFER_MAX) {
            g_critical("SessionWindow::save: pixbuf  size out of range.");
            continue;
        }

        std::string appname = row->get_appname();
        std::string parameters = row->get_parameters();
        std::string titlename = row->get_titlename();

        memcpy(st.pixbuff, iconBuffer, buffer_size);
        strcpy(st.appname, appname.c_str());
        strcpy(st.parameters, parameters.c_str());
        strcpy(st.titlename, titlename.c_str());

        size_t result = fwrite(&st, sizeof (st), 1, f);
        if (result == 0)
            g_critical("SessionWindow::save:: Error writing file> fwrite\n");

        delete [] (gchar*) iconBuffer;

    }

    fclose(f);

    Gtk::MessageDialog dialog(*this, _("\n\nSession group successfully saved!\n\n"), false, Gtk::MESSAGE_INFO);
    dialog.run();
    close();

    this->close();
}

SessionWindow::type_signal_getactive SessionWindow::signal_getactive()
{
    return m_signal_getactive;
}

void SessionWindow::on_signal_getactive(WnckWindow* window)
{
    if (m_deleteSet)
        return;

    if (!WNCK_IS_WINDOW(window))
        return;


    std::string appname = Launcher::getAppNameByWindow(window);
    if (appname.empty()) {
        m_EntryAppName.set_text(_("Click here to get the Application name!"));
        return;
    }

    m_EntryAppName.set_text(appname);
    m_window = window;

}

void SessionWindow::on_active_window_changed_callback(WnckScreen *screen,
        WnckWindow *previously_active_window, gpointer user_data)
{
    if (m_deleteSet)
        return;

    m_signal_getactive.emit(previously_active_window);
}
