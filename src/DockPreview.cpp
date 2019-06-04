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
#include "Utilities.h"
#include "WnckHandler.h"
#include "DockItem.h"
#include <gdk/gdkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>

#define PREVIEW_FRAMES_PERSECOND  8
#define PREVIEW_TITLE_OFFSET 28
#define PREVIEW_HV_OFFSET 20
#define PREVIEW_MAX_ELAPSED_FRAMES PREVIEW_FRAMES_PERSECOND * 3

using namespace std::chrono_literals;

std::vector<DockItem*> DockPreview::m_previewItems;
bool DockPreview::m_allowDraw;
bool DockPreview::m_updateRequired;

DockPreview::DockPreview():Gtk::Window(Gtk::WindowType::WINDOW_POPUP)
{

    DockPreview::m_allowDraw = true;
    DockPreview::m_updateRequired = false;

    // Set up the top-level window.
    set_title("DockPreview");
    set_decorated(false);
    set_app_paintable(true);

    // Set masks for mouse events
    add_events(
            Gdk::BUTTON_PRESS_MASK |
            Gdk::BUTTON_RELEASE_MASK |
            Gdk::SCROLL_MASK |
            Gdk::SMOOTH_SCROLL_MASK |
            Gdk::POINTER_MOTION_HINT_MASK |
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

    Glib::signal_timeout().connect(sigc::mem_fun(*this, &DockPreview::on_timeoutDraw), 1000 / PREVIEW_FRAMES_PERSECOND);
    g_signal_connect(G_OBJECT(wnckscreen), "window-opened", G_CALLBACK(DockPreview::on_window_opened), NULL);
    g_signal_connect(wnckscreen, "window_closed", G_CALLBACK(DockPreview::on_window_closed), NULL);

}
/**
 * Destructor
 */
DockPreview::~DockPreview()
{
   // g_object_unref static items
    for (DockItem* item : m_previewItems) {
        if (item->m_scaledPixbuf) {
            g_object_unref(item->m_scaledPixbuf);
        }
    }

    DockPreview::m_previewItems.clear();
    g_print("Preview destroy.\n");
}

void DockPreview::Show(const std::vector<DockItem*>& items, const guint index, const guint cellSize)
{

    // Using assignment operator to copy the items vector
    DockPreview::m_previewItems = items;
    m_dockItemIndex = index;
    m_dockItemCellSize = cellSize;

    for (DockItem* item : DockPreview::m_previewItems){
        item->m_isDynamic = true;
        item->m_image = NULLPB;
        item->m_firstImage = NULLPB;
        item->m_scaledPixbuf = nullptr;
       // item->m_scaledPixels = nullptr;
        item->m_elapsedFrames = 0;
        item->m_tmpxid = 0;
        item->m_isAlive = true;

        /*if(item->m_window &&  wnck_window_is_minimized (item->m_window)) {
                   auto timestamp = gtk_get_current_event_time();
                   wnck_window_unminimize (item->m_window, timestamp);
                   //wnck_window_minimize (item->m_window);

        }*/
    }

    // Buble sort by appname name
    int size = (int) m_previewItems.size();
    int i, m, j;

    for (i = 0; i < size - 1; i = i + 1) {
        m = i;
        for (j = i + 1; j < size; j = j + 1) {

            std::string s1 = m_previewItems.at(j)->m_appname.c_str();
            std::string s2 = m_previewItems.at(m)->m_appname.c_str();

            s1 = s1.substr(0, 40);
            s2 = s2.substr(0, 40);

            std::string a = Utilities::stringToLower(s1.c_str());
            std::string b = Utilities::stringToLower(s2.c_str());

            if (a < b) {
                m = j;
            }
        }
        std::swap(m_previewItems[i], m_previewItems[m]);
    }

//    this->Update();
//    this->show();
};


inline guint DockPreview::get_CountItems()
{
    guint count = 0;
    for (DockItem* item : DockPreview::m_previewItems){
        if (!item->m_isAlive){
            continue;
        }
        count++;
    }

    return count;
}


bool DockPreview::Update()
{
    if ((int) m_previewItems.size() == 0) {
        return false;
    }

    int x = 0;
    int y = 0;

    guint windowWidth = (m_dockItemCellSize * 4) + Configuration::get_previewSize();
    guint windowHeight = windowWidth -  (PREVIEW_HV_OFFSET * 2);

    m_cellWidth = windowWidth - 20;
    m_cellHeight =windowHeight - 20 ;  m_cellWidth - (PREVIEW_HV_OFFSET * 2);

    guint itemsSize = get_CountItems();
    guint separatorsSize = Configuration::get_separatorMargin() * (itemsSize - 1);

    if (DockWindow::is_Horizontal()){
        windowWidth = m_cellWidth  *  itemsSize + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;
        if (windowWidth > DockWindow::Monitor::get_workarea().width){
            m_cellWidth = (DockWindow::Monitor::get_workarea().width - DockWindow::get_dockWindowStartEndMargin() -  separatorsSize ) / itemsSize;

            windowWidth =m_cellWidth * itemsSize + separatorsSize + PREVIEW_HV_OFFSET;
            windowHeight = m_cellWidth -  (PREVIEW_HV_OFFSET * 2) - DockWindow::Monitor::get_workarea().x;

            m_cellHeight = windowHeight - PREVIEW_HV_OFFSET ;
        }

    } else {
        windowHeight = m_cellHeight * itemsSize + DockWindow::get_dockWindowStartEndMargin() + separatorsSize;
        if (windowHeight  > DockWindow::Monitor::get_workarea().height){
            m_cellHeight =   (DockWindow::Monitor::get_workarea().height - DockWindow::get_dockWindowStartEndMargin() - separatorsSize ) / itemsSize;

            windowHeight =m_cellHeight *  itemsSize + separatorsSize + PREVIEW_HV_OFFSET;
            windowWidth = m_cellWidth;

            m_cellWidth = windowWidth - PREVIEW_HV_OFFSET;
        }
    }

    if ( m_cellHeight < 40 || m_cellHeight < 40) {
            Utilities::Messages::LimitReachedMessage();
            return false;
    }


    resize(windowWidth  , windowHeight);
    DockItemPositions::get_CenterPosition(m_dockItemIndex, x, y, windowWidth,  windowHeight);
    this->move(x, y);

    this->set_ItemsDynamic();

    return true;

}


/*Server::type_signal_something Server::signal_something()
{
  return m_signal_something;
}

void DockPreview::do_something()
{
  m_signal_something.emit(false, 5);
}*/

void DockPreview::set_ItemsDynamic()
{
    for (DockItem* item : m_previewItems) {
        item->m_isDynamic = true;
        item->m_elapsedFrames = 0;
    }
}


void DockPreview::set_CurrentItemDynamic()
{
    if (m_currentIndex < 0 || m_currentIndex > m_previewItems.size() ) {
        return;
    }

    m_previewItems[m_currentIndex]->m_isDynamic = true;
    m_previewItems[m_currentIndex]->m_elapsedFrames = 0;
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
    m_mouseIn = true;
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
    m_mouseIn = false;
    return true;
}

/**
 * Hide the window and reset values;
 */
void DockPreview::Close()

{
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
inline bool DockPreview::ComparePixels (const GdkPixbuf* pixbuf, const guint8* pixels_first, const guint8* pixels_current)
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
 * Compare the pixels for equality
 *
 * @param first_pixbuf to compare
 * @param current_pixbuf to compare
 *
 * @return true if are equal otherwise false.
 */
inline bool DockPreview::ComparePixels (const Glib::RefPtr<Gdk::Pixbuf>& first_pixbuf, const  Glib::RefPtr<Gdk::Pixbuf>& current_pixbuf)
{
    if(!current_pixbuf || !current_pixbuf){
        return false;
    }

    if (first_pixbuf->get_bits_per_sample() != 8 || first_pixbuf->get_colorspace() != Gdk::COLORSPACE_RGB) {
        return false;
    }

    int  x, y;
    int  w = first_pixbuf->get_width();
    int h = first_pixbuf->get_height();
    int channels = first_pixbuf->get_n_channels();
    gint rowstride = first_pixbuf->get_rowstride();
    gint pixel_offset = 0;

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            pixel_offset = y * rowstride + x * channels;

            guchar* firstPixel = &first_pixbuf->get_pixels()[pixel_offset];
            guchar* currentPixel = &current_pixbuf->get_pixels()[pixel_offset];

            if (firstPixel[0] != currentPixel[0] || firstPixel[1] != currentPixel[1] ||  firstPixel[2] != currentPixel[2]){
                return false;
            }
        }
    }

    return true;
}

