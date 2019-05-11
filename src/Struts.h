//*****************************************************************
//
//  Copyright © 2018 Juan R. González
//  j-gonzalez@email.de
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//*****************************************************************


#ifndef STRUTS_H
#define	STRUTS_H

#include <gtkmm/window.h>
#include <gdkmm/screen.h>
#include <gdk/gdk.h>
#include "Configuration.h"




class RootScreen;

class Struts {
    public:
        Struts(Gtk::Window* window, RootScreen* screen);
        virtual ~Struts();
        void update();
        void remove();


    private:

        Gtk::Window* window = NULL;
        RootScreen* screen;

        void update(bool reset);

};

#endif	/* STRUTS_H */

