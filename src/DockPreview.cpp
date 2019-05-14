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

#include "DockPreview.h"
#include "DockPanel.h"
#include "DockItemPositions.h"
#include "DockWindow.h"
#include "DockItem.h"
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <chrono>
#include <thread>
#include <algorithm>

#define PREVIEW_TITLE_OFFSET 32
#define PREVIEW_HV_OFFSET 20

using namespace std::chrono_literals;

bool DockPreview::threadRunning;
std::vector<DockItem*> DockPreview::m_previewItems;
bool DockPreview::m_detectMovement;
bool DockPreview::m_allowDraw;

DockPreview::DockPreview():Gtk::Window(Gtk::WindowType::WINDOW_POPUP){

    DockPreview::threadRunning = true;
    DockPreview::m_detectMovement = true;
    DockPreview::m_allowDraw = true;

    // Set up the top-level window.
    set_title("DockPreview");
    set_decorated(false);
    set_app_paintable(true);

    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::BUTTON_PRESS_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
            Gdk::FOCUS_CHANGE_MASK |
            Gdk::ENTER_NOTIFY_MASK |
            Gdk::LEAVE_NOTIFY_MASK |
            Gdk::POINTER_MOTION_MASK);


    GdkScreen *screen;
    GdkVisual *visual;

    gtk_widget_set_app_paintable(GTK_WIDGET(gobj()), TRUE);
    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);

    if (visual != NULL && gdk_screen_is_composited(screen)) {
        gtk_widget_set_visual(GTK_WIDGET(gobj()), visual);
    }


    m_font.set_family("System");
    m_font.set_size(8 * PANGO_SCALE);
    m_font.set_weight(Pango::WEIGHT_NORMAL);

    WnckScreen *wnckscreen = wnck_screen_get_default();

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DockPreview::on_timeoutDraw), 1000 / 8);

    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(DockPreview::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(DockPreview::on_window_closed), NULL);

    //m_timer.start();
}

DockPreview::~DockPreview()
{
    if(m_thread != nullptr){

        // tell the background thread to terminate.
        this->threadRunning = false;

        // Detach
        m_thread->detach();

        // free memory
        delete m_thread;

        // pointed dangling to ptr NULL
        m_thread = NULL;
    }

   // g_object_unref static items
    for (DockItem* item : m_previewItems) {
        if (item->m_scaledPixbuf) {
            g_object_unref(item->m_scaledPixbuf);
        }
    }

    g_print("Preview destroy.\n");
}

void DockPreview::init(const std::vector<DockItem*>& items, const guint index)
{
    this->scanSet = false;

    // Using assignment operator to copy the items vector
    DockPreview::m_previewItems = items;
    for (DockItem* item : DockPreview::m_previewItems){
        item->m_isDynamic = false;
        item->m_image = NULLPB;
        item->m_scaledPixbuf = nullptr;
    }


    int x = 0;
    int y = 0;
    guint hv_diff = PREVIEW_HV_OFFSET;
    this->cellWidth = 240;
    this->cellHeight= this->cellWidth - hv_diff;
    guint separatorsSize = Configuration::get_separatorMargin() * (items.size() - 1);

    if (DockWindow::is_Horizontal()){

        int width = this->cellWidth  *  items.size() + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;

        if (width  > DockWindow::Monitor::get_geometry().width){
            width = DockWindow::Monitor::get_geometry().width;
            this->cellWidth = (DockWindow::Monitor::get_geometry().width - DockWindow::get_dockWindowStartEndMargin() - separatorsSize ) / items.size();
            this->cellHeight = this->cellWidth - hv_diff;
        }

        this->set_size_request(width, this->cellHeight);
        DockItemPositions::get_CenterPosition(index, x, y, width, this->cellHeight );

    } else {

        // make the cell height smaller looks better
        this->cellHeight -= hv_diff;
        int height = this->cellHeight  *  items.size() + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;

        if (height  > DockWindow::Monitor::get_geometry().height){
            height = DockWindow::Monitor::get_geometry().height;
            this->cellHeight =   (DockWindow::Monitor::get_geometry().height - DockWindow::get_dockWindowStartEndMargin() - separatorsSize ) / items.size();
            this->cellWidth = this->cellHeight + hv_diff;
        }

        this->set_size_request(this->cellWidth, height);
        DockItemPositions::get_CenterPosition(index, x, y, this->cellWidth,  height);
    }

    this->move(x, y);

    // Start the background thread
    m_thread = new std::thread(this->MovementDetector);

   // start the timer for post movement detections
    m_detectMovementTimer.start();
}
/**
 * handles on_enter_notify_event
 * Event triggered by pointer enter widget area.
 * The signal will be emitted when the pointer enters the widget's window.
 * This signal will be sent to the grab widget if there is one.
 *
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param crossing_event
 * @return true to stop other handlers from being invoked for the event. false to propagate the event further
 */
