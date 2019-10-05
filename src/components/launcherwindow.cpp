#include "launcherwindow.h"
#include <glibmm/i18n.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include "components/config.h"
#include "components/dockitem.h"
#include "utils/position.h"
#include "utils/system.h"

DL_NS_BEGIN

#define DEF_DATADIRNAME "data/"
#define DEF_ICONNAME "data/images/docklight.logo.png"
#define DEF_LAUCHERTEMPLATE "docklight.template.desktop"
#define DEF_LAUCHERTEMPLATEPATH "data/docklight.template.desktop"
#define DEF_LAUCHERSCRIPT "docklight.launcher.sh"
#define DEF_LAUCHERSCRIPTPATH "data/docklight.launcher.sh"

#define DEF_DIALOG_WIDTH 440
#define DEF_DIALOG_HEIGHT 200

launcher_buttons_box::launcher_buttons_box(bool horizontal, const Glib::ustring& title,
                                           gint spacing, Gtk::ButtonBoxStyle layout)
    : Gtk::Frame(title),
      m_button_createLauncher(_("Create Launcher")),
      m_button_testLauncher(_("Test Launcher")),
      m_button_cancel(_("Cancel"))
{
    Gtk::ButtonBox* bbox = nullptr;

    if (horizontal)
        bbox = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_HORIZONTAL));
    else
        bbox = Gtk::manage(new Gtk::ButtonBox(Gtk::ORIENTATION_VERTICAL));

    bbox->set_border_width(5);

    add(*bbox);

    bbox->set_layout(layout);
    bbox->set_spacing(spacing);

    bbox->add(m_button_createLauncher);
    bbox->add(m_button_testLauncher);
    bbox->add(m_button_cancel);
}

launcher_window::launcher_window()
    : Gtk::Window(Gtk::WindowType::WINDOW_TOPLEVEL),
      m_vbox(Gtk::ORIENTATION_VERTICAL),
      m_vpaned(Gtk::ORIENTATION_VERTICAL),
      m_vbox_center(Gtk::ORIENTATION_VERTICAL),
      m_button_box(Gtk::ORIENTATION_VERTICAL),
      m_label_file(_("Command: ")),
      m_button_file(_("Choose command")),
      m_button_create(_("Create launcher")),
      m_button_test("_(Test launcher"),
      m_button_categories_link(_("Available")),
      m_label_info("_(Create launcher."),
      m_label_categories(_("Categories: "))
{
    set_title(_("Docklight desktop file launcher"));
    set_size_request(DEF_DIALOG_WIDTH, DEF_DIALOG_HEIGHT);
    this->set_resizable(false);
    this->set_keep_above(true);
    this->set_type_hint(Gdk::WindowTypeHint::WINDOW_TYPE_HINT_DIALOG);

    set_border_width(12);
    m_label_file.set_margin_left(4);
    m_label_categories.set_margin_left(4);

    add(m_vbox);

    m_vbox.add(m_frame_top);
    m_frame_top.add(m_label_info);
    m_frame_top.set_size_request(-1, 50);
    m_frame_top.set_margin_bottom(10);
    m_vbox_center.set_margin_top(4);
    m_vbox_center.set_margin_bottom(4);
    m_vbox_center.set_margin_left(4);
    m_vbox_center.set_margin_right(4);
    m_vbox.add(m_frame);
    m_frame.add(m_vbox_center);
    m_vbox_center.add(m_grid);
    m_vbox.add(m_hbox);

    launcher_buttons_box* lb = new launcher_buttons_box(true, "", 5, Gtk::BUTTONBOX_SPREAD);

    m_hbox.pack_start(*Gtk::manage(lb), Gtk::PACK_EXPAND_WIDGET);
    m_entry_file.set_max_length(MAX_INPUT);
    m_entry_categories.set_max_length(MAX_INPUT);

    std::string docklightIcon = system_util::get_current_path(DEF_ICONNAME);
    this->set_icon_from_file(docklightIcon.c_str());

    m_grid.attach(m_label_file, 0, 3, 1, 1);
    m_grid.attach(m_entry_file, 1, 3, 1, 1);
    m_grid.attach(m_button_file, 2, 3, 1, 1);
    m_grid.attach(m_label_categories, 0, 8, 1, 1);
    m_grid.attach(m_entry_categories, 1, 8, 1, 1);
    m_grid.attach(m_button_categories_link, 2, 8, 1, 1);

    m_grid.set_row_spacing(4);
    m_grid.set_column_spacing(4);

    lb->m_button_cancel.signal_pressed().connect(
        sigc::mem_fun(*this, &launcher_window::on_button_close_clicked));

    lb->m_button_createLauncher.signal_pressed().connect(
        sigc::mem_fun(*this, &launcher_window::on_button_createLauncher_clicked));

    lb->m_button_testLauncher.signal_pressed().connect(
        sigc::mem_fun(*this, &launcher_window::on_button_testLauncher_clicked));

    m_button_file.signal_clicked().connect(
        sigc::mem_fun(*this, &launcher_window::on_button_file_clicked));

    m_button_categories_link.signal_clicked().connect(
        sigc::mem_fun(*this, &launcher_window::on_button_categories_link_clicked));

    int x, y;
    position_util::get_center_screen_position(DEF_DIALOG_WIDTH, DEF_DIALOG_HEIGHT, x, y);
    move(x, y);
    m_grid.show_all();
}

