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

#include <chrono>
#include <thread>

using namespace std::chrono_literals;

bool DockPreview::threadRunning;
//DockItem* DockPreview::m_itemToDetectMovement;

std::vector<DockItem*> DockPreview::m_previewItems;
DockPreview::DockPreview():Gtk::Window(Gtk::WindowType::WINDOW_POPUP){

    DockPreview::threadRunning = true;
  //  DockPreview::m_itemToDetectMovement = nullptr;

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
    // tell the background thread to terminate.
    this->threadRunning = false;

    // Detach
    m_thread->detach();

    // free memory
    delete m_thread;

    // pointed dangling to ptr NULL
    m_thread = NULL;

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
    }


    int x = 0;
    int y = 0;
    guint hv_diff = 20;
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
    //    A
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
 * This method will be call from the background tread to manage the application image animation.
 * The animation consists in invert the colors of the image.
 */
void DockPreview::MovementDetector()
{

    guint w,h, x, y, i;
    GdkRectangle boundingbox;
    while(threadRunning)
    {
        for(DockItem* item : DockPreview::m_previewItems){

            bool found = false;
            GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), item->m_xid);
            if(wm_window == nullptr){
                continue;
            }

            gdk_window_get_frame_extents(wm_window, &boundingbox);
          //  guchar* buff = (guchar*) std::malloc(((boundingbox.width * boundingbox.height)*3) * sizeof(guchar));

//            GdkPixbuf* base = nullptr;
            gint pixel_offset = 0;
//Returns a read-only pointer to the raw pixel data; must not be modified. This function allows skipping the implicit copy that must be made if gdk_pixbuf_get_pixels() is called on a read-only pixbuf.
            const guint8* pixels_first = nullptr;
            const guint8* pixels_current = nullptr;
            GdkPixbuf* pixbuf = nullptr;
            g_print("Start Scann for %d\n",item->m_xid );

            for (i = 0 ; i < 4 ; i++){
                pixbuf = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width, boundingbox.height);
                pixels_current = gdk_pixbuf_read_pixels(pixbuf);

                if(i == 0 && pixbuf != nullptr) {
//                    base = gdk_pixbuf_copy(current);
                    pixels_first = gdk_pixbuf_read_pixels(pixbuf);
                    g_print("SET\n");
                    continue;
                }


                int w = gdk_pixbuf_get_width(pixbuf);
                int h = gdk_pixbuf_get_height(pixbuf);
                int channels = gdk_pixbuf_get_n_channels(pixbuf);
                gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
                found = false;
                for (y = 0; y < h; y++) {
                    for (x = 0; x < w; x++) {
                        pixel_offset = y * rowstride + x * channels;


                        auto pixel_first = pixels_first[pixel_offset];
                        auto pixel_current = pixels_current[pixel_offset];

                        if (pixel_first !=  pixel_current )
                        {
                            found = true;
//                        g_print(" %d %d\n", pixel_first, pixel_current);
                        break;

                        }

 //                       this->m_pixelsbuf[pixel_offset] = pixels[ pixel_offset];
                        //guchar* pixel = &m_AppRunImage->get_pixels()[pixel_offset];
                        //
                    }

                    if(found)
                    break;
                }

            //    delete[] pixels_first;
            //    delete pixels_current;
                g_object_unref(pixbuf);

//                if (item->isMovementDetected(pb)){
//                     g_print("Movement detected %d\n", item->m_xid);
//                }


                    if(found){
                        break;
                    }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }


                    if(found){
            ;
            g_print("Found  %s\n",item->m_titlename.c_str());
            item->m_isDynamic = true;
                    }

          // std::free(buff);
        }


