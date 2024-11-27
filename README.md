# Docklight-5
Linux Dock with Preview.
Helps to access and manage application windows.

![image](https://github.com/user-attachments/assets/ca3dc03c-05c5-430d-b143-e553aeadeebe)


## Installation
clone this repository to get the stable version:

```bash
git clone [(https://github.com/yoosamui/docklight)]
```
This will create the diretory docklight.

clone this repository to get the current version:

```bash
git clone --branch current [(https://github.com/yoosamui/docklight)]
```
This will create the directory docklight.


cd into it:

```bash
cd docklight
```

Now execute the following bash scripts:

```bash
sudo ./install_dependencies.sh
sudo ./install_docklight.sh
```
docklight is now installed on your system.
Start it with:

```bash
docklight5
```
You can start docklight with the following parameters:

![image](https://github.com/user-attachments/assets/9555d475-219b-43fc-9f16-237003d7f509)


You should start docklight after your linux login.

Create the folowing desktop file:

```bash
#!/usr/bin/env xdg-open
[Desktop Entry]
Type=Application
Exec=/bin/bash /usr/bin/docklight5
X-GNOME-Autostart-enabled=true
NoDisplay=false
Hidden=false
Name[en_US]=docklight-5
Comment[en_US]=Start Applications, access and manage them easily.
X-GNOME-Autostart-Delay=1
```

named it "docklight5.desktop" and save it at
```bash
/home/$USER/.config/autostart/docklight5.desktop
```

## Uninstallation

```bash
cd docklight
sudo make uninstall
```
If you want to remove the configuration files, do the following bash statement:
```bash
rm -rfd /home/$USER/.config/docklight
```
## Compile the source code

```bash
cd docklight
./autogen.sh
./configure.sh
make

The final execuable is located at:
cd docklight
/src/docklight

```

## Current state

in development...