bool DockPreview::on_enter_notify_event(GdkEventCrossing* crossing_event)
{
    m_canLeave = true;
    m_mouseIn = true;
    //    m_dockpanelReference->m_previewWindowActive = true;
    return true;
}

/**
 * handles on_leave_notify_event
 * Event triggered by pointer leaving widget area.
 * The signal_leave_notify_event() will be emitted when the pointer leaves the widget's window.
 * To receive this signal, the Gdk::Window associated to the widget needs to enable the Gdk::LEAVE_NOTIFY_MASK mask.
 * This signal will be sent to the grab widget if there is one.
 *
 * @param crossing_event
 * @return true to stop other handlers from being invoked for the event. false to propagate the event further
 */
bool DockPreview::on_leave_notify_event(GdkEventCrossing* crossing_event)
{
    if (!m_canLeave) {
        return true;
    }

    this->hideMe();

    // tell the caller that we are leaving...
    //  m_dockpanelReference->previewWindowClosed();
    return true;
}

/**
 * Hide the window and reset values;
 */
void DockPreview::hideMe()
{
    //  hide();

    m_canLeave = true;
    //  m_isActive = false;
    //  m_isVisible = false;
    //    m_mouseIn = false;
    //m_dockpanelReference->m_previewWindowActive = false;



    //this->m_previewItems.clear();
    DockPanel::PreviewClose();
}

/**
 * handles Mouse button press : process mouse button event
 * true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 * @param event
 * @return true to stop other handlers,false to propagate the event further.
 * here we acivate the window and also close the window.
 */
bool DockPreview::on_button_press_event(GdkEventButton *event)
{



}

/**
 * Compares the given pixbuf data.
 * @params  const GdkPixbuf* pixbuf
 * @params  const guint8* pixels_first
 * @params  const guint8* pixels_current
 *
 * @return true if the pixbuf data contains diferent data, otherwise false.
 */
inline bool ComparePixels (const GdkPixbuf* pixbuf, const guint8* pixels_first, const guint8* pixels_current)
{
    if (gdk_pixbuf_get_bits_per_sample(pixbuf) != 8 || gdk_pixbuf_get_colorspace(pixbuf) != GDK_COLORSPACE_RGB) {

        return false;
    }

    int  x, y;
    int  w = gdk_pixbuf_get_width(pixbuf);
    int h = gdk_pixbuf_get_height(pixbuf);
    int channels = gdk_pixbuf_get_n_channels(pixbuf);
    gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    gint pixel_offset = 0;

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            pixel_offset = y * rowstride + x * channels;

            if (pixels_first[pixel_offset] != pixels_current[pixel_offset]) {
            g_print("TRUE\n");
                return true;
            }
        }
    }

    return false;
}

/**
 * Detects movement by comparing the image pixels. This method runs in a thread.
 */
void DockPreview::MovementDetector()
{
    guint w,h, x, y, i;
    GdkRectangle boundingbox;
    bool found = false;
    gint pixel_offset = 0;
    GdkPixbuf* pixbuf = nullptr;
    GdkPixbuf* pixbuf_first = nullptr;

    while(threadRunning)
    {
        if (DockPreview::m_detectMovement) {
            for(DockItem* item : DockPreview::m_previewItems){
                found = false;

                if (item->m_isDynamic){
                    continue;
                }

                GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), item->m_xid);
                if(wm_window == nullptr){
                    g_warning("Movedetection failed. wm_window is NULL\n");
                    return;
                }

                gdk_window_get_frame_extents(wm_window, &boundingbox);
                const guint8* pixels_first = nullptr;
                const guint8* pixels_current = nullptr;
                pixbuf = nullptr;
                pixbuf_first = nullptr;

                if (!DockPreview::threadRunning){
                    return;
                }

                for (i = 0 ; i < 10 ; i++){
                    if (i == 0) {
                        // This function will create an RGB pixbuf with 8 bits per channel with the size specified by the
                        // width and height arguments scaled by the scale factor of window . The pixbuf will contain
                        // an alpha channel if the window contains one.
                        pixbuf_first = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width, boundingbox.height);

                        // Returns a read-only pointer to the raw pixel data; must not be modified.
                        // This function allows skipping the implicit copy that must be made if
                        // gdk_pixbuf_get_pixels() is called on a read-only pixbuf.
                        pixels_first = gdk_pixbuf_read_pixels(pixbuf_first);
                        continue;
                    }

                    // This function will create an RGB pixbuf with 8 bits per channel with the size specified by the
                    // width and height arguments scaled by the scale factor of window . The pixbuf will contain
                    // an alpha channel if the window contains one.
                    pixbuf = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width, boundingbox.height);
                    if(pixbuf == nullptr){
                        g_warning("Movedetection gdk_pixbuf_get_from_window failed. NULL\n");
                        continue;
                    }

                    // Returns a read-only pointer to the raw pixel data; must not be modified.
                    // This function allows skipping the implicit copy that must be made if
                    // gdk_pixbuf_get_pixels() is called on a read-only pixbuf.
                    pixels_current = gdk_pixbuf_read_pixels(pixbuf);

                    // if is true then a movement has been detected
                    if (ComparePixels(pixbuf, pixels_first, pixels_current)){
                        item->m_isDynamic = true;
                        found = true;
                    }

                    // release ref
                    g_object_unref(pixbuf);

                    if (!DockPreview::threadRunning){
                        return;
                    }

                    if(found){
                        break;
                    }

                    item->m_isDynamic = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                // release ref of first buff
                g_object_unref(pixbuf_first);
            }

            DockPreview::m_detectMovement = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}


