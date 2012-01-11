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
        float border) {
    logical_window_ = add_borders(window, border);
    update_matrices();
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

void LogicalPaintedWidget::update_matrices() {
    WRectF out = device_window();
    WRectF& in = logical_window_;
    ThreeWPoints from(in.topLeft(), in.topRight(), in.bottomLeft());
    ThreeWPoints to(out.topLeft(), out.topRight(), out.bottomLeft());
    logical2device_ = Matrix3x3(from, to);
    device2logical_ = logical2device_.inverted();
}

}

}

