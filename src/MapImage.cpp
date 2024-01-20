/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"
#include "global.hpp"

#include <boost/bind.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WAnchor>
#include <Wt/WImage>

#ifndef WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION
// FIXME nasty public morozov
#define private friend class Wt::Wc::MapImage; private
#include <Wt/WCompositeWidget>
#undef private
#define implementation() Wt::WCompositeWidget::impl_
#endif // WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION

#include "MapImage.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

class MapResource : public WResource {
public:
    MapResource(const std::string& redirect_to, MapImage* map_image,
                WObject* parent = 0):
        WResource(parent),
        redirect_to_(redirect_to),
        map_image_(map_image),
        app_(wApp)
    { }

    ~MapResource() {
        beingDeleted();
    }

    void handleRequest(const Wt::Http::Request& request,
                       Wt::Http::Response& response) {
        WApplication::UpdateLock app_lock = app_->getUpdateLock();
        WMouseEvent::Coordinates xy(-1, -1);
        const std::string* xy_str = request.getParameter("MapImageXY");
        if (xy_str) {
            int comma_pos = xy_str->find(',');
            if (comma_pos != std::string::npos) {
                std::string x_str = xy_str->substr(1, comma_pos - 1);
                std::string y_str = xy_str->substr(comma_pos + 1);
                try {
                    int x = boost::lexical_cast<int>(x_str);
                    int y = boost::lexical_cast<int>(y_str);
                    xy = WMouseEvent::Coordinates(x, y);
                } catch (...) {
                }
            }
        }
        map_image_->clicked().emit(xy);
        response.setMimeType("text/html");
        response.out() << "<html><head>";
        response.out() << "<meta http-equiv='refresh' ";
        response.out() << " content='0; url=" << redirect_to_ << "' />";
        response.out() << "</head><body></body></html>";
    }

private:
    std::string redirect_to_;
    MapImage* map_image_;
    WApplication* app_;
};

void emit_clicked(MapImage::ClickSignal& signal, const WMouseEvent& event) {
    signal.emit(event.widget());
}

MapImage::MapImage(WImage* image, WContainerWidget* parent):
    WCompositeWidget(parent) {
    if (wApp->environment().ajax()) {
        setImplementation(image);
        image->clicked().connect(boost::bind(emit_clicked, boost::ref(clicked_), _1));
    } else {
        std::string url = wApp->bookmarkUrl();
        char join = url.find('?') == std::string::npos ? '?' : '&';
        url += join;
        url += "wtd=" + wApp->sessionId();
        MapResource* resource = new MapResource(url, this, this);
        image->setAttributeValue("ismap", "ismap");
        setImplementation(new WAnchor(resource->url() + "&MapImageXY=", image));
    }
}

WImage* MapImage::image() {
    if (wApp->environment().ajax()) {
        return DOWNCAST<WImage*>(implementation());
    } else {
        return DOWNCAST<WAnchor*>(implementation())->image();
    }
}

}

}

