# Docklight

A lightweight Linux dock for managing application windows and launching applications.

> **⚠️ Active Development**
>
> Docklight is currently under active development. Features, APIs, configuration options, and documentation may change between releases.

![Docklight Screenshot](https://github.com/user-attachments/assets/7994983c-8d4f-4ace-91e8-20a88a236521)

---

# Installation

**Supported platform:** Debian/Ubuntu (64-bit)

> **Note**
>
> If you are using a non-Debian distribution, edit `install_dependencies.sh` and replace the package manager commands with those appropriate for your system.

## Stable release

Clone the stable branch:

```bash
git clone https://github.com/yoosamui/docklight
cd docklight
```

## Development version

Clone the latest development branch:

```bash
git clone --branch current https://github.com/yoosamui/docklight
cd docklight
```

## Install

Run the installation scripts:

```bash
sudo ./install_dependencies.sh
sudo ./install_docklight.sh
```

Docklight will start automatically after your next login.

To start it immediately:

```bash
docklight5
```

or launch it with command-line options:

```bash
docklight5 -l left
```

> **Note**
>
> Command-line parameters override the values defined in the configuration file.

### Available command-line options

![Command Line Options](https://github.com/user-attachments/assets/9555d475-219b-43fc-9f16-237003d7f509)

---

# Uninstallation

From the project directory:

```bash
cd docklight
sudo make uninstall
```

To also remove your personal configuration:

```bash
rm -rf ~/.config/docklight
```

---

# Building from Source

```bash
cd docklight
./autogen.sh
./configure.sh
make
```

The compiled executable is located at:

```text
src/docklight
```

---

# Configuration

Docklight stores its configuration in:

```text
~/.config/docklight
```

Configuration file:

```text
docklight5.config
```

You can customize:

* Icon size
* Window preview size
* Themes
* Appearance
* Behavior

---

# Mouse Controls

| Action             | Result                                         |
| ------------------ | ---------------------------------------------- |
| Left click         | Minimize or restore the application group      |
| Left click + hold  | Start drag & drop                              |
| Right click        | Open the context menu                          |
| Right click + hold | Show image preview                             |
| Mouse wheel        | Cycle through windows in the application group |

---

# Current Status

**Version:** `5.3.0` **Status:** RC

