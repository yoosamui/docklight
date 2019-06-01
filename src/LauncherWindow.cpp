//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on January  2017
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

#include "LauncherWindow.h"
#include "Utilities.h"
#include "Configuration.h"
#include "DockItemPositions.h"
#include "DockItem.h"
#include "DockPanel.h"

#include  <glibmm/i18n.h>

LaucherButtonBox::LaucherButtonBox(bool horizontal,
        const Glib::ustring& title,
        gint spacing,
        Gtk::ButtonBoxStyle layout)
    : Gtk::Frame(title),
    m_Button_createLauncher(_("Create Launcher")),
    m_Button_testLauncher(_("Test Launcher")),
    m_Button_Cancel(_("Cancel"))
{
    Gtk::ButtonBox* bbox = nullptr;

    if (horizontal)
        bbox = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL));
    else
        bbox = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_VERTICAL));

    bbox->set_border_width(5);

    add(*bbox);

    /* Set the appearance of the Button Box */
    bbox->set_layout(layout);
    bbox->set_spacing(spacing);

    bbox->add(m_Button_createLauncher);
    bbox->add(m_Button_testLauncher);
    bbox->add(m_Button_Cancel);



}

LauncherWindow::LauncherWindow()
    : //Gtk::Window(Gtk::WindowType::WINDOW_POPUP),
        Gtk::Window(Gtk::WindowType::WINDOW_TOPLEVEL),
        m_VBox(Gtk::ORIENTATION_VERTICAL),
        m_VPaned(Gtk::ORIENTATION_VERTICAL),
        m_VBoxCenter(Gtk::ORIENTATION_VERTICAL),
        m_ButtonBox(Gtk::ORIENTATION_VERTICAL),
        m_labelFile(_("Command: ")),
        m_Button_File(_("Choose command")),
        m_Button_Icon(_("Choose Icon")),
        m_Button_create(_("Create Launcher")),
        m_Button_test("_(Test Launcher"),
        m_Button_CategoriesLink(_("Available")),
        m_labeInfo("_(Create Launcher."),
        m_labelName(_("Name: ")),
        m_labelComment(_("Comment: ")),
        m_labelIcon(_("Icon: ")),
        m_labelCategories(_("Categories: ")),
        m_Button_close(_("Close"))
{

    set_title(_("Docklight Launcher"));
    set_size_request(440, 300);
    this->set_resizable(false);
    this->set_keep_above(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);



    //this->set_decorated(false);
    /* Sets the border width of the window. */
    set_border_width(12);

    m_labelComment.set_margin_left(4);
    m_labelFile.set_margin_left(4);
    m_labelCategories.set_margin_left(4);

    add(m_VBox);

    m_VBox.add(m_FrameTop);
    m_FrameTop.add(m_labeInfo);
    m_FrameTop.set_size_request(-1, 50);
    m_FrameTop.set_margin_bottom(10);

    // m_Frame.set_size_request(-1, 50);
    // m_Frame.set_margin_top(10);

    m_VBoxCenter.set_margin_top(4);
    m_VBoxCenter.set_margin_bottom(4);
    m_VBoxCenter.set_margin_left(4);
    m_VBoxCenter.set_margin_right(4);

    m_VBox.add(m_Frame);
    m_Frame.add(m_VBoxCenter);
    m_VBoxCenter.add(m_grid);
    m_VBox.add(m_HBox);


    LaucherButtonBox* lb = new LaucherButtonBox(true, "", 5,
            Gtk::BUTTONBOX_SPREAD);

    m_HBox.pack_start(*Gtk::manage(
                //new LaucherButtonBox(true, "", 5,
                //Gtk::BUTTONBOX_SPREAD)
                lb
                ),
            Gtk::PACK_EXPAND_WIDGET);


    m_EntryName.set_max_length(60);
    m_EntryComment.set_max_length(150);
    m_EntryIcon.set_max_length(150);
    m_EntryFile.set_max_length(MAX_INPUT);
    m_EntryCategories.set_max_length(MAX_INPUT);



    std::string docklightIcon = Utilities::getExecPath(DEF_ICONNAME);
    this->set_icon_from_file(docklightIcon.c_str());

    m_grid.attach(m_labelName, 0, 0, 1, 1);
    m_grid.attach(m_EntryName, 1, 0, 1, 1);


    m_grid.attach(m_labelComment, 0, 1, 1, 1);
    m_grid.attach(m_EntryComment, 1, 1, 2, 1);

    m_grid.attach(m_labelFile, 0, 3, 1, 1);
    m_grid.attach(m_EntryFile, 1, 3, 1, 1);
    m_grid.attach(m_Button_File, 2, 3, 1, 1);


    m_grid.attach(m_labelIcon, 0, 6, 1, 1);
    m_grid.attach(m_EntryIcon, 1, 6, 1, 1);
    m_grid.attach(m_Button_Icon, 2, 6, 1, 1);

    m_grid.attach(m_labelCategories, 0, 8, 1, 1);
    m_grid.attach(m_EntryCategories, 1, 8, 1, 1);
    m_grid.attach(m_Button_CategoriesLink, 2, 8, 1, 1);



    m_grid.set_row_spacing(4);
    m_grid.set_column_spacing(4);


    // lb->m_Button_createLauncher.set_sensitive(false);

    lb->m_Button_Cancel.signal_pressed().connect(sigc::mem_fun(*this, &LauncherWindow::close));
    lb->m_Button_createLauncher.signal_pressed().connect(sigc::mem_fun(*this, &LauncherWindow::on_button_createLauncher_clicked));
    lb->m_Button_testLauncher.signal_pressed().connect(sigc::mem_fun(*this, &LauncherWindow::on_button_testLauncher_clicked));


    m_Button_File.signal_clicked().connect(sigc::mem_fun(*this,
                &LauncherWindow::on_button_file_clicked));

    m_Button_Icon.signal_clicked().connect(sigc::mem_fun(*this,
                &LauncherWindow::on_button_icon_clicked));

    m_Button_CategoriesLink.signal_clicked().connect(sigc::mem_fun(*this,
                &LauncherWindow::on_button_CategoriesLink_clicked));


    this->signal_delete_event().
        connect(sigc::mem_fun(*this, &LauncherWindow::on_delete_event));

    int x, y;
    DockItemPositions::get_CenterScreenPos(440, 300, x, y);
    move(x, y);
    m_grid.show_all();

    //std::string iconFile = Utilities::getExecPath("gnome-panel-launcher.png");
    //this->set_icon_from_file(iconFile);

}

