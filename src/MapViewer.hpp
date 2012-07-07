/*
 * Copyright (C) 2012 Ramil Mintaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_MAP_VIEWER_HPP_
#define WC_MAP_VIEWER_HPP_

#include <boost/function.hpp>
#include <boost/system/error_code.hpp>

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>
#include <Wt/WGoogleMap>
#include <Wt/WPoint>
#include <Wt/WSignal>
#include <Wt/WJavaScript>
#include <Wt/WEvent>
#include <Wt/WRectF>

namespace Wt {

namespace Wc {

/** Map viewer.
It is intended to facilitate an introduction of
a dynamic map in any web page.

Main features:
 - It uses free content (<a href="http://openstreetmap.org/">OpenStreetMap</a>).
   Ajax version is based on <a href="http://openlayers.org">OpenLayers</a> API.
 - The OSM data search is based on
   <a href="http://wiki.openstreetmap.org/wiki/Nominatim">Nominatim</a> API.
 - It supports HTML version.

Additional features:
 - coordinates can be set by a method,
 - map can be moved using buttons,
 - map can be moved using mouse (Ajax only),
 - map can be scaled (zoom in, zoom out),
 - map can be contained markers,
 - time zone can be getting by a method
   (<a href="http://www.geonames.org/about.html">GeoNames</a>).

Usage example:
\code
MapViewer* mv = new MapViewer();
mv->resize(400, 300);
mv->set_center(MapViewer::Coordinate(54.8, 20.25), 9); // Kaliningrad, Russia
mv->set_search_panel();
mv->chosen().connect(this, &Class::to_do);
\endcode

<h3>CSS</h3>

A map has several style classes. The look can be overridden
using the following style class selectors:

\verbatim
.olControlAttribution  :   OSM copyright
.mvSearch              :   The search panel container.
.mvSearchResultNode    :   The search result node container.
\endverbatim
*/
class MapViewer : public Wt::WCompositeWidget {
public:
    /** A geographical coordinate (latitude/longitude) */
    typedef Wt::WGoogleMap::Coordinate Coordinate;

    /** A pair of Coordinate's */
    typedef std::pair<Coordinate, Coordinate> CoordinatePair;

    /** GeoNode.
     It stored coordinates and a description of a node. */
    typedef std::pair<Coordinate, WString> GeoNode;

    /** GeoNodes.
     It is a vector saved a GeoNode set. */
    typedef std::vector<GeoNode> GeoNodes;

    /** Time zone info */
    struct TZ {
        /** Constructor */
        TZ(int t = 0, bool i = false):
            tz(t), is_st(i)
        { }

        int tz; /**< The amount of time in hours to add to UTC
                     to get standard time in this time zone */

        bool is_st; /**< If it is summer time (only for today) */
    };

    /** Constructor */
    MapViewer(Wt::WContainerWidget* parent = 0);

    /** Destructor */
    ~MapViewer();

    /** Sets the map view to the given center */
    void set_center(const Coordinate& center);

    /** Sets the map view to the given center and zoom level */
    void set_center(const Coordinate& center, int zoom);

    /** Allows to pan to a new lonlat.
    If the new lonlat is in the current extent,
    the map will slide smoothly (if implemented js).
    */
    void pan_to(const Coordinate& pos);

    /** Allows to top shift */
    void top_shift(double power = 1.0);

    /** Allows to bottom shift */
    void bottom_shift(double power = 1.0);

    /** Allows to left shift */
    void left_shift(double power = 1.0);

    /** Allows to right shift */
    void right_shift(double power = 1.0);

    /** Increments zoom level by one */
    void zoom_in();

    /** Decrements zoom level by one */
    void zoom_out();

    /** Zoom to a specific zoom level */
    void zoom_to(int level);

    /** Get map marginal coordinates leftTop and rightBottom points.
    \note It must be use only after map center and map zoom defines.
    */
    const CoordinatePair& get_map_marginal_coords() const {
        return marginal_map_coords_;
    }

    /** Tests if the map contains a position.  */
    bool is_map_contained(const Coordinate& pos) const;

    /** Add markers */
    void add_markers(const GeoNodes& ns);

    /** Destroy markers */
    void destroy_markers();

    /** Set marker image url
    Default is http://www.openlayers.org/dev/img/marker.png
    */
    void set_marker_img_url(const std::string& url) {
        marker_img_url_ = url;
    }

    /** The click event
    Event signal emitted when a mouse key was clicked on this widget.
    */
    Signal<Coordinate>& clicked() {
        return clicked_;
    }

    /** The search event.
    Event signal emitted only when a search based on
    the nominatim_search function was completed.
    */
    Signal<GeoNodes>& found();

    /** The nominatim search
    A serching based on the nominatim.
    You will have to use the found() function that takes results.
    */
    void search(const WString& query);

    /** Set a search panel integrated in the map. */
    void set_search_panel(const WString& title = "");

    /** The search choice event.
    Event signal emitted when a found node of the search panel was chosen
    (pushed OK).
    */
    Signal<GeoNode>& chosen() {
        return *chosen_;
    }

