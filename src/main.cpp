//  Copyright (c) 2018-2024 Juan R. González
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
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// clang-format off
#include <gtkmm/application.h>
#include <config.h>
#include <gdk/gdkx.h>
#include <glibmm/i18n.h>
#include <gtkmm/main.h>
#include <libintl.h>
#include <cstdlib>

#include "appwindow.h"
// clang-format on

using namespace docklight;

int main(int argc, char *argv[])
{
    // The XInitThreads function initializes Xlib support for concurren
    // threads. This function must be the first Xlib function a multi-threaded
    // program calls, and it must complete before any other Xlib call is made.
    // This function returns a nonzero status if initialization was successful;
    // otherwise, it returns zero. On systems that do not support threads, this
    // function always returns zero.
    // https://www.x.org/releases/X11R7.5/doc/man/man3/XInitThreads.3.html
    XInitThreads();

    // The initialization code:
    // Set up the user current locale.
    setlocale(LC_ALL, "");
    g_print("Initialize gettext\n");
    char *domain = bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
    g_print(("bindtextdomain: %s %s %s\n"), domain, GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);

    // bind_text domain_codeset - set encoding of message translations
    char *btdcodeset = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    g_print("bind_textdomain_codeset: %s\n", btdcodeset);

    // The  text domain  function  sets or retrieves the current  message
    // domain.
    char *txtdomain = textdomain(GETTEXT_PACKAGE);
    g_print("textdomain: %s\n", txtdomain);

    g_print("create application\n");
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(
        argc, argv, "org.gtkmm.AppWindow.base",
        Gio::APPLICATION_HANDLES_COMMAND_LINE | Gio::APPLICATION_NON_UNIQUE);

    g_print("create AppWindow \n");
    AppWindow win;
    int result = win.init(app);

    if (result != 0) {
        g_error("AppWindow init error.\n");
        exit(result);
    }

    // Shows the window and returns when it is closed.
    g_print("app running...\n\n");
    result = app->run(win);
    g_print("Terminate with code %d \n", result);

    return result;
}

