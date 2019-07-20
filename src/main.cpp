
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

    g_print("Initialize gettext\n");
    char *domain = bindtextdomain(GETTEXT_PACKAGE, PROGRAMNAME_LOCALEDIR);
    g_print("bindtextdomain: %s %s %s\n", domain, GETTEXT_PACKAGE,
            PROGRAMNAME_LOCALEDIR);

    // bind_text domain_codeset - set encoding of message trans‐lations
    char *btdcodeset = bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    g_print("bind_textdomain_codeset: %s\n", btdcodeset);

    // The  text domain  function  sets or retrieves the current  message
    // domain.
    char *txtdomain = textdomain(GETTEXT_PACKAGE);
    g_print("textdomain: %s\n", txtdomain);

    g_print("create application\n");
    Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(
        argc, argv, "org.gtkmm.Appwindow",
        Gio::APPLICATION_HANDLES_COMMAND_LINE | Gio::APPLICATION_NON_UNIQUE);

    g_print("create window and init signals. evaluate parameters.\n");
    AppWindow win;
    int result = win.init(app);
    if (result != 0) {
        g_error("AppWindow init error.\n");
        exit(result);
    }

    // Shows the window and returns when it is closed.
    g_print("app running\n");
    result = app->run(win);

    g_print("Terminate with code %d \n", result);

    return result;
}