bool LauncherWindow::on_delete_event(GdkEventAny* event)
{
    m_dockpanel->m_launcherWindow = nullptr;
    delete(this);
    return false;
}

LauncherWindow::~LauncherWindow()
{
    g_print("LauncherWindow destroy!\n");
}

void LauncherWindow::init(DockPanel& dockpanel, DockItem* dockitem)
{
    m_dockitem = dockitem;
    m_dockpanel = &dockpanel;

    char message[NAME_MAX];
    sprintf(message, _("Launcher for %s could not be found.\nYou need to create a Launcher for this Application."),
            dockitem->get_title().c_str());

    m_labeInfo.set_text(message);
    m_EntryName.set_text(m_dockitem->get_title().c_str());
    std::string defaultcomment(m_dockitem->get_title() + _(" Application"));
    m_EntryComment.set_text(defaultcomment.c_str());
    m_EntryFile.set_text("");
    m_EntryIcon.set_text("");
    m_EntryCategories.set_text("Other");


}

void LauncherWindow::on_button_testLauncher_clicked()
{
    if (!isFormFieldsValid()) {
        InvalidFormFieldsMessage();
        return;
    }


    if (system(m_EntryFile.get_text().c_str()) == 0)
        return;

    Gtk::MessageDialog dialog(*this, _("Launcher test Failed!"), false, Gtk::MESSAGE_INFO);
    //dialog.set_icon_name()
    dialog.set_secondary_text(_("The command you enter is invalid.\n\n"
                "Some Applications are scripts files and others requires Administrator privileges:\n"
                "for scrips files you need to add the bash on the beginning:\n\n"
                "/bin/sh \"/usr/local/scriptfile.sh\"\n"
                "/usr/bin/bash \"/home/yoo/Develop/scriptfile\"\n\n"
                "for Applications that requires Administrator privileges:\n"
                "pkexec Application\n\n"
                "Please make sure that the Application starts from the command line.\n"
                "Open a terminal and navigate to the directory where the application is locate\n"
                "and trying the command from there..."));

    dialog.run();

}

