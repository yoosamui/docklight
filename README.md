
# Docklight
 Manage access to applications.

![image](https://github.com/user-attachments/assets/7994983c-8d4f-4ace-91e8-20a88a236521)


## Installation
Docklight for Debian/Ubuntu Linux platform (64-bit)

For non Debian/Ubuntu Systems change the package manager in the install_dependecies.sh script.
clone this repository if you want to get the **stable** version:

```bash
$ git clone https://github.com/yoosamui/docklight
```
This will create the directory docklight.

clone this repository if you want to get the **current** version:

```bash
$ git clone --branch current https://github.com/yoosamui/docklight
```
This will create the directory docklight.


cd into it:

```bash
$ cd docklight
```
Eexecute the following bash scripts:

```bash
$ sudo ./install_dependencies.sh
$ sudo ./install_docklight.sh
```
docklight is now installed on your system and will start after next login.
Start it manually with:

```bash
$ docklight5
or
with parameters like so:
$ docklight5 '-l left '
```

You can start docklight with the following parameters:

![image](https://github.com/user-attachments/assets/9555d475-219b-43fc-9f16-237003d7f509)
Remember that, parameters replace the config file values.

## Uninstallation

```bash
$ cd docklight
$ sudo make uninstall
```
If you want to remove the configuration files, do the following bash statement:
```bash
$ rm -rfd /home/$USER/.config/docklight
```
## Compile the source code

```bash
$ cd docklight
$ ./autogen.sh
$ ./configure.sh
$ make

The final executable is located at:
$ cd <YOUR_DOCKLIGHT_PATH>/src/docklight

```

## Configure
Change the icon or preview size values or event create new themes.


The configurations directory is here: **/home/$USER/.config/docklight**

The configuration Filename is: **docklight5.config**


## Mouse keys

Left mouse click                : minimize/unminimize group
.
Left mouse press and hold       : activate drag & drop.

Right mouse click               : activate context menu.

Right mouse click longer press  : activate context image preview.

Mouse wheel                     : scroll through the window group list.

## Current state
beta version: 5.1.64