inline bool DockPreview::ComparePixels (const guint8* pixels, const GdkPixbuf* current_pixbuf)
{
    if(GDK_IS_PIXBUF(current_pixbuf) == false || pixels == nullptr){
        return false;
    }
    if (gdk_pixbuf_get_bits_per_sample(current_pixbuf) != 8 || gdk_pixbuf_get_colorspace(current_pixbuf) != GDK_COLORSPACE_RGB) {
        return false;
    }

    int  x, y;
    int  w = gdk_pixbuf_get_width(current_pixbuf);
    int h = gdk_pixbuf_get_height(current_pixbuf);
    int channels = gdk_pixbuf_get_n_channels(current_pixbuf);
    gint rowstride = gdk_pixbuf_get_rowstride(current_pixbuf);
    gint pixel_offset = 0;

    const guint8* pixels_current = gdk_pixbuf_read_pixels(current_pixbuf);
    if (pixels_current == nullptr){
        return false;
    }

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            pixel_offset = y * rowstride + x * channels;
            if (pixels[pixel_offset] != pixels_current[pixel_offset]) {
                return false;
            }
        }
    }

    return true;
}

bool DockPreview::on_timeoutDraw()
{
    if (!m_mouseIn && !DockPanel::is_mouseIn()) {
        int mouseX, mouseY;
        if (Utilities::getMousePosition(mouseX, mouseY)) {

            int px, py;
            this->get_position(px, py);
            bool close = false;

            if (DockWindow::is_Horizontal()) {
                close = mouseY <  py;
                if (Configuration::get_dockWindowLocation() == panel_locationType::TOP) {
                    close = mouseY > this->get_height() + py;
                }
            }
            else{
                close = mouseX < px;
                if (Configuration::get_dockWindowLocation() == panel_locationType::LEFT) {
                    close = mouseX > this->get_width() + px;
                }
            }

            if (close) {
                DockPreview::Close();
                return true;
            }
        }
    }


    if (DockPreview::m_updateRequired){

        if ((guint)DockPreview::m_previewItems.size() == 0) {
            DockPreview::Close();
            return true;
        }

        m_currentIndex = m_currentIndex > 1 ? m_currentIndex - 1 : 0;
        this->Update();

        DockPreview::m_updateRequired = false;

    }

    if (m_allowDraw){
        Gtk::Widget::queue_draw();
    }

    return true;
}


