/* 
 * File:   IconWindow.h
 * Author: yoo
 *
 * Created on March 23, 2019, 1:27 PM
 */

#ifndef ICONWINDOW_H
#define	ICONWINDOW_H

#include <gtkmm.h> 
#include "Configuration.h"

class IconWindow : public Gtk::Window {
public:
    IconWindow();
    virtual ~IconWindow();
    void setImage(const Glib::RefPtr<Gdk::Pixbuf> image);
private:
    Glib::RefPtr<Gdk::Pixbuf> m_image = NULLPB;
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

};

#endif	/* ICONWINDOW_H */

