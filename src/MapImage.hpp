/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_MAP_IMAGE_HPP_
#define WC_MAP_IMAGE_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>
#include <Wt/WEvent>

namespace Wt {

namespace Wc {

/** Image, providing coordinates of mouse click even in HTML mode.
For Ajax mode, normal WImage::clicked() is used.

Implementation for HTML version is based on
<a href="http://www.w3schools.com/tags/att_img_ismap.asp">ismap</a>.

\ingroup util
*/
class MapImage : public WCompositeWidget {
public:
    /** Signal type for click */
    typedef Signal<WMouseEvent::Coordinates> ClickSignal;

    /** Constructor.
    Ownership of the image is transferred.
    */
    MapImage(WImage* image, WContainerWidget* parent = 0);

    /** Event signal emitted when a mouse key was clicked on this widget.
    First template paramater is mouse coordinates relative to the widget.

    If error occurs, return Coordinates(-1, -1).
    */
    ClickSignal& clicked() {
        return clicked_;
    }

    /** Get image */
    WImage* image();

private:
    ClickSignal clicked_;
};

}

}

#endif

