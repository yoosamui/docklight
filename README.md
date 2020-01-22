
Docklight
---------

lightweight Dockbar for launch common applications.
Expandable with plugins.

Support several linux Window Managers. e.g Mutter, Fluxbox, Openbox, xfce etc.
Compton is recomendet for Window Managers without Compositor Manager e.g for Fluxbox or Openbox.

Low CPU usage:

IDLE CPU 0.0%
ACTIVITY CPU 2.0 - 4.0%
IMAGE GROUP PREVIEW STATIC IMAGES ( Images without movement ) 0.0% after 1 second.

![pic_1](https://user-images.githubusercontent.com/9448387/69397493-03f5b800-0d19-11ea-8c38-57fa02a937af.png)


- Multiple Monitors and multiple instance configurations.
- Autohide, Intellihide and none (strut) mode.
- Postioning on top, right,  bottom  or left.


Examples:

Start docklight on the default location defined in the configuration.
Use the primary monitor.
Read the settings from the default configuration docklight.config located in:
 ~/.config/docklight folder

./docklight

Start docklight with specific icon size.
Use the primary monitor.
This overrides the configuration value.

./docklight -r 64


Start docklight on the left location
Use the monitor with the number 1.
Read the settings from the specific configuration config1.config located in :
~/.config/docklight folder.

./docklight -l left -m 1 -c config1.config



in development...
