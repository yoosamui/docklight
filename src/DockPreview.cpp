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
#include "WnckHandler.h"
#include "DockItem.h"
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <chrono>
#include <thread>
#include <algorithm>

#define PREVIEW_TITLE_OFFSET 28//32
#define PREVIEW_HV_OFFSET 20

using namespace std::chrono_literals;

bool DockPreview::threadRunning;
std::vector<DockItem*> DockPreview::m_previewItems;
bool DockPreview::m_detectMovement;
bool DockPreview::m_allowDraw;

DockPreview::DockPreview():Gtk::Window(Gtk::WindowType::WINDOW_POPUP){

//(docklight:31507): Gdk-WARNING **: 20:23:48.099: XID collision, trouble ahead
//Preview destroy.
    DockPreview::threadRunning = false; //////////////////////////////////// find the buf
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

}
/**
 * Destructor
 */
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

void DockPreview::Show(const std::vector<DockItem*>& items, const guint index, const guint cellSize)
{
    this->scanSet = false;

    // Using assignment operator to copy the items vector
    DockPreview::m_previewItems = items;
    for (DockItem* item : DockPreview::m_previewItems){
        item->m_isDynamic = true;
        item->m_image = NULLPB;
        item->m_scaledPixbuf = nullptr;
    }

    int x = 0;
    int y = 0;

    guint windowWidth = cellSize * 6;
    guint windowHeight = windowWidth -  (PREVIEW_HV_OFFSET * 2);

    m_cellWidth = windowWidth - 20;
    m_cellHeight =windowHeight - 20 ;  m_cellWidth - (PREVIEW_HV_OFFSET * 2);

    guint separatorsSize = Configuration::get_separatorMargin() * (items.size() - 1);

    if (DockWindow::is_Horizontal()){
        windowWidth = m_cellWidth  *  items.size() + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;

        if (windowWidth > DockWindow::Monitor::get_geometry().width){

            m_cellWidth = (DockWindow::Monitor::get_geometry().width - DockWindow::get_dockWindowStartEndMargin() -  separatorsSize ) / items.size();

            windowWidth =m_cellWidth * items.size()+ separatorsSize + PREVIEW_HV_OFFSET;
            windowHeight = m_cellWidth -  (PREVIEW_HV_OFFSET * 2);

            m_cellHeight = windowHeight - PREVIEW_HV_OFFSET ;

        }

        this->set_size_request(windowWidth, windowHeight);
        DockItemPositions::get_CenterPosition(index, x, y, windowWidth, windowHeight);



    } else {

        windowHeight = m_cellHeight * items.size() + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;

        if (windowHeight  > DockWindow::Monitor::get_geometry().height){

            m_cellHeight =   (DockWindow::Monitor::get_geometry().height - DockWindow::get_dockWindowStartEndMargin() - separatorsSize ) / items.size();

            windowHeight =m_cellHeight * items.size() + separatorsSize + PREVIEW_HV_OFFSET;
            windowWidth = m_cellWidth;

            m_cellWidth = windowWidth - PREVIEW_HV_OFFSET;
        }

        this->set_size_request(windowWidth  , windowHeight);
        DockItemPositions::get_CenterPosition(index, x, y, windowWidth,  windowHeight);
    }

    this->move(x, y);
    this->show_all();
    // Start the background thread
    m_thread = new std::thread(this->MovementDetector);

   // start the timer for post movement detections
   // m_detectMovementTimer.start();
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



    //m_previewItems.clear();
    DockPanel::PreviewClose();
}


/**
 * Compares the given pixbuf data.
 *
 * @params  const GdkPixbuf* pixbuf
 * @params  const guint8* pixels_first
 * @params  const guint8* pixels_current
 *
 * @return true if the pixbuf data contains diferences, otherwise false.
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

                for (i = 0 ; i < 4 ; i++){
                    if (i == 0) {
                        // This function will create an RGB pixbuf with 8 bits per channel with the size specified by the
                        // width and height arguments scaled by the scale factor of window . The pixbuf will contain
                        // an alpha channel if the window contains one.
                        pixbuf_first = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width, boundingbox.height);
                        if (pixbuf_first == nullptr){
                            break;
                        }

                        // Returns a read-only pointer to the raw pixel data; must not be modified.
                        // This function allows skipping the implicit copy that must be made if
                        // gdk_pixbuf_get_pixels() is called on a read-only pixbuf.
                        pixels_first = gdk_pixbuf_read_pixels(pixbuf_first);
                        if (pixbuf_first == nullptr){
                            break;
                        }
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


                    if(found){
                        break;
                    }

                    item->m_isDynamic = false;

                    if (!DockPreview::threadRunning){
                        break;
                    }

                    // One second is slow but accurate
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
    bool detectMovement = DockPreview::m_detectMovement;
    DockPreview::m_detectMovement = false;
    DockPreview::m_allowDraw = false;

    guint index = 0;
    for (DockItem* item : m_previewItems){
        if (wnck_window_get_xid(window) == item->m_xid){

        // Deletes the element by index;
        m_previewItems.erase(m_previewItems.begin() + index);
        }

        index++;
    }

    DockPreview::m_allowDraw = false;
    DockPreview::m_detectMovement = detectMovement;
}


/**
 * handles Mouse button press : process mouse button event. true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 *
 * @param event
 * @return true to stop other handlers,false to propagate the event further.
 * here we acivate the window and also close the window.
 */