bool DockPreview::on_timeoutDraw()
{
    if (m_allowDraw && !m_detectMovement && m_detectMovementTimer.elapsed() > 3.5){
    //    m_detectMovement = true;

        m_detectMovementTimer.stop();
        m_detectMovementTimer.reset();

        m_detectMovementTimer.start();
        return true;
    }

    Gtk::Widget::queue_draw();
    return true;
}


void DockPreview::on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data){

}
void DockPreview::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{

    m_detectMovement = true;
    DockPreview::m_allowDraw = false;
}



bool DockPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{

    if (!DockPreview::m_allowDraw) {
        return true;
    }

    guint idx = 0;
    guint x,y;
    guint  width = this->get_width();
    guint  height = this->get_height();


   // if (!m_firstDrawDone)
    {
        cr->set_source_rgba(
                this->m_Theme.Panel().Fill().Color::red,
                this->m_Theme.Panel().Fill().Color::green,
                this->m_Theme.Panel().Fill().Color::blue,
                this->m_Theme.Panel().Fill().Color::alpha);

        if (DockWindow::is_Horizontal()){

            Utilities::RoundedRectangle(cr, 0, 0, width, this->cellHeight ,6);
            cr->fill();
            y = 8;
            x =  DockWindow::get_dockWindowStartEndMargin() / 2 ;
        }
        else {

            Utilities::RoundedRectangle(cr, 0, 0, this->cellWidth, height, 6);
            cr->fill();
            x = 8;
            y = DockWindow::get_dockWindowStartEndMargin() / 2 ;
        }

    }

    for (DockItem* item : this->m_previewItems)
    {
  //      if(!m_firstDrawDone)
        {
            cr->set_source_rgba(
                    this->m_Theme.PanelCell().Stroke().Color::red,
                    this->m_Theme.PanelCell().Stroke().Color::green,
                    this->m_Theme.PanelCell().Stroke().Color::blue,
                    this->m_Theme.PanelCell().Stroke().Color::alpha);
        }

        if (DockWindow::is_Horizontal()){

//            if( !m_firstDrawDone)
            {
                Utilities::RoundedRectangle(cr, x, y, this->cellWidth  , this->cellHeight - 16  ,6);
                cr->stroke();
            }
            x +=  this->cellWidth + Configuration::get_separatorMargin();
        }
        else {

            Utilities::RoundedRectangle(cr, x, y, this->cellWidth - 16  , this->cellHeight  ,6);
            cr->stroke();

            if(!item->m_image || item->m_isDynamic ) {
                if(item->m_scaledPixbuf){
                    g_object_unref(item->m_scaledPixbuf);
                }

                item->m_scaledPixbuf = GetPreviewImage(item, item->m_scaleWidth, item->m_scaleHeight);
                if (item->m_scaledPixbuf !=  nullptr){
                    item->m_image = Glib::wrap(item->m_scaledPixbuf, true);
                }
            }

            if (item->m_image){
                guint centerX = (this->cellWidth / 2) -  item->m_scaleWidth / 2;
                guint centerY = ((this->cellHeight - PREVIEW_TITLE_OFFSET) / 2) -  item->m_scaleHeight / 2;
                Gdk::Cairo::set_source_pixbuf(cr,item->m_image, x + centerX - 8,  y + PREVIEW_TITLE_OFFSET + centerY - 1);
                cr->paint();
            }

            y +=  this->cellHeight + Configuration::get_separatorMargin();

        }

    }

       return true;
}

