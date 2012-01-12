/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WPainter>
#include <Wt/WRectF>
#include <Wt/WPaintDevice>
#include <Wt/WContainerWidget>

#include "LogicalPaintedWidget.hpp"
#include "Matrix3x3.hpp"

namespace Wt {

namespace Wc {

LogicalPaintedWidget::LogicalPaintedWidget(WContainerWidget* parent):
    WPaintedWidget(parent)
{ }

void LogicalPaintedWidget::set_logical_window(const WRectF& window,
        float border, bool preserve_aspect) {
    logical_window_ = add_borders(window, border);
    update_matrices(preserve_aspect);
}

WPointF LogicalPaintedWidget::logical2device(const WPointF& logical) const {
    return logical2device_.map(logical);
}

WPointF LogicalPaintedWidget::device2logical(const WPointF& device) const {
    return device2logical_.map(device);
}

WRectF LogicalPaintedWidget::device_window() const {
    return WRectF(0, 0, width().toPixels(), height().toPixels());
}

WRectF LogicalPaintedWidget::add_borders(const WRectF& rect, float border) {
    double width = rect.width();
    double height = rect.height();
    WRectF res = rect;
    res.setX(rect.x() - width * border);
    res.setY(rect.y() - height * border);
    res.setWidth(width * (1 + 2 * border));
    res.setHeight(height * (1 + 2 * border));
    return res;
}

void LogicalPaintedWidget::update_matrices(bool preserve_aspect) {
    WRectF out = device_window();
    WRectF& in = logical_window_;
    if (preserve_aspect) {
        out = change_aspect(out, in);
    }
    ThreeWPoints from(in.topLeft(), in.topRight(), in.bottomLeft());
    ThreeWPoints to(out.topLeft(), out.topRight(), out.bottomLeft());
    logical2device_ = Matrix3x3(from, to);
    device2logical_ = logical2device_.inverted();
}

Wt::WRectF LogicalPaintedWidget::change_aspect(const Wt::WRectF& rect,
        const Wt::WRectF& master) {
    float rect_aspect = rect.width() / rect.height();
    float master_aspect = master.width() / master.height();
    Wt::WRectF result = rect;
    if (rect_aspect > master_aspect) {
        // change width
        float aspect_factor = rect_aspect / master_aspect;
        result.setWidth(rect.width() / aspect_factor);
        float border_part = (rect_aspect - master_aspect) / rect_aspect;
        double border_width = rect.width() * (border_part / 2);
        result.setX(rect.x() + border_width);
    } else if (rect_aspect < master_aspect) {
        // change height
        float aspect_factor = master_aspect / rect_aspect;
        result.setHeight(rect.height() / aspect_factor);
        float border_part = (master_aspect - rect_aspect) / master_aspect;
        double border_width = rect.height() * (border_part / 2);
        result.setY(rect.y() + border_width);
    }
    return result;
}

}

}