void launcher_window::on_button_close_clicked()
{
    this->close();
}

bool launcher_window::on_delete_event(GdkEventAny* event)
{
    return false;
}

launcher_window::~launcher_window()
{
    g_print("launcher_window destroy!\n");
}

void launcher_window::init(const shared_ptr<DockItem> dockitem)
{
    m_dockitem = dockitem;

    char message[NAME_MAX];
    sprintf(message,
            _("Launcher for %s could not be found.\nYou need to create a "
              "desktop file for this Application."),
            dockitem->get_name().c_str());

    m_label_info.set_text(message);
    // m_entry_name.set_text(m_dockitem->get_title().c_str());
    //
    string defaultcomment(m_dockitem->get_title() + _(" Application"));
    //    m_entry_comment.set_text(defaultcomment.c_str());
    m_entry_file.set_text("");
    //  m_entry_icon.set_text("");
    m_entry_categories.set_text("Other");
}

void launcher_window::on_button_testLauncher_clicked()
{
    if (!is_valid_command()) {
        invalid_entry_message();
        return;
    }

    if (system(m_entry_file.get_text().c_str()) == 0) {
        m_test_launch = true;
        return;
    }

    m_test_launch = false;
    Gtk::MessageDialog dialog(*this, _("Launcher test failed!"), false, Gtk::MESSAGE_INFO);

    // dialog.set_icon_name()
    dialog.set_secondary_text(
        _("The command you enter is invalid.\n\n"
          "Some Applications are scripts files and others requires Administrator "
          "privileges:\n"
          "for scrips files you need to add the bash on the beginning:\n\n"
          "/bin/sh \"/usr/local/scriptfile.sh\"\n"
          "/usr/bin/bash \"/home/USER/Develop/scriptfile\"\n\n"
          "for Applications that requires Administrator privileges:\n"
          "pkexec Application\n\n"
          "Please make sure that the Application starts from the command line.\n"
          "Open a terminal and navigate to the directory where the application "
          "is locate\n"
          "and trying the command from there..."));

    dialog.run();
}