void LauncherWindow::on_button_createLauncher_clicked()
{

    if (!isFormFieldsValid()) {
        InvalidFormFieldsMessage();
        return;
    }


    std::string desktopfile = m_dockitem->get_desktopFileName();
    std::string templatefileLocal(Utilities::getExecPath(DEF_LAUCHERTEMPLATEPATH));
    std::string desktopfileLocal(Utilities::getExecPath(DEF_DATADIRNAME + desktopfile));


    // create a copy from template
    char command[NAME_MAX];
    sprintf(command, "cp %s %s ", templatefileLocal.c_str(), desktopfileLocal.c_str());
    if (system(command) != 0) {
        Gtk::MessageDialog dialog(*this, _("\n\nCopy template Error!\n\n"), false, Gtk::MESSAGE_INFO);
        dialog.run();
        return;
    }


    GError *error = NULL;
    GKeyFile *key_file = g_key_file_new();
    if (FALSE == g_key_file_load_from_file(key_file,
                desktopfileLocal.c_str(),
                GKeyFileFlags::G_KEY_FILE_NONE, &error)) {

        if (error) {
            g_error_free(error);
            error = NULL;
        }
        g_key_file_free(key_file);
        return;
    }


    // Name
    std::string name(m_EntryName.get_text());
    if (name.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Name", name.c_str());


    // Comment
    std::string comment(m_EntryComment.get_text());
    if (!comment.empty()) {
        g_key_file_set_string(key_file, "Desktop Entry", "Comment", comment.c_str());
    }

    // Command Executable
    std::string executable(m_EntryFile.get_text());
    if (executable.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Exec", executable.c_str());


    // Icon
    std::string iconfile(m_EntryIcon.get_text());
    if (!iconfile.empty()) {
        g_key_file_set_string(key_file, "Desktop Entry", "Icon", iconfile.c_str());
    } else {

        //g_key_file_set_string(key_file, "Desktop Entry", "Icon", "applications-system");
        g_key_file_remove_key(key_file, "Desktop Entry", "Icon", &error);

    }

    // Categories
    std::string categories(m_EntryCategories.get_text());
    if (categories.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Categories", categories.c_str());



    if (FALSE == g_key_file_save_to_file(key_file, desktopfileLocal.c_str(), &error)) {
        if (error) {
            g_error_free(error);
            error = NULL;
        }
        g_key_file_free(key_file);
        return;
    }


    g_key_file_free(key_file);


    //https://www.freedesktop.org/software/polkit/docs/0.105/pkexec.1.html
    sprintf(command, "pkexec %s %s",
            Utilities::getExecPath(DEF_LAUCHERSCRIPTPATH).c_str(),
            desktopfileLocal.c_str());


    if (system(command) != 0) {
        Gtk::MessageDialog dialog(*this, _("\n\nPermissions required. Please try again!\n\n"), false, Gtk::MESSAGE_INFO);
        dialog.run();
        return;
    }


    Gtk::MessageDialog dialog(*this, _("\n\nLauncher created successfully!\n\n"), false, Gtk::MESSAGE_INFO);
    dialog.run();
    close();
}

void LauncherWindow::on_button_file_clicked()
{

    Gtk::FileChooserDialog dialog(_("Please choose a file"),
            Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);


    //Add response buttons the the dialog:
    dialog.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
    dialog.add_button(_("_Open"), Gtk::RESPONSE_OK);

    //Show the dialog and wait for a user response:
    int result = dialog.run();

    if (result != Gtk::RESPONSE_OK) {
        dialog.close();
        return;
    }

    std::string filename = dialog.get_filename();

    m_EntryFile.set_text(filename.c_str());

}

void LauncherWindow::on_button_icon_clicked()
{

    Gtk::FileChooserDialog dialog(_("Please choose a file"),
            Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);


    //Add response buttons the the dialog:
    dialog.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
    dialog.add_button(_("_Open"), Gtk::RESPONSE_OK);

    auto filter_text = Gtk::FileFilter::create();
    filter_text->set_name(_("Image files"));
    filter_text->add_mime_type("image/png");
    filter_text->add_mime_type("image/ico");
    filter_text->add_mime_type("image/svg");
    filter_text->add_mime_type("image/jpg");
    dialog.add_filter(filter_text);

    //Show the dialog and wait for a user response:
    int result = dialog.run();

    if (result != Gtk::RESPONSE_OK) {
        dialog.close();
        return;
    }

    std::string filename = dialog.get_filename();

    m_EntryIcon.set_text(filename.c_str());

}

void LauncherWindow::on_button_CategoriesLink_clicked()
{
    //system("sensible-browser https://specifications.freedesktop.org/menu-spec/menu-spec-1.0.html#category-registry");
    if (system("xdg-open https://specifications.freedesktop.org/menu-spec/menu-spec-1.0.html#category-registry") != 0) {

        Gtk::MessageDialog dialog(*this, _("Browser could not be found!\n"), false,
                Gtk::MESSAGE_INFO,
                Gtk::BUTTONS_OK,
                true
                );
        dialog.run();
    }

}

bool LauncherWindow::isFormFieldsValid()
{
    if (m_EntryName.get_text().empty())
        return false;

    if (m_EntryFile.get_text().empty())
        return false;

    return true;

}

void LauncherWindow::InvalidFormFieldsMessage()
{
    Gtk::MessageDialog dialog(*this, _("Fields Name and Command can't be empty!\n"), false,
            Gtk::MESSAGE_INFO,
            Gtk::BUTTONS_OK,
            true
            );
    dialog.run();

}
