/* 
 * File:   AppRunAnimation.cpp
 * Author: yoo
 * 
 * Created on March 21, 2019, 6:00 PM
 */

#include <gtk/gtk.h>

#include "AppRunAnimation.h"
#include "DockPanel.h"
#include "IconWindow.h"

AppRunAnimation::AppRunAnimation(){ }

AppRunAnimation::~AppRunAnimation(){ }

void AppRunAnimation::start(DockPanel& panel, Glib::RefPtr<Gdk::Pixbuf> image)
{
    if (image->get_colorspace() != Gdk::COLORSPACE_RGB || image->get_bits_per_sample() != 8) {
        panel.AppRunAnimationLauncherCompleted();
        return;
    }

    gint x, y, i;
    int w = image->get_width();
    int h = image->get_height();
    int channels = image->get_n_channels();
    gint rowstride = image->get_rowstride();
    gint pixel_offset;

    for (i = 0; i < 4; i++) {
        for (y = 0; y < h; y++) {
            for (x = 0; x < w; x++) {
                pixel_offset = y * rowstride + x * channels;
                guchar* pixel = &image->get_pixels()[pixel_offset];

                pixel[0] = 255 - pixel[0];
                pixel[1] = 255 - pixel[1];
                pixel[2] = 255 - pixel[2];
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    panel.AppRunAnimationLauncherCompleted();
    panel.update();
}