void launcher_window::on_button_createLauncher_clicked()
{
    if (!is_valid_command()) {
        invalid_entry_message();
        return;
    }

    if (!m_test_launch) {
        Gtk::MessageDialog dialog(
            *this,
            _("\n\nYou need to make a test launch first.\nAfter the Application "
              "starts, close it and click create launcher.\n\n"),
            false, Gtk::MESSAGE_INFO);
        dialog.run();
        return;
    }

    std::string desktopfile = m_dockitem->get_name() + ".desktop";
    std::string templatefileLocal(system_util::get_current_path(DEF_LAUCHERTEMPLATEPATH));

    std::string desktopfileLocal(system_util::get_current_path(DEF_DATADIRNAME + desktopfile));

    // create a copy from template
    char command[NAME_MAX];
    sprintf(command, "cp %s %s ", templatefileLocal.c_str(), desktopfileLocal.c_str());

    if (system(command) != 0) {
        Gtk::MessageDialog dialog(*this, _("\n\nCopy template Error!\n\n"), false,
                                  Gtk::MESSAGE_INFO);
        dialog.run();
        return;
    }

    sprintf(command, "%s/%s.png", system_util::get_current_path(DEF_DATADIRNAME).c_str(),
            m_dockitem->get_name().c_str());

    string iconlocal(command);
    m_dockitem->get_image()->save(command, "png");

    GError* error = NULL;
    GKeyFile* key_file = g_key_file_new();
    if (FALSE == g_key_file_load_from_file(key_file, desktopfileLocal.c_str(),
                                           GKeyFileFlags::G_KEY_FILE_NONE, &error)) {
        if (error) {
            g_error_free(error);
            error = NULL;
        }
        g_key_file_free(key_file);
        return;
    }

    // Name
    g_key_file_set_string(key_file, "Desktop Entry", "Name", m_dockitem->get_name().c_str());

    // icon
    sprintf(command, "/usr/share/icons/hicolor/48x48/apps/%s.png", m_dockitem->get_name().c_str());

    g_key_file_set_string(key_file, "Desktop Entry", "Icon", command);

    // Command Executable
    std::string executable(m_entry_file.get_text());
    if (executable.empty()) {
        g_key_file_free(key_file);
        return;
    }
    g_key_file_set_string(key_file, "Desktop Entry", "Exec", executable.c_str());

    // Categories
    std::string categories(m_entry_categories.get_text());
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

    // https://www.freedesktop.org/software/polkit/docs/0.105/pkexec.1.html
    sprintf(command, "pkexec %s %s %s",
            system_util::get_current_path(DEF_LAUCHERSCRIPTPATH).c_str(), desktopfileLocal.c_str(),
            iconlocal.c_str());

    if (system(command) != 0) {
        Gtk::MessageDialog dialog(*this, _("\n\nPermissions required. Please try again!\n\n"),
                                  false, Gtk::MESSAGE_INFO);
        dialog.run();
        return;
    }

    string desktop_filename = "/usr/share/applications/" + m_dockitem->get_name() + ".desktop";
    m_dockitem->set_desktop_filename(desktop_filename);
    AppUpdater::save();

    // Gtk::MessageDialog dialog(*this, _("\n\nLauncher created successfully!\n\n"), false,
    // Gtk::MessageDialog dialog(*this, desktop_filename.c_str(), false, Gtk::MESSAGE_INFO);
    // dialog.run();
    close();
}

void launcher_window::on_button_file_clicked()
{
    m_test_launch = false;
    Gtk::FileChooserDialog dialog(_("Please choose a file"), Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);

    // Add response buttons the the dialog:
    dialog.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
    dialog.add_button(_("_Open"), Gtk::RESPONSE_OK);

    // Show the dialog and wait for a user response:
    int result = dialog.run();

    if (result != Gtk::RESPONSE_OK) {
        dialog.close();
        return;
    }

    std::string filename = dialog.get_filename();

    m_entry_file.set_text(filename.c_str());
}

void launcher_window::on_button_categories_link_clicked()
{
    // system("sensible-browser
    // https://specifications.freedesktop.org/menu-spec/menu-spec-1.0.html#category-registry");
    if (system("xdg-open "
               "https://specifications.freedesktop.org/menu-spec/"
               "menu-spec-1.0.html#category-registry") != 0) {
        Gtk::MessageDialog dialog(*this, _("Browser could not be found!\n"), false,
                                  Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
}

bool launcher_window::is_valid_command()
{
    if (m_entry_file.get_text().empty()) {
        return false;
    }
    return true;
}

void launcher_window::invalid_entry_message()
{
    Gtk::MessageDialog dialog(*this, _("The field command can't be empty!\n"), false,
                              Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
    dialog.run();
}

DL_NS_END
