/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_LOGICAL_PAINTED_WIDGET_HPP_
#define WC_LOGICAL_PAINTED_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WPaintedWidget>
#include <Wt/WTransform>
#include <Wt/WRectF>

namespace Wt {

namespace Wc {

/** An extension of WPaintedWidget, related to logical coordinated

\attention Don't forget to set logical coordinates window (set_logical_window())
    and to set this window and a view port from paintEvent() method
    (logical_window() and logical_view_port()).

\ingroup util
*/

class LogicalPaintedWidget : public WPaintedWidget {
public:
    /** Constructor */
    LogicalPaintedWidget(WContainerWidget* parent = 0);

    /** Set the window bounding points in logical coordinates.
    \param window  The window.
    \param border Add 5% to each side of the rectangle.
    \param preserve_aspect  Whether width to height ratio should be preserved.
    Logical window defaults to null WRectF.
    */
    void set_logical_window(const WRectF& window, float border = 0.05,
            bool preserve_aspect = true);

    /** Return the window bounding points in logical coordinates.
    You MUST set this window to the painter in paintEvent() method.
    */
    const WRectF& logical_window() {
        return logical_window_;
    }

    /** Return the window bounding points in logical coordinates.
    You MUST set this view port to the painter in paintEvent() method.
    */
    const WRectF& logical_view_port() {
        return logical_view_port_;
    }

    /** Map the point from logical to device coordinates */
    WPointF logical2device(const WPointF& logical) const;

    /** Map the point from device to logical coordinates */
    WPointF device2logical(const WPointF& device) const;

    /** Get the matrix used for device-to-logical mapping */
    const WTransform& device2logical_matrix() const {
        return device2logical_;
    }

    /** Get the matrix used for logical-to-device mapping */
    const WTransform& logical2device_matrix() const {
        return logical2device_;
    }

    /** Return rectangle representing device coordinates */
    WRectF device_window() const;

    /** Return the rectangle with added borders.
    \param rect   Input rectangle.
    \param border A fraction of width/height to be added to each side.
    */
    static WRectF add_borders(const WRectF& rect, float border);

private:
    WRectF logical_window_;
    WRectF logical_view_port_;
    WTransform device2logical_;
    WTransform logical2device_;

    void update_matrices(bool preserve_aspect);
    Wt::WRectF change_aspect(const Wt::WRectF& rect, const Wt::WRectF& master);
};

}

}

#endif

