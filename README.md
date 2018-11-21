# Docklight
Docklight is a lightweight Dock launcher for common Applications.

Docklight” is a simple but effective dock launcher for GNOME desktops e.g (gnome, cinnamon, mate etc.).
It supports a Preview from all open Applications.

The “Docklight” preview allows more control over the opened windows. You can navigate to a specific window much easier.
Select the window or close the desired application the real time preview make that very easy.
“Docklight” is written in C++ with “GTK 3” and “libwnck”. “Docklight” is open source width a GNU General Public License.


![docklight_sample](https://cloud.githubusercontent.com/assets/9448387/19643317/e3d066ea-9a13-11e6-8971-96cf38752c97.jpg)


Docklight has been designed for users who have to handle multiple applications. The desktop may have several windows open or hidden behind other windows. This is where docklight can help you handle multiple windows easily.


 Licensing:

  This program is released under the terms of the GNU General Public
  License (GNU GPL).

  You can find a copy of the license in the file LICENSE.
  
 Compiling:
  
  In order to build this program you need a working GNOME environment
  with the development tools installed properly.
  The following is the detailed list of libraries you need:
  
	- gtkmm-3.0
	- libwnck-3.0	
	- libx11-dev
	- Autotols
   
	./autogen.sh
	./configure --prefix=/usr/lib/docklight
	make
	sudo make install


 Install from a debian package:
 
 you can install “Docklight” direct from the debian Package/docklight_1.0-*_amd64.deb located in this   repository.
 
 Credits:

  Docklight is based of the inspiration from other great docks. It is simple but is still in   development. 
  It will  bring more refinements and  new features to make Docklight more comfortable and powerful.
  Many thanks to the all developers and to all the people who contribute to linux.

  




