#include <getopt.h>

#include "appwindow.h"
#include "common.h"
#include "components/command_line.h"

DL_NS_BEGIN

appwindow::appwindow()
{
    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }

    this->show_all();
}

appwindow::~appwindow()
{
    g_print("free appwindow class.\n");
}

int appwindow::init(Glib::RefPtr<Gtk::Application> &app)
{
    app->signal_command_line().connect(
        sigc::bind(sigc::ptr_fun(&appwindow::on_command_line), app), false);

    return 0;
}

int appwindow::on_command_line(
    const Glib::RefPtr<Gio::ApplicationCommandLine> &command_line,
    Glib::RefPtr<Gtk::Application> &app)
{
    int argc = 0;
    char **argv = command_line->get_arguments(argc);

    cli::arguments a(argc, argv);
    if (a.validate() == false) {
        return -1;
    }

    const GSList *list = a.get_results();

    if (!list) {
        g_print("No parameters provided.\n");
    }

    for (const GSList *l = list; l; l = l->next) {
        cli::result_t *data = static_cast<cli::result_t *>(l->data);
        g_print("arguments: --%c %d %s\n", data->arg, data->int_value,
                data->string_value);
    }

    // Load config

    // activate window
    app->activate();

    // clang-format on
    return argc;
}

DL_NS_END