/**
 * Transfers image data from a GdkWindow and converts it to an RGB(A)
 * representation inside a GdkPixbuf. In other words, copies image data
 * from a server-side drawable to a client-side RGB(A) buffer.
 * This allows to efficiently read individual pixels on the client side.
 *
 * Create a new GdkPixbuf containing a copy of src scaled to dest_width x dest_height .
 * Leaves src unaffected. interp_type should be GDK_INTERP_NEAREST
 * if you want maximum speed (but when scaling down GDK_INTERP_NEAREST is usually unusably ugly).
 * The default interp_type should be GDK_INTERP_BILINEAR which offers reasonable quality and speed.
 *
 * @param DockItem*  item
 * @return GdkPixbuf*
 *
 */


/**
 * Converts the X11 window to a GdkWindow. The result will procesed to a pixbuf and scaled.
 * Return a GdkPixbuf pointer and the scale values.
 */
GdkPixbuf* DockPreview::GetPreviewImage(DockItem* item, guint& scaleWidth, guint& scaleHeight)
{
    // Wraps a native window in a GdkWindow. The function will try to look up the window using gdk_x11_window_lookup_for_display() first.
    // If it does not find it there, it will create a new window.
    // This may fail if the window has been destroyed. If the window was already known to GDK, a new reference to the existing GdkWindow is returned.
    // Returns a GdkWindow wrapper for the native window, or NULL if the window has been destroyed. The wrapper will be newly created, if one doesn’t exist already.
    GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), item->m_xid);
    if(wm_window == nullptr){
        return nullptr;
    }

    // Get the window size
    guint winWidth = gdk_window_get_width(wm_window);
    guint winHeight = gdk_window_get_height(wm_window);

    // This function will create an RGB pixbuf with 8 bits per channel with the size specified by the width and height
    // arguments scaled by the scale factor of window . The pixbuf will contain an alpha channel if the window contains one.
    // If the window is off the screen, then there is no image data in the obscured/offscreen regions to be placed in the pixbuf.
    // The contents of portions of the pixbuf corresponding to the offscreen region are undefined.
    // If the window you’re obtaining data from is partially obscured by other windows, then the contents of the pixbuf areas
    // corresponding to the obscured regions are undefined.
    // If the window is not mapped (typically because it’s iconified/minimized or not on the current workspace), then NULL will be returned.
    // If memory can’t be allocated for the return value, NULL will be returned instead.
    // (In short, there are several ways this function can fail, and if it fails it returns NULL; so check the return value.)
    // creates a newly pixbuf with a reference count of 1, or NULL on error.
    GdkPixbuf *winPixbuf = gdk_pixbuf_get_from_window(wm_window, 0, 0, winWidth, winHeight);
    if (winPixbuf == nullptr) {
        return nullptr;
    }

    int width = this->cellWidth;
    int height = this->cellHeight - PREVIEW_TITLE_OFFSET - 2;

    double minSize = std::min(width, height);
    double maxSize = std::max(winWidth, winHeight);
    double aspectRatio = minSize / maxSize;

    scaleWidth = winWidth * aspectRatio ;
    scaleHeight = winHeight * aspectRatio;

    // ajust width size to make looks better
    if(winWidth >= DockWindow::Monitor::get_geometry().width / 2 ) {
        scaleWidth = width - PREVIEW_HV_OFFSET;
    }

    // ajust height size to make looks better
    if(winHeight >= DockWindow::Monitor::get_geometry().height / 2 ) {
       scaleHeight = height;
    }

    // Scale with reasonable quality and speed.
    GdkPixbuf* scaledpb = gdk_pixbuf_scale_simple(winPixbuf, scaleWidth  , scaleHeight, GDK_INTERP_BILINEAR);
    if (scaledpb == nullptr) {
        g_object_unref(winPixbuf);
        return nullptr;
    }

    // unref
    g_object_unref(winPixbuf);

    // the caller should unref this pointer
    return scaledpb;
}

/*
 REFS:
    https://developer.gnome.org/gdk3/stable/gdk3-Pixbufs.html#gdk-pixbuf-get-from-window
    https://developer.gnome.org/gdk3/stable/gdk3-X-Window-System-Interaction.html#gdk-x11-window-foreign-new-for-display
    https://developer.gnome.org/gdk3/stable/gdk3-X-Window-System-Interaction.html   https://source.puri.sm/dorota.czaplejewicz/gtk/commit/1f076257059f15f37c2c93853c3eff2ec2be2aa1?w=1
    https://github.com/nobled/gtk/blob/master/gdk/gdkpixbuf-drawable.c
    https://developer.gnome.org/gdk3/stable/gdk3-Pixbufs.html#gdk-pixbuf-get-from-window
*/


