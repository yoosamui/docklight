//*****************************************************************
//
//  Copyright (C) 2015 Juan R. Gonzalez
//  Created on November 20, 2015, 12:17 PM 
//  j-gonzalez@email.de
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



#include <gtkmm/application.h>
#include "AppWindow.h"
#include <sstream>
#include <glibmm/i18n.h>
#include <gtkmm/main.h>
#include <libintl.h>
#include <config.h>
#include <gtk-3.0/gtk/gtk.h>
#include "Utilities.h"
#include <iostream>
#include <cstdlib>
namespace
{

    int on_command_line(const Glib::RefPtr<Gio::ApplicationCommandLine>& command_line,
                        Glib::RefPtr<Gtk::Application>& app)
    {
        int argc = 0;
        char** argv = command_line->get_arguments(argc);

        for (int i = 0; i < argc; ++i) {

            if (strcmp(argv[i], "top") == 0) {
                Configuration::set_dockWindowLocation(panel_locationType::TOP);
                continue;
            }

            if (strcmp(argv[i], "botom") == 0) {
                Configuration::set_dockWindowLocation(panel_locationType::BOTTOM);
                continue;
            }

            if (strcmp(argv[i], "left") == 0) {
                Configuration::set_dockWindowLocation(panel_locationType::LEFT);
                continue;
            }
            if (strcmp(argv[i], "right") == 0) {
                Configuration::set_dockWindowLocation(panel_locationType::RIGHT);
                continue;
            }
        }

        app->activate(); // Without activate() the window won't be shown.
        return EXIT_SUCCESS;
    }
} // anonymous namespace

/**
 * The Entry Point.
 * @param argc
 * @param argv
 * @return exit code
 */
int main(int argc, char *argv[])
{
    g_print("INIT GETTEXT\n");
    char* domain = bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
    g_print("bindtextdomain: %s %s %s\n", domain, GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);

    // bind_text domain_codeset - set encoding of message trans‐lations
    char* btdcodeset = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    g_print("bind_textdomain_codeset: %s\n", btdcodeset);

    // The  text domain  function  sets or retrieves the current  message domain.
    char* txtdomain = textdomain(GETTEXT_PACKAGE);
    g_print("textdomain: %s\n\n", txtdomain);

    g_print("CREATE GTK APP\n");
    //  Glib::RefPtr<Gtk::Application> app =
    //  Gtk::Application::create(argc, argv, "org.gtkmm.docklight");

    //auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");
    auto app = Gtk::Application::create(argc, argv,
                                        "org.gtkmm.example", Gio::APPLICATION_HANDLES_COMMAND_LINE | Gio::APPLICATION_NON_UNIQUE);
    app->signal_command_line().connect(sigc::bind(sigc::ptr_fun(&on_command_line), app), false);

    g_print("CREATE AppWindow\n");
    AppWindow win;

    int r = win.init();
    if (r != 0) {
        g_print("Appwindow init error.\n");
        exit(r);
    }

    //Shows the window and returns when it is closed.
    int result = app->run(win);
    g_print("Terminate with code %d \n", result);
    return result;

}
