/*
 * Copyright (C) 2011 Ramil Mintaev
 *
 * See the LICENSE.MapViewer file for terms of use.
 */

#include <Wt/Wc/util.hpp>
#include <Wt/Wc/MapImage.hpp>

#include <math.h>
#include <boost/math/constants/constants.hpp>
#include <boost/bind.hpp>

#include <Wt/WLength>
#include <Wt/WString>
#include <Wt/WEnvironment>
#include <Wt/WPoint>
#include <Wt/WGridLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WImage>
#include <Wt/WCssStyleSheet>

#include "MapViewer.hpp"
#include "Application.hpp"

using namespace Wt;
using namespace Wt::Wc;

const double pi = boost::math::constants::pi<double>();

namespace OMV {

MapViewer::MapViewer(Wt::WContainerWidget* p):
    Wt::WCompositeWidget(p), zoom_(1), center_(Coordinate(0, 0)),
    jclicked_(this, "click") {
    wApp->require("http://openlayers.org/api/OpenLayers.js", "OpenLayers");
    setImplementation(new Wt::WContainerWidget());
    map_name_ = "map_" + get_impl()->id();
    layer_name_ = map_name_ + "_layer";
    update_impl();
}

MapViewer::~MapViewer() {
    destroy_map();
}

void MapViewer::update_impl() {
    setLayoutSizeAware(true);
    if (!implementation()) {
        setImplementation(new WContainerWidget());
    }
    get_impl()->clear();
    if (js()) {
        std::string map_created_str;
        map_created_str = "new OpenLayers.Map('"
                          + get_impl()->id() + "')";
        jclicked().connect(this, &MapViewer::jclick_on);
        doJavaScript(store_jsv(map_name_, map_created_str)); // add a map.
        add_osm_layer(layer_name_);
        set_click_signal_();
        wApp->styleSheet().addRule(".olControlAttribution",
                                   "position:absolute !important;"
                                   "bottom:0 !important;right:0 !important;");
    } else {
        views_map_in_html();
    }
}

void MapViewer::add_osm_layer(const std::string& layer_var_name,
                              const std::string& param) {
    if (js()) {
        std::stringstream strm;
        strm << store_jsv(layer_var_name,
                          "new OpenLayers.Layer.OSM(" + param + ")")
             << get_stored_jsv(map_name_) << ".addLayer("
             << get_stored_jsv(layer_var_name) << ");";
        doJavaScript(strm.str());
    }
}

void MapViewer::set_center(const Coordinate& center) {
    center_ = center;
    if (js()) {
        std::stringstream strm;
        strm << store_jsv("lonLatOf_" + map_name_, get_lonlat_jsc(center))
             << get_stored_jsv(map_name_) << ".setCenter("
             << get_stored_jsv("lonLatOf_" + map_name_) << ");";
        doJavaScript(strm.str());
    } else {
        views_map_in_html();
    }
}

void MapViewer::set_center(const Coordinate& center, int zoom) {
    zoom_ = zoom;
    center_ = center;
    marginal_tile_coords_ = marginal_pic_coords(center_);
    if (js()) {
        std::stringstream strm;
        strm << store_jsv("lonLatOf_" + map_name_, get_lonlat_jsc(center))
             << store_jsv("zoomOf_" + map_name_, TO_S(zoom_))
             << get_stored_jsv(map_name_) << ".setCenter("
             << get_stored_jsv("lonLatOf_" + map_name_)
             << ", " << get_stored_jsv("zoomOf_" + map_name_) + ");";
        doJavaScript(strm.str());
    } else {
        views_map_in_html();
    }
}

bool MapViewer::js() const {
    return wApp->environment().javaScript();
}

void MapViewer::pan_to(const Coordinate& center) {
    center_ = center;
    if (js()) {
        std::stringstream strm;
        strm << store_jsv("pan_to_lonLatOf_" + map_name_,
                          get_lonlat_jsc(center))
             << get_stored_jsv(map_name_) << ".panTo("
             << get_stored_jsv("pan_to_lonLatOf_" + map_name_) << ");";
        doJavaScript(strm.str());
    } else {
        marginal_tile_coords_ = marginal_pic_coords(center_);
        views_map_in_html();
    }
}

void MapViewer::left_shift(double power) {
    double w = diff_between(marginal_tile_coords_.second.longitude(),
                            marginal_tile_coords_.first.longitude());
    pan_to(Coordinate(center_.latitude(), center_.longitude() - w * power));
}

void MapViewer::right_shift(double power) {
    double w = diff_between(marginal_tile_coords_.second.longitude(),
                            marginal_tile_coords_.first.longitude());
    pan_to(Coordinate(center_.latitude(), center_.longitude() + w * power));
}

void MapViewer::top_shift(double power) {
    double h = diff_between(marginal_tile_coords_.second.latitude(),
                            marginal_tile_coords_.first.latitude());
    pan_to(Coordinate(center_.latitude() + h * power, center_.longitude()));
}

void MapViewer::bottom_shift(double power) {
    double h = diff_between(marginal_tile_coords_.second.latitude(),
                            marginal_tile_coords_.first.latitude());
    pan_to(Coordinate(center_.latitude() - h * power, center_.longitude()));
}

void MapViewer::zoom_in() {
    if (zoom_ < 17) {
        zoom_++;
        marginal_tile_coords_ = marginal_pic_coords(center_);
    }
    if (js()) {
        doJavaScript(get_stored_jsv(map_name_) + ".zoomIn();");
    } else {
        views_map_in_html();
    }
}

void MapViewer::zoom_out() {
    if (zoom_ > 1) {
        zoom_--;
        marginal_tile_coords_ = marginal_pic_coords(center_);
    }
    if (js()) {
        doJavaScript(get_stored_jsv(map_name_) + ".zoomOut();");
    } else {
        views_map_in_html();
    }
}

void MapViewer::zoom_to(int level) {
    if (level > 0 && level < 18) {
        zoom_ = level;
        marginal_tile_coords_ = marginal_pic_coords(center_);
    }
    if (js()) {
        doJavaScript(get_stored_jsv(map_name_) +
                     ".zoomTo(" + TO_S(level) + ");");
    } else {
        views_map_in_html();
    }
}

void MapViewer::remove_layer() {
    doJavaScript(get_stored_jsv(map_name_) + ".removeLayer("
                 + get_stored_jsv(layer_name_) + ", false);");
}

void MapViewer::destroy_map() {
    if (js()) {
        doJavaScript(get_stored_jsv(map_name_) + ".destroy();");
    }
}

WPoint MapViewer::w2t(const Coordinate& pos, int zoom) const {
    // World to tile position.
    int x = (int)(floor((pos.longitude() + 180.0) / 360.0 * pow(2.0, zoom)));
    double lat_rad = pos.latitude() *  pi / 180.0;
    int y = (int)(floor((1.0 - log(tan(lat_rad) + 1.0 / cos(lat_rad)) / pi) /
                        2.0 * pow(2.0, zoom)));
    return WPoint(x, y);
}

MapViewer::Coordinate MapViewer::t2w(const WPoint& pos, int zoom) const {
    // Tile to World position.
    double lng = pos.x() / pow(2.0, zoom) * 360.0 - 180;
    double n = pi - 2.0 * pi * pos.y() / pow(2.0, zoom);
    double lat = 180.0 / pi * atan(0.5 * (exp(n) - exp(-n)));
    return MapViewer::Coordinate(lat, lng);
}

MapViewer::CoordinatePair
MapViewer::marginal_pic_coords(const WPoint& tile) const {
    return std::make_pair(t2w(tile, zoom_),
                          t2w(WPoint(tile.x() + 1, tile.y() + 1), zoom_));
}

MapViewer::CoordinatePair
MapViewer::marginal_pic_coords(const MapViewer::Coordinate& pos) const {
    return marginal_pic_coords(w2t(pos, zoom_));
}

std::pair<int, int> MapViewer::tile_coord2tile_left_top(const Coordinate& pos) {
    double wp = diff_between(marginal_tile_coords_.second.longitude(),
                             marginal_tile_coords_.first.longitude());
    double lp = diff_between(marginal_tile_coords_.second.latitude(),
                             marginal_tile_coords_.first.latitude());
    int left = (int)(round(diff_between(pos.longitude(),
                                        marginal_tile_coords_.first.longitude())
                           / wp * 256.0));
    int top = (int)(round(diff_between(pos.latitude(),
                                       marginal_tile_coords_.first.latitude()) /
                          lp * 256.0));
    return std::make_pair(left, top);
}

double MapViewer::diff_between(double x, double y) {
    if ((x >= 0 && y >= 0) || (x <= 0 && y <= 0)) {
        return get_abs(x - y);
    } else {
        if (x >= 0 && y < 0) {
            return x - y;
        } else {
            return get_abs(y - x);
        }
    }
    return 0;
}

void MapViewer::views_map_in_html() {
    std::pair<int, int> lt = tile_coord2tile_left_top(center_);
    std::vector<int>  rmns;
    int half_width = get_impl()->width().value() / 2;
    int half_height = get_impl()->height().value() / 2;
    int to_left_margin =  half_width - lt.first;
    rmns.push_back(to_left_margin);
    int to_top_margin =  half_height - lt.second;
    int to_bottom_margin =  half_height * 2 - to_top_margin;
    rmns.push_back(to_bottom_margin);
    int to_right_margin =  half_width * 2 - to_left_margin;
    rmns.push_back(to_right_margin);
    rmns.push_back(to_top_margin);
    //
    std::vector<int> crops;
    int column = 0;
    int row = 0;
    xy_center_ = w2t(center_, zoom_);
    int X = xy_center_.x();
    int y = xy_center_.y();
    for (int i = 0; i < 4; i++) {
        double invm = rmns[i] / 256.0;
        int cr = invm > 1.0 ? (int)(256 - 256 * (invm - (int)invm)) :
                 (int)(256 * (invm > 0 ? (1 - invm) : invm));
        int v = (int)invm;
        v += cr > 0 ? 1 : 0;
        if (cr < 0) {
            cr *= -1;
        }
        crops.push_back(cr);
        if (i % 2 == 0) {
            column += v;
        } else {
            row += v;
        }
        if (i == 0) {
            X -= v;
        }
        if (i == 3) {
            y -= v;
        }
    }
    //
    int vori;
    int hori;
    int x;
    std::string xstd;
    std::string ystd;
    std::vector<int> img_margin(4, 0);
    get_impl()->clear();
    WContainerWidget* gcw = new WContainerWidget();
    wApp->styleSheet().addRule(".mapContainer",
                               "position:relative;top:-110px;");
    gcw->addStyleClass("mapContainer");
    WGridLayout* gl = new WGridLayout();
    gl->setHorizontalSpacing(0);
    gl->setVerticalSpacing(0);
    gl->setContentsMargins(0, 0, 0, 0);
    for (int i = 0; i < row; i++) {
        ystd = TO_S(y);
        vori = -1;
        int cw_h = 256;
        img_margin[3] = 0;
        img_margin[1] = 0;
        if (i == 0) {
            vori = 3;
            cw_h = 256 - crops[3] + (row < 2 ? 256 - crops[1] : 0);
            img_margin[3] = -crops[3];
            img_margin[1] = row < 2 ? -crops[1] : 0;
        } else if (i == row - 1) {
            vori = 1;
            cw_h = 256 - crops[1];
            img_margin[1] = -crops[1];
        }
        x = X;
        for (int j = 0; j < column; j++) {
            xstd = TO_S(x);
            WContainerWidget* cw = new WContainerWidget();
            WImage* img = new WImage(WLink(WLink::Url,
                                           "http://a.tile.openstreetmap.org/" +
                                           TO_S(zoom_) +
                                           "/" + xstd + "/" + ystd + ".png"));
            hori = -1;
            int cw_w = 256;
            img_margin[0] = 0;
            img_margin[2] = 0;
            if (j == 0) {
                hori = 0;
                cw_w = 256 - crops[0] + (column < 2 ? 256 - crops[2] : 0);
                img_margin[0] = -crops[0];
                img_margin[2] = column < 2 ? -crops[2] : 0;
            } else if (j == column - 1) {
                hori = 2;
                cw_w = 256 - crops[2];
                img_margin[2] = -crops[2];
            }
            cw->resize(cw_w, cw_h);
            if (vori != -1 || hori != -1) {
                cw->setOverflow(WContainerWidget::OverflowHidden);
                //
                for (int k = 0; k < 4; k++) {
                    img->setMargin(img_margin[k], get_side(k));
                }
            }
            MapImage* map_img = new MapImage(img, cw);
            map_img->clicked().connect(boost::bind(&MapViewer::click_on,
                                                   this, WPoint(x, y), _1));
            gl->addWidget(cw, i, j);
            x++;
        }
        y++;
    }
    gcw->setLayout(gl);
    get_impl()->setContentAlignment(AlignTop);
    get_impl()->addWidget(get_html_control_panel());
    get_impl()->addWidget(gcw);
}

WContainerWidget* MapViewer::get_html_control_panel() {
    WContainerWidget* cw = new WContainerWidget(get_impl());
    WVBoxLayout* vl = new WVBoxLayout(cw);
    vl->setSpacing(0);
    vl->setContentsMargins(0, 0, 0, 0);
    WContainerWidget* north_cw = new WContainerWidget();
    north_cw->resize(WLength(), 18);
    north_cw->setContentAlignment(AlignCenter);
    WImage* img_north = new WImage("http://openlayers.org/api/img/"
                                   "north-mini.png", north_cw);
    img_north->clicked().connect(boost::bind(&MapViewer::top_shift,
                                 this, 0.34));
    vl->addWidget(north_cw);
    WContainerWidget* west_east_cw = new WContainerWidget();
    west_east_cw->resize(WLength(), 18);
    west_east_cw->setContentAlignment(AlignCenter);
    WImage* img_west = new WImage("http://openlayers.org/api/img/west-mini.png",
                                  west_east_cw);
    img_west->clicked().connect(boost::bind(&MapViewer::left_shift,
                                            this, 0.34));
    WImage* img_east = new WImage("http://openlayers.org/api/img/east-mini.png",
                                  west_east_cw);
    img_east->clicked().connect(boost::bind(&MapViewer::right_shift,
                                            this, 0.34));
    vl->addWidget(west_east_cw);
    WContainerWidget* south_cw = new WContainerWidget();
    south_cw->resize(WLength(), 18);
    south_cw->setContentAlignment(AlignCenter);
    WImage* img_south = new WImage("http://openlayers.org/api/img/"
                                   "south-mini.png", south_cw);
    img_south->clicked().connect(boost::bind(&MapViewer::bottom_shift,
                                 this, 0.34));
    vl->addWidget(south_cw);
    vl->addWidget(new WBreak());
    WContainerWidget* zoom_plus_cw = new WContainerWidget();
    zoom_plus_cw->resize(WLength(), 18);
    zoom_plus_cw->setContentAlignment(AlignCenter);
    WImage* img_zoom_plus = new WImage("http://openlayers.org/api/img/"
                                       "zoom-plus-mini.png", zoom_plus_cw);
    img_zoom_plus->clicked().connect(boost::bind(&MapViewer::zoom_in, this));
    vl->addWidget(zoom_plus_cw);
    WContainerWidget* zoom_minus_cw = new WContainerWidget();
    zoom_minus_cw->resize(WLength(), 18);
    zoom_minus_cw->setContentAlignment(AlignCenter);
    WImage* img_zoom_minus = new WImage("http://openlayers.org/api/img/"
                                        "zoom-minus-mini.png", zoom_minus_cw);
    img_zoom_minus->clicked().connect(boost::bind(&MapViewer::zoom_out, this));
    vl->addWidget(zoom_minus_cw);
    wApp->styleSheet().addRule(".menuControlPanel", "position:relative;"
                               "width:60px;top:8px;left:0px;z-index:1004;");
    cw->addStyleClass("menuControlPanel");
    return cw;
}

Side MapViewer::get_side(int v) {
    if (v == 0) {
        return Left;
    } else if (v == 1) {
        return Bottom;
    } else if (v == 2) {
        return Right;
    } else if (v == 3) {
        return Top;
    }
    return Left;
}

template <class Type>
Type MapViewer::get_abs(Type val) {
    return val < 0 ? -val : val;
}

std::string MapViewer::store_jsv(const std::string& key,
                                 const std::string& value) const {
    return "$(" + jsRef() + ").data('" + key + "', " + value + ");";
}

std::string MapViewer::get_stored_jsv(const std::string& key) const {
    return "$(" + jsRef() + ").data('" + key + "')";
}

std::string MapViewer::get_lonlat_jsc(const Coordinate& pos) const {
    std::stringstream strm;
    strm << "new OpenLayers.LonLat(" << pos.longitude()
         << ", " << pos.latitude() << ")"
         << ".transform(new OpenLayers.Projection('EPSG:4326'),"
         << get_stored_jsv(map_name_) + ".getProjectionObject())";
    return strm.str();
}

Wt::WContainerWidget* MapViewer::get_impl() {
    return DOWNCAST<Wt::WContainerWidget*>(implementation());
}

std::string MapViewer::set_js_listener_control_(
    const JSignal<Coordinate> &signal,
    const std::string& signal_name) const {
    std::stringstream strm;
    strm << "OpenLayers.Control.Click = OpenLayers.Class(OpenLayers.Control, {"
         << "defaultHandlerOptions: {'single': true, 'double': false,"
         << "'pixelTolerance': 0, 'stopSingle': false, 'stopDouble': false},"
         <<      "initialize: function(options) {"
         << "this.handlerOptions = OpenLayers.Util.extend("
         << "{}, this.defaultHandlerOptions);"
         << "OpenLayers.Control.prototype.initialize.apply(this, arguments);"
         << "this.handler = new OpenLayers.Handler.Click(this, {"
         << "'" + signal_name + "': this.trigger}, this.handlerOptions);},"
         <<      "trigger: function(e) {"
         << "var map = " + get_stored_jsv(map_name_) + ";"
         << "var lonlat = map.getLonLatFromViewPortPx(e.xy)"
         << ".transform(map.getProjectionObject(), "
         "new OpenLayers.Projection('EPSG:4326'));"
         << signal.createCall("lonlat.lat +' ' + lonlat.lon")
         << ";}});";
    return strm.str();
}

void MapViewer::set_click_signal_() {
    std::stringstream strm;
    strm << set_js_listener_control_(jclicked_, "click")
         << store_jsv("click1", "new OpenLayers.Control.Click()")
         << get_stored_jsv(map_name_) << ".addControl("
         << get_stored_jsv("click1") << ");"
         << get_stored_jsv("click1") << ".activate();";
    doJavaScript(strm.str());
}

void MapViewer::jclick_on(const Coordinate& pos) {
    clicked_.emit(Coordinate(pos.latitude(), pos.longitude()));
}

void MapViewer::click_on(const WPoint& tile_xy,
                         const WMouseEvent::Coordinates& img_xy) {
    double wp = diff_between(marginal_tile_coords_.second.longitude(),
                             marginal_tile_coords_.first.longitude());
    double lp = diff_between(marginal_tile_coords_.second.latitude(),
                             marginal_tile_coords_.first.latitude());
    Coordinate lt_coord = t2w(tile_xy, zoom_);
    double lng = lt_coord.longitude() + img_xy.x / 256.0 * wp;
    double lat = lt_coord.latitude() - img_xy.y / 256.0 * lp;
    clicked_.emit(Coordinate(lat, lng));
}

}