bool DockPreview::on_button_press_event(GdkEventButton *event)
{
g_print("MousePress\n");


    if ((event->type == GDK_BUTTON_PRESS)) {
        // Check if the event is a left button click.
        if (event->button == 1) {
            if (m_currentIndex < 0  ||  m_currentIndex > DockPreview::m_previewItems.size()){


                return true;
            }

            DockItem *item = DockPreview::m_previewItems[m_currentIndex];

            // Handle close preview window
            /*int pos_x = (m_previewWidth - 5) + (m_previewWidth * m_currentIndex);
            if (event->x >= pos_x && event->x <= pos_x + DEF_PREVIEW_LEFT_MARGING && // FIXTHIS: use rectangle instead.
                    event->y >= 19 && event->y <= 19 + DEF_PREVIEW_LEFT_MARGING) {

                wnck_window_close(item->m_window, gtk_get_current_event_time());
                m_isActive = false;

                return true;
            }*/

            // Activate and/or minimize the window
            WnckHandler::ActivateWindow(item->m_window);

            // reload the image and checks if its have movement.
            // In this case it will change from static to Dynamic.
            // This can happen when a browser play a video and gets minimized and
            // stops playing. When it gets unminimized should play again in the preview.
            //if (!item->m_isDynamic) {

                //item->m_imageLoadedRequired = true;
            //}

            // The event has been handled.
            return true;
        }
    }
    return true;
}


/**
 * handles on_motion_notify_event. true to stop other handlers from being invoked for the event.
 * false to propagate the event further.
 *
 * @param event GdkEventMotion
 * @return true/false
 */
bool DockPreview::on_motion_notify_event(GdkEventMotion*event)
{
    m_currentIndex = this->get_Index(event->x, event->y);
/*
    if (m_currentIndex < 0  ||  m_currentIndex > DockPreview::m_previewItems.size()){
        return true;
    }

    if (!m_allowDraw){
        return true;
    }

    DockItem *item = DockPreview::m_previewItems[m_currentIndex];

    // Activate and/or minimize the window
    WnckHandler::SelectWindow(item->m_window);*/
    return true;
}

/**
 * calculate the item index from mouse coordinates.
 * @param x
 * @param y
 * @return the item index or -1 if the item could not be found.
 */