void DockPreview::on_window_opened(WnckScreen *screen, WnckWindow *window, gpointer data){

}
void DockPreview::on_window_closed(WnckScreen *screen, WnckWindow *window, gpointer data)
{
    if ((int) m_previewItems.size() == 0) {
        return;
    }

    DockPreview::m_allowDraw = false;
    guint index = 0;

    for (DockItem* item : m_previewItems){
        if (wnck_window_get_xid(window) == item->m_xid){
            if (item->m_scaledPixbuf != nullptr)
                g_object_unref(item->m_scaledPixbuf);

            item->m_isAlive = false;

            // Deletes the element by index;
            m_previewItems.erase(m_previewItems.begin() + index);
            DockPreview::m_updateRequired = true;

            break;
        }

        index++;
    }

    DockPreview::m_allowDraw = true;
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
    m_mouseDown = true;
    if ((event->type == GDK_BUTTON_PRESS)) {
        this->set_CurrentItemDynamic();
    }
    return true;
}

bool DockPreview::on_button_release_event(GdkEventButton *event)
{

    if ((event->type != GDK_BUTTON_RELEASE)) {
        m_mouseDown = false;
        return true;
    }

    // Mouse click
    if (m_mouseDown){

        m_mouseDown = false;
        // Check if the event is a left button click.
        if (event->button == 1) {
            if (m_currentIndex < 0  ||  m_currentIndex > DockPreview::m_previewItems.size()){
                return true;
            }

            DockItem *item = DockPreview::m_previewItems[m_currentIndex];

            // Handle close window
            if (m_closeSymbolMouseOver)  {
                auto window = item->m_window;
                if (window == nullptr|| item->m_xid == 0){
                    return true;
                }

// https://developer.gnome.org/libwnck/stable/WnckWindow.html#wnck-window-close



                m_allowDraw = false;

                if( WNCK_IS_WINDOW(window) == false ){
                    return true;
                }

                WnckHandler::closeByWindow(window);
//                wnck_window_close(window, gtk_get_current_event_time());

                item->m_isAlive = false;

                m_closeSymbolMouseOver = false;
                m_allowDraw = true;

                return true;
            }

            // Activate and/or minimize the window
            WnckHandler::ActivateWindow(item->m_window);

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
bool DockPreview::on_motion_notify_event(GdkEventMotion* event)
{
    if (!m_allowDraw) {
        return true;
    }

    m_currentIndex = this->get_Index(event->x, event->y);


    m_closeSymbolMouseOver = false;
    if ((int)event->x >= m_closeSymbolX && (int)event->x <= m_closeSymbolX + 24 &&
            (int)event->y >= m_closeSymbolY && (int)event->y <= m_closeSymbolY + 24) {

        m_closeSymbolMouseOver = true;
    }

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
    guint closeSymbolOffsetX = 0;

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

        if (!item->m_isAlive ){
            continue;
        }

        if (!item->m_image || item->m_isDynamic ) {

            // deref pointer
            if (item->m_scaledPixbuf != nullptr &&  GDK_IS_PIXBUF(item->m_scaledPixbuf)){
                    g_object_unref(item->m_scaledPixbuf);
            }

            item->m_scaledPixbuf = this->GetPreviewImage(item, item->m_scaleWidth, item->m_scaleHeight);
            if (item->m_scaledPixbuf != nullptr &&  GDK_IS_PIXBUF(item->m_scaledPixbuf)){
                item->m_image = Glib::wrap(item->m_scaledPixbuf, true);
            }

            if (!item->m_firstImage && item->m_elapsedFrames == 0) {
                item->m_firstImage = Glib::wrap(item->m_scaledPixbuf, true);
                item->m_tmpxid = item->m_xid;
            }

            if (item->m_firstImage && item->m_elapsedFrames == PREVIEW_MAX_ELAPSED_FRAMES && item->m_tmpxid == item->m_xid ) {
                if(this->ComparePixels(item->m_firstImage,item->m_image)){
                    item->m_isDynamic = false;
                    g_print("static: %s\n",item->get_windowName().c_str());
                }
            }

            item->m_elapsedFrames++;
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
        if ( m_Theme.PreviewCell().Fill().Color::alpha > 0.f ) {

            cr->set_source_rgba(
                    m_Theme.PreviewCell().Fill().Color::red,
                    m_Theme.PreviewCell().Fill().Color::green,
                    m_Theme.PreviewCell().Fill().Color::blue,
                    m_Theme.PreviewCell().Fill().Color::alpha);

            Utilities::RoundedRectangle(cr, x + centerX, y + centerY, m_cellWidth, m_cellHeight, m_Theme.PreviewCell().Ratio());
            cr->fill();

            cr->set_source_rgba(
                    m_Theme.PreviewCell().Stroke().Color::red,
                    m_Theme.PreviewCell().Stroke().Color::green,
                    m_Theme.PreviewCell().Stroke().Color::blue,
                    m_Theme.PreviewCell().Stroke().Color::alpha);

            Utilities::RoundedRectangle(cr, x + centerX, y + centerY, m_cellWidth, m_cellHeight, m_Theme.PreviewCell().Ratio());
            cr->stroke();
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

            // close symbol section
            closeSymbolOffsetX = DockWindow::is_Horizontal() ? 12 : 0;    // Close X offset

            m_closeSymbolX = x - closeSymbolOffsetX +  m_cellWidth - 8;
            m_closeSymbolY = y + titleofsetY - 2;



            if (m_closeSymbolMouseOver) {
                cr->save();

                cr->set_source_rgba(
                        m_Theme.PreviewClose().Fill().Color::red,
                        m_Theme.PreviewClose().Fill().Color::green,
                        m_Theme.PreviewClose().Fill().Color::blue,
                        m_Theme.PreviewClose().Fill().Color::alpha);

                cr->rectangle(m_closeSymbolX - 4, m_closeSymbolY - 4, 22, 24);
                cr->fill();

                cr->set_source_rgba(
                        m_Theme.PreviewClose().Stroke().Color::red,
                        m_Theme.PreviewClose().Stroke().Color::green,
                        m_Theme.PreviewClose().Stroke().Color::blue,
                        m_Theme.PreviewClose().Stroke().Color::alpha);

                cr->set_line_width(m_Theme.PreviewClose().LineWidth());
                cr->rectangle(m_closeSymbolX - 4, m_closeSymbolY - 4, 22, 24);
                cr->stroke();


                cr->restore();
            }

            cr->set_source_rgba(
                    m_Theme.PreviewClose().Stroke().Color::red,
                    m_Theme.PreviewClose().Stroke().Color::green,
                    m_Theme.PreviewClose().Stroke().Color::blue,
                    m_Theme.PreviewClose().Stroke().Color::alpha);
            cr->set_line_width(m_Theme.PreviewClose().LineWidth());

            cr->move_to(x - closeSymbolOffsetX +  m_cellWidth - 6, y + titleofsetY);
            cr->line_to(x - closeSymbolOffsetX +  m_cellWidth - 6, y + titleofsetY);
            cr->line_to(x - closeSymbolOffsetX +  m_cellWidth + 4, y + titleofsetY + 10);

            cr->move_to(x - closeSymbolOffsetX +  m_cellWidth + 4 , y + titleofsetY);
            cr->line_to(x - closeSymbolOffsetX +  m_cellWidth + 4 , y + titleofsetY);
            cr->line_to(x - closeSymbolOffsetX +  m_cellWidth - 6 , y + titleofsetY + 10);

            cr->stroke();

            // reduce text clipping rectangle width to make space for the close symbol
            cr->rectangle(x + titleofsetX, y + titleofsetY, m_cellWidth  - 30, PREVIEW_TITLE_OFFSET - 10);

        }
        else {
            // Text clipping rectangle
            cr->rectangle(x + titleofsetX, y + titleofsetY, m_cellWidth - 12, PREVIEW_TITLE_OFFSET - 10);

        }


        // draws the clipping rectangle invisible
        cr->set_source_rgba(1,1,1, 0.f); // for debuging set alpha to 1.f
        cr->clip_preserve();
        cr->stroke();

        // set the text color using themes
       // cr->set_source_rgba(1.0, 1.0, 1.0, 1.0); // white text
            cr->set_source_rgba(
                    m_Theme.PreviewTitleText().Stroke().Color::red,
                    m_Theme.PreviewTitleText().Stroke().Color::green,
                    m_Theme.PreviewTitleText().Stroke().Color::blue,
                    m_Theme.PreviewTitleText().Stroke().Color::alpha);

        //Create and set up a Pango layout for the clipping rectangle area
        m_refLayout = create_pango_layout(item->get_windowName());
        m_refLayout->set_font_description(m_font);

        cr->move_to(x + titleofsetX, y + titleofsetY);

        m_refLayout->show_in_cairo_context(cr);
        cr->reset_clip(); // Reset the clipping


        // set the image center
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
 * Converts the X11 window to a GdkWindow. The result will procesed to a pixbuf and scaled.
 * Return a GdkPixbuf pointer and the scale values.
 */
GdkPixbuf* DockPreview::GetPreviewImage(DockItem* item, guint& scaleWidth, guint& scaleHeight)
{
    scaleWidth = 0;
    scaleHeight = 0;

    // Wraps a native window in a GdkWindow. The function will try to look up the window using gdk_x11_window_lookup_for_display() first.
    // If it does not find it there, it will create a new window.
    // This may fail if the window has been destroyed. If the window was already known to GDK, a new reference to the existing GdkWindow is returned.
    // Returns a GdkWindow wrapper for the native window, or NULL if the window has been destroyed. The wrapper will be newly created, if one doesn’t exist already.
    GdkWindow *wm_window = gdk_x11_window_foreign_new_for_display(gdk_display_get_default(), item->m_xid);
    if(wm_window == nullptr){
        return nullptr;
    }

    if( GDK_IS_WINDOW(wm_window) == false) {
       return nullptr;
    }

    // Get the window size
    guint winWidth = gdk_window_get_width(wm_window);
    guint winHeight = gdk_window_get_height(wm_window);

    // This function will create an RGB pixbuf with 8 bits per channel with the size specified by the width and height
    // arguments scaled by the scale factor of window . The pixbuf will contain an alpha channel if the window contains one.
    // If the window is off the screen, then there is no image data in the obscured/offscreen regions to be placed in the pixbuf.
    // The contents of portions of the pixbuf corresponding to the offscreen region are undefined.
    // ---------------------------------------------------------------------------------------------------------------------------
    // If the window you’re obtaining data from is partially obscured by other windows, then the contents of the pixbuf areas
    // corresponding to the obscured regions are undefined.
    // ---------------------------------------------------------------------------------------------------------------------------
    // If the window is not mapped (typically because it’s iconified/minimized or not on the current workspace), then NULL will be returned.
    // If memory can’t be allocated for the return value, NULL will be returned instead.
    // (In short, there are several ways this function can fail, and if it fails it returns NULL; so check the return value.)
    // ---------------------------------------------------------------------------------------------------------------------------
    // creates a newly pixbuf with a reference count of 1, or NULL on error.

    GdkPixbuf *winPixbuf = gdk_pixbuf_get_from_window(wm_window, 0, 0, winWidth, winHeight);
    if (winPixbuf == nullptr) {
        return nullptr;
    }

    if (GDK_IS_PIXBUF(winPixbuf) == false){
        return nullptr;
    }

    int width = m_cellWidth ;
    int height = m_cellHeight - PREVIEW_TITLE_OFFSET;

    double minSize = std::min(width, height);
    double maxSize = std::max(winWidth, winHeight);
    double aspectRatio = minSize / maxSize;

    scaleWidth = winWidth * aspectRatio ;
    scaleHeight = winHeight * aspectRatio;

    guint half_WindowWidth =  DockWindow::Monitor::get_workarea().width / 2 ;
    guint half_WindowHeight =  DockWindow::Monitor::get_workarea().height / 2 ;

    // ajust width size to make it looks better
    if(winWidth - 20 > half_WindowWidth ) {
        if(winHeight - 20 > half_WindowHeight )
            scaleWidth = width  - 4;
    }

    // ajust height size to make it looks better
    if(winHeight - 20 > half_WindowHeight ) {
        if(winWidth - 20 > half_WindowWidth )
           scaleHeight = height - 2;
    }

    GdkPixbuf* scaledpb = gdk_pixbuf_scale_simple(winPixbuf, scaleWidth  , scaleHeight, GDK_INTERP_BILINEAR);
    if (scaledpb == NULL) {
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
https://developer.gnome.org/gdk-pixbuf/stable/gdk-pixbuf-File-saving.html#gdk-pixbuf-save
*/