    /** The time zone inquiry.
    Return Signal<TZ> event.
    If ajax var is false Wt::Client will be use only for inquiry.
    If the info is invalid, tz will be -13.
    */
    Signal<TZ>& time_zone(const Coordinate& pos,
                          bool ajax = true);

protected:
    /** Layer Constructor.
    A Layer is a data source -- information about how OpenLayers
    should request data and display it.
    */
    void add_osm_layer(const std::string& layer_var_name,
                       const std::string& param = "");

    /** Removes a layer from the map.
    Removes a layer from the map by removing its visual element
    (the layer.div property), then removing it from the map's internal
    list of layers, setting the layer’s map property to null.
    */
    void remove_layer();

    /** Update map viewer */
    void update_impl();

    /** The click event.
    Event signal emitted when a mouse key was clicked on this widget.
    \note When JavaScript is disabled, the signal will never fire.
    */
    JSignal<Coordinate>& jclicked() {
        return jclicked_;
    }

private:
    int zoom_;
    std::string map_name_;
    std::string layer_name_;
    std::string sp_title_;
    CoordinatePair marginal_tile_coords_;
    GeoNode pos_;
    WPoint xy_pos_;
    Signal<Coordinate> clicked_;
    JSignal<Coordinate> jclicked_;
    Signal<GeoNodes>* found_;
    JSignal<std::string>* jfound_;
    Signal<GeoNode>* chosen_;
    JSignal<std::string>* jchosen_;
    Signal<GeoNodes>* html_found_signal_;
    Signal<GeoNodes>* found_signal_;
    JSignal<std::string>* jtz_signal_;
    Signal<TZ>* tz_signal_;
    //
    mutable bool markers_;
    bool smp_;
    bool html_search_panel_;
    bool enable_updates_;
    //
    WPushButton* sr_button_;
    WContainerWidget* sr_cw_;
    //
    Http::Client* http_;
    Http::Client* tz_http_;
    GeoNodes sp_fns_;
    GeoNodes marker_nodes_;
    CoordinatePair marginal_map_coords_;
    std::pair<int, int> tile_lt_;
    std::pair<double, double> to_px_;
    std::string marker_img_url_;

    void destroy_map();

    Wt::WContainerWidget* get_impl();

    WContainerWidget* get_html_map();
    void html_markers_view(WContainerWidget* cw);
    WContainerWidget* get_html_osm_attribution();
    WContainerWidget* get_html_control_panel();
    void html_v(WContainerWidget* cw);

    const std::string get_smp_jsc() const;
    const std::string get_search_js_action() const;
    const GeoNode found_node_parser(const std::string& data) const;
    void nominatim_data_parser(const std::string& data);
    void nominatim_data_parser(const boost::system::error_code& e,
                               const Http::Message& response);
    void choice_data_parser(const std::string data);
    void tz_data_parser(const std::string& data);
    void tz_data_parser(const boost::system::error_code& e,
                        const Http::Message& response);
    const std::string cipher(const std::string& str);
    const GeoNodes
    http_request_parser(const boost::system::error_code& e,
                        const Http::Message& response);
    WContainerWidget* html_search_panel();
    void panel_html_search(WLineEdit* edit);
    void html_search_present(const GeoNodes& ns);
    void html_searh_chosen();
    void simple_refresh();
    void set_html_result_visible(bool enable = true);
    void smp_calc(const GeoNodes& ns);
    WRectF tauten(const GeoNodes& ns);
    void click_node(const GeoNode& node, int num);

    const std::string adding_marker_jsc(const Coordinate& pos);
    void ch_markers_size(int num = -1);

    const WPoint w2t(const Coordinate& pos, int zoom) const;
    const Coordinate t2w(const WPoint& pos, int zoom) const;
    const WPoint w2px(const Coordinate& pos) const;

    void map_param_calc();
    const CoordinatePair marginal_pic_coords(const WPoint& tile) const;
    const CoordinatePair marginal_pic_coords(const Coordinate& pos) const;

    template <class Type> Type get_abs(Type val) const;
    double diff_between(double x, double y) const;
    Side get_side(int v) const;
    double coord_control(double val, std::string dir = "lon") const;
    double mod(double x, double y) const;
    std::pair<double, double> tile_size();

    bool js() const;
    const std::string store_jsv(const std::string& key,
                                const std::string& value) const;
    const std::string get_stored_jsv(const std::string& key) const;
    const std::string get_lonlat_jsc(const std::string& lat,
                                     const std::string& lon) const;
    const std::string get_lonlat_jsc(const Coordinate& pos) const;
    void click_on(const WPoint& tile_xy,
                  const WMouseEvent::Coordinates& img_xy);
    void click_on(const Coordinate& pos);

    void set_click_signal_();
    const std::string set_ajax_action(const std::string& url,
                                      const std::string& js_action) const;
    JSignal<std::string>& jfound() {
        return *jfound_;
    }
    void pan_to(const GeoNode& node);

    /** Returns a JavaScript call that triggers the signal. */
    std::string set_js_listener_control_(const JSignal<Coordinate> &signal,
                                         const std::string& signal_name) const;

    void search(const WString& query, Signal<GeoNodes>*);
};

}

}

#endif