inline int DockPreview::get_Index(const int& mouseX, const int& mouseY)
{
    Gdk::Point mouse(mouseX, mouseY);
    int idx = 0;
    int x = DockWindow::get_dockWindowStartEndMargin() / 2;
    int y = x;

    if (DockWindow::is_Horizontal()) {
        for (auto item : DockPreview::m_previewItems) {
            if (mouse.get_x() >= x && mouse.get_x() <= x + m_cellWidth) {
                return idx;
            }

            x += m_cellWidth + Configuration::get_separatorMargin();
            idx++;
        }
    }
    else
    {
        int height;
        for (auto item : DockPreview::m_previewItems) {
            if (mouse.get_y() >= y && mouse.get_y() <= y + m_cellHeight) {
                g_print(" idx:%d\n", idx);
                return idx;
            }

            y += m_cellHeight + Configuration::get_separatorMargin();
            idx++;
        }
    }

    return -1;
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

    guint centerX = 0;
    guint centerY = 0;
    guint imgcenterX = 0;
    guint imgcenterY = 0;
    guint titleofsetX = 0;
    guint titleofsetY = 0;
    guint titlecliping = 0;

    if (m_Theme.Preview().Fill().Color::alpha > 0.f) {
        cr->set_source_rgba(
            m_Theme.Preview().Fill().Color::red,
            m_Theme.Preview().Fill().Color::green,
            m_Theme.Preview().Fill().Color::blue,
            m_Theme.Preview().Fill().Color::alpha);

        Utilities::RoundedRectangle(cr, 0, 0, this->get_width(), height, m_Theme.Preview().Ratio());
        cr->fill();
    }

    if (DockWindow::is_Horizontal()){
        y = 0;
        x =  DockWindow::get_dockWindowStartEndMargin() / 2 ;
    }
    else {
        x = 0;
        y = DockWindow::get_dockWindowStartEndMargin() / 2 ;
    }

    for (DockItem* item : m_previewItems) {

        if(!item->m_image || item->m_isDynamic ) {
            if(item->m_scaledPixbuf){
                g_object_unref(item->m_scaledPixbuf);
            }

            item->m_scaledPixbuf = GetPreviewImage(item, item->m_scaleWidth, item->m_scaleHeight);
            if (item->m_scaledPixbuf !=  nullptr){
                item->m_image = Glib::wrap(item->m_scaledPixbuf, true);
            }
        }

        if (DockWindow::is_Horizontal()) {
            centerY = this->get_height() / 2 - (m_cellHeight / 2);
            centerX = 0;

            titleofsetX = 5;
            titleofsetY = 16;
        }
        else {
            centerX = this->get_width() / 2 - m_cellWidth / 2;
            centerY = 0;

            titleofsetX = 16;
            titleofsetY = 5;
        }

        // Fill cell
        if( m_Theme.PreviewCell().Fill().Color::alpha > 0.f ) {

            cr->set_source_rgba(
                    m_Theme.PreviewCell().Fill().Color::red,
                    m_Theme.PreviewCell().Fill().Color::green,
                    m_Theme.PreviewCell().Fill().Color::blue,
                    m_Theme.PreviewCell().Fill().Color::alpha);

            Utilities::RoundedRectangle(cr, x + centerX, y + centerY, m_cellWidth, m_cellHeight, m_Theme.PreviewCell().Ratio());
            cr->fill();
        }

        // Selector
        if (idx == m_currentIndex) {

            cr->set_source_rgba(
                    m_Theme.Selector().Fill().Color::red,
                    m_Theme.Selector().Fill().Color::green,
                    m_Theme.Selector().Fill().Color::blue,
                    m_Theme.Selector().Fill().Color::alpha);

            Utilities::RoundedRectangle(cr, x + centerX, y + centerY, m_cellWidth, m_cellHeight, m_Theme.Selector().Ratio());
            cr->fill();

            cr->set_source_rgba(
                    m_Theme.Selector().Stroke().Color::red,
                    m_Theme.Selector().Stroke().Color::green,
                    m_Theme.Selector().Stroke().Color::blue,
                    m_Theme.Selector().Stroke().Color::alpha);
            cr->set_line_width(m_Theme.Selector().LineWidth());

            Utilities::RoundedRectangle(cr, x + centerX, y + centerY, m_cellWidth, m_cellHeight, m_Theme.Selector().Ratio());
            cr->stroke();

        }


        // draw title the clipping rectangle
        //       cr->set_source_rgba(1.0, 1.0, 1.0, 0.0);
        if (idx == m_currentIndex) {

            cr->rectangle(x + titleofsetX, y + titleofsetY, m_cellWidth  - 30, PREVIEW_TITLE_OFFSET - 10);
        }
        else {

            cr->rectangle(x + titleofsetX, y + titleofsetY, m_cellWidth - 12, PREVIEW_TITLE_OFFSET - 10);
        }

        cr->clip_preserve();
        cr->stroke();
        cr->set_source_rgba(1,1,1,1);
        auto layout = create_pango_layout(item->m_appname);
        layout->set_font_description(m_font);

        cr->set_source_rgba(1.0, 1.0, 1.0, 1.0); // white text
        cr->move_to(x + titleofsetX, y + titleofsetY);
        layout->show_in_cairo_context(cr);
        cr->reset_clip(); // Reset the clipping


        // Draw image
        if (DockWindow::is_Horizontal()) {
                imgcenterX = m_cellWidth / 2 -  item->m_scaleWidth / 2;
                imgcenterY = (m_cellHeight - 12) / 2 - item->m_scaleHeight / 2;
        }
        else {
                 imgcenterX = (m_cellWidth / 2 -  item->m_scaleWidth / 2) + 10;
                 imgcenterY = (m_cellHeight - PREVIEW_TITLE_OFFSET) / 2 - item->m_scaleHeight / 2;
        }

        // draw image
        if (item->m_image){
            Gdk::Cairo::set_source_pixbuf(cr,item->m_image, x + imgcenterX  ,  y + PREVIEW_TITLE_OFFSET + imgcenterY - 1);
            cr->paint();
        }

        // Stroke
        if( m_Theme.PreviewCell().Stroke().Color::alpha > 0.f ){
            cr->set_source_rgba(
                    m_Theme.PreviewCell().Stroke().Color::red,
                    m_Theme.PreviewCell().Stroke().Color::green,
                    m_Theme.PreviewCell().Stroke().Color::blue,
                    m_Theme.PreviewCell().Stroke().Color::alpha);

            Utilities::RoundedRectangle(cr, x + centerX, y + centerY, m_cellWidth, m_cellHeight, m_Theme.PreviewCell().Ratio());
            cr->stroke();
        }


        if (DockWindow::is_Horizontal()) {
            x +=  m_cellWidth + Configuration::get_separatorMargin();
        }
        else {
            y +=  m_cellHeight + Configuration::get_separatorMargin();
        }

        idx++;
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

    int width = m_cellWidth ;
    int height = m_cellHeight - PREVIEW_TITLE_OFFSET;

    double minSize = std::min(width, height);
    double maxSize = std::max(winWidth, winHeight);
    double aspectRatio = minSize / maxSize;

    scaleWidth = winWidth * aspectRatio ;
    scaleHeight = winHeight * aspectRatio;

    // ajust width size to make looks better
    if(winWidth - 50 > DockWindow::Monitor::get_geometry().width / 2 ) {
        scaleWidth = width  - 4;
    }

    // ajust height size to make looks better
    if(winHeight - 50 > DockWindow::Monitor::get_geometry().height / 2 ) {
       scaleHeight = height - 2;
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


