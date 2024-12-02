<<<<<<< HEAD
# Docklight-5
Linux Dock with Preview.
=======
# Docklight
>>>>>>> dev
Helps to access and manage application windows.


![image](https://github.com/user-attachments/assets/b64b9a1d-a1cf-4f10-9e1c-b9619f81688e)


## Installation
clone this repository if you want to get the **stable** version:

```bash
git clone https://github.com/yoosamui/docklight
```
This will create the directory docklight.

clone this repository if you want to get the **current** version:

```bash
git clone --branch current https://github.com/yoosamui/docklight
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
docklight is now installed on your system and will start after next login.
Start it manually with:

```bash
docklight5
or
with parameters like so:
docklight5 '-l left '
```

You can start docklight with the following parameters:

![image](https://github.com/user-attachments/assets/9555d475-219b-43fc-9f16-237003d7f509)
Remember that, parameters replace the config file values. 

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

## Configure
Change the icon or preview size values or event create new themes.


The configurations directory is here: **/home/$USER/.config/docklight**

The configuration Filename is: **docklight5.config**


## Current state

in development...