//                    GdkPixbuf* scaledpb = GetPreviewImage(DockPreview::m_itemToDetectMovement);
 //                   if (scaledpb == nullptr){

   //                 }

        g_print("Finish Thread\n");
        break;

    //GdkRectangle boundingbox;
    //gdk_window_get_frame_extents(wm_window, &boundingbox);

    //GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width, boundingbox.height);
    //if (pb == nullptr) {
        //return nullptr;
    //}

    //return pb;

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
       }

        /*if (m_AppRunImage && m_AppRunImage->get_colorspace() == Gdk::COLORSPACE_RGB && m_AppRunImage->get_bits_per_sample() == 8){
            int w = m_AppRunImage->get_width();
            int h = m_AppRunImage->get_height();
            int channels = m_AppRunImage->get_n_channels();
            gint rowstride = m_AppRunImage->get_rowstride();
            gint pixel_offset;
            for (i = 0; i < 6; i++) {
                for (y = 0; y < h; y++) {
                    for (x = 0; x < w; x++) {
                        pixel_offset = y * rowstride + x * channels;
                        guchar* pixel = &m_AppRunImage->get_pixels()[pixel_offset];

                        pixel[0] = 255 - pixel[0];
                        pixel[1] = 255 - pixel[1];
                        pixel[2] = 255 - pixel[2];
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            m_AppRunImage = NULLPB;
        }
        */


}

bool DockPreview::on_timeoutDraw(){

if(!m_firstDrawDone){


     //  m_firstDrawDone = true;
}
        Gtk::Widget::queue_draw();
    return true;
}
void DockPreview::on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data){

}
void DockPreview::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data){

}



bool DockPreview::on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{

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

            //if(!m_firstDrawDone )
            {

                if( item->m_image == NULLPB || item->m_isDynamic ) {
                    GdkPixbuf* scaledpb = GetPreviewImage(item);
                    if (scaledpb !=  nullptr){
                        //Glib::RefPtr<Gdk::Pixbuf> preview = IconLoader::PixbufConvert(scaledpb);
                        item->m_image = IconLoader::PixbufConvert(scaledpb);

                    }
                }

                if (item->m_image != NULLPB){

                        Gdk::Cairo::set_source_pixbuf(cr,item->m_image, x + 2 , y + 32 );
                        cr->paint();

                }

            }

            y +=  this->cellHeight + Configuration::get_separatorMargin();

        }

    }

       m_firstDrawDone = true;

    if(m_firstDrawDone )
        g_print("HAS DRAW\n");

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


GdkPixbuf* DockPreview::GetImage(DockItem* item)
{

    GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), item->m_xid);
    if(wm_window == nullptr){
        return nullptr;
    }

    GdkRectangle boundingbox;
    gdk_window_get_frame_extents(wm_window, &boundingbox);

    GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width, boundingbox.height);
    if (pb == nullptr) {
        return nullptr;
    }

    return pb;
}

GdkPixbuf* DockPreview::GetPreviewImage(DockItem* item)
{
    GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), item->m_xid);
    if(wm_window == nullptr){
        return nullptr;
    }

    GdkRectangle boundingbox;
    gdk_window_get_frame_extents(wm_window, &boundingbox);

    GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width - 10, boundingbox.height - 30);
    //GdkPixbuf *pb = gdk_pixbuf_get_from_window(wm_window, 0, 0, boundingbox.width - 1, boundingbox.height - 1);
    if (pb == nullptr) {
        return nullptr;
    }
   int height = this->cellHeight; //m_previewHeight - DEF_PREVIEW_SCALE_HEIGHT_OFFSET;
    int width = this->cellWidth;//m_previewWidth - DEF_PREVIEW_SCALE_WIDTH_OFFSET;
    int scale_heght = height;

    int windowheight = gdk_window_get_height(wm_window);

    int heightHalf = (height / 2);

    if (windowheight < 300)
        scale_heght = heightHalf;
    if (windowheight < 200)
        scale_heght = heightHalf - 20;
    if (windowheight < 100)
        scale_heght = heightHalf - 40;
    if (windowheight < 50)
        scale_heght = heightHalf - 60;

    if (scale_heght < 10)
        scale_heght = height;





    // offers reasonable quality and speed.
    GdkPixbuf* scaledpb = gdk_pixbuf_scale_simple(pb,this->cellWidth-20, this->cellHeight-34, GDK_INTERP_BILINEAR);
    //GdkPixbuf* scaledpb = gdk_pixbuf_scale_simple(pb,width, scale_heght, GDK_INTERP_BILINEAR);
    if (scaledpb == nullptr) {
        g_object_unref(pb);
        return nullptr;
    }


    g_object_unref(pb);
    return scaledpb;

}





