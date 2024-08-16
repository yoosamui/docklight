#pragma once

#include <gmodule.h>
#include <gtkmm/enums.h>
#include "common.h"

DL_NS_BEGIN

namespace config
{
#define DEF_MIN_ITEM_SIZE 26

    namespace style
    {
        struct Color {
            Color() {}
            Color(const double red, const double green, const double blue, const double alpha)
            {
                this->red = red;
                this->green = green;
                this->blue = blue;
                this->alpha = alpha;
            }

            double red = 0.0;
            double green = 0.0;
            double blue = 0.0;
            double alpha = 1.0;
        };

        class ColorWindow
        {
          public:
            ColorWindow(){};
            ColorWindow(const Color fill, const Color stroke, const double lineWith,
                        const double ratio, const int mask)
            {
                m_Fill = fill;
                m_Stroke = stroke;
                m_LineWidth = lineWith;
                m_Ratio = ratio;
                m_Mask = mask;
            }

            Color Fill() const { return m_Fill; }
            Color Stroke() const { return m_Stroke; }
            double LineWidth() const { return m_LineWidth; }
            double Ratio() const { return m_Ratio; }
            int Mask() const { return m_Mask; }

          private:
            // defaults
            Color m_Fill = Color(0.0, 0.50, 0.66, 0.9);
            Color m_Stroke = Color(1, 1, 1, 0);
            double m_LineWidth = 1.0;
            double m_Ratio = 0;
            int m_Mask = 0;
        };

        class Theme
        {
          public:
            Theme(){};
            ColorWindow& Panel() const { return *m_Panel; }
            ColorWindow& PanelGradient() const { return *m_PanelGradient; }
            ColorWindow& PanelCell() const { return *m_PanelCell; }
            ColorWindow& PanelDrag() const { return *m_PanelDrag; }
            ColorWindow& PanelIndicator() const { return *m_PanelIndicator; }
            ColorWindow& PanelSeparator() const { return *m_PanelSeparator; }
            ColorWindow& PanelTitle() const { return *m_PanelTitle; }
            ColorWindow& PanelTitleText() const { return *m_PanelTitleText; }
            ColorWindow& Selector() const { return *m_Selector; }
            ColorWindow& Preview() const { return *m_Preview; }
            ColorWindow& PreviewGradient() const { return *m_PreviewGradient; }
            ColorWindow& PreviewCell() const { return *m_PreviewCell; }
            ColorWindow& PreviewTitleText() const { return *m_PreviewTitleText; }
            ColorWindow& PreviewClose() const { return *m_PreviewClose; }
            ColorWindow& Separator() const { return *m_Separator; }

            void set_Panel(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_Panel != nullptr) {
                    delete m_Panel;
                    m_Panel = nullptr;
                }

                m_Panel = cw;
            }

            void set_PanelGradient(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelGradient != nullptr) {
                    delete m_PanelGradient;
                    m_PanelGradient = nullptr;
                }

                m_PanelGradient = cw;
            }

            void set_PanelDrag(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelDrag != nullptr) {
                    delete m_PanelDrag;
                    m_PanelDrag = nullptr;
                }

                m_PanelDrag = cw;
            }

            void set_PanelIndicator(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelIndicator != nullptr) {
                    delete m_PanelIndicator;
                    m_PanelIndicator = nullptr;
                }

                m_PanelIndicator = cw;
            }

            void set_PanelSeparator(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelSeparator != nullptr) {
                    delete m_PanelSeparator;
                    m_PanelSeparator = nullptr;
                }

                m_PanelSeparator = cw;
            }

            void set_PanelTitle(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelTitle != nullptr) {
                    delete m_PanelTitle;
                    m_PanelTitle = nullptr;
                }

                m_PanelTitle = cw;
            }

            void set_PanelTitleText(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelTitleText != nullptr) {
                    delete m_PanelTitleText;
                    m_PanelTitleText = nullptr;
                }

                m_PanelTitleText = cw;
            }

            void set_PanelCell(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PanelCell != nullptr) {
                    delete m_PanelCell;
                    m_PanelCell = nullptr;
                }

                m_PanelCell = cw;
            }

            void set_Selector(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_Selector != nullptr) {
                    delete m_Selector;
                    m_Selector = nullptr;
                }

                m_Selector = cw;
            }

            void set_Preview(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_Preview != nullptr) {
                    delete m_Preview;
                    m_Preview = nullptr;
                }

                m_Preview = cw;
            }

            void set_PreviewGradient(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PreviewGradient != nullptr) {
                    delete m_PreviewGradient;
                    m_PreviewGradient = nullptr;
                }

                m_PreviewGradient = cw;
            }
            void set_PreviewCell(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PreviewCell != nullptr) {
                    delete m_PreviewCell;
                    m_PreviewCell = nullptr;
                }

                m_PreviewCell = cw;
            }

            void set_PreviewTitleText(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PreviewTitleText != nullptr) {
                    delete m_PreviewTitleText;
                    m_PreviewTitleText = nullptr;
                }

                m_PreviewTitleText = cw;
            }

            void set_PreviewClose(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_PreviewClose != nullptr) {
                    delete m_PreviewClose;
                    m_PreviewClose = nullptr;
                }

                m_PreviewClose = cw;
            }

            void set_Separator(ColorWindow* cw)
            {
                if (cw == nullptr) {
                    return;
                }

                if (m_Separator != nullptr) {
                    delete m_Separator;
                    m_Separator = nullptr;
                }

                m_Separator = cw;
            }

          private:
            ColorWindow* m_Panel = new ColorWindow();
            ColorWindow* m_PanelGradient = new ColorWindow();
            ColorWindow* m_PanelDrag = new ColorWindow();
            ColorWindow* m_PanelIndicator = new ColorWindow();
            ColorWindow* m_PanelSeparator = new ColorWindow();
            ColorWindow* m_PanelTitle = new ColorWindow();
            ColorWindow* m_PanelTitleText = new ColorWindow();
            ColorWindow* m_PanelCell = new ColorWindow();
            ColorWindow* m_Selector = new ColorWindow();
            ColorWindow* m_Preview = new ColorWindow();
            ColorWindow* m_PreviewGradient = new ColorWindow();
            ColorWindow* m_PreviewCell = new ColorWindow();
            ColorWindow* m_PreviewTitleText = new ColorWindow();
            ColorWindow* m_PreviewClose = new ColorWindow();
            ColorWindow* m_Separator = new ColorWindow();
        };

    }  // namespace style

    void load(const GSList* args_list);
    dock_location_t get_dock_location();
    int get_dock_area();
    Gtk::Orientation get_dock_orientation();
    int get_separator_margin();
    int get_window_start_end_margin();
    int get_icon_size();
    void set_icon_size(int value);
    dock_indicator_type_t get_indicator_type();
    bool is_autohide_none();
    bool is_autohide();
    bool is_intelihide();
    bool is_show_title();
    bool is_separator_line();
    int get_separator_size();
    void set_separator_line(bool value);
    dock_alignment_t get_dock_alignment();
    void set_dock_alignment(dock_alignment_t value);
    float get_animation_delay();
    int get_anchor_margin();
    float get_hide_delay();
    style::Theme get_theme();
}  // namespace config

DL_NS_END

