/*
 * Copyright (C) 2011 Ramil Mintaev
 *
 * See the LICENSE.MapViewer file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WDoubleValidator>
#include <Wt/WGridLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WIntValidator>
#include <Wt/WLabel>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WText>
#include <Wt/WVBoxLayout>
#include <Wt/WScrollArea>
#include <Wt/Wc/MapViewer.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

typedef MapViewer::Coordinate Coordinate;

class Example : public Wt::WContainerWidget {
public:
    Example(WContainerWidget* p = 0);

private:
    void set_zoom_to();
    void set_pan_to();
    void get_pos(const Coordinate& pos);
    void left_shift();
    void right_shift();
    void top_shift();
    void bottom_shift();
    void get_search(MapViewer::GeoNode node);
    void search_pr(WLineEdit* edit);
    void search_presenting(WContainerWidget* cw,
                           const MapViewer::GeoNodes& nodes);
    void get_time_zone(const MapViewer::TZ& tz);

    MapViewer* mv_;
    WLineEdit* edit_of_zoom_to_;
    WLineEdit* edit_of_pan_to_lng_;
    WLineEdit* edit_of_pan_to_lat_;
    WText* click_pos_;
    WText* click_search_;
    WText* time_zone_;
};

Example::Example(WContainerWidget* p):
    WContainerWidget(p) {
    WContainerWidget* cw = new WContainerWidget(this);
    WHBoxLayout* hl = new WHBoxLayout();
    mv_ = new MapViewer();
    mv_->resize(400, 300);
    mv_->set_center(Coordinate(54.8, 20.25), 9);
    mv_->clicked().connect(this, &Example::get_pos);
    hl->addWidget(mv_, 0, AlignTop);
    //
    WPushButton* to_left = new Wt::WPushButton("<");
    WPushButton* to_right = new Wt::WPushButton(">");
    to_left->clicked().connect(this, &Example::left_shift);
    to_right->clicked().connect(this, &Example::right_shift);
    WPushButton* to_top = new Wt::WPushButton("^");
    WPushButton* to_bottom = new Wt::WPushButton("v");
    to_top->clicked().connect(this, &Example::top_shift);
    to_bottom->clicked().connect(this, &Example::bottom_shift);
    //
    WPushButton* zoom_in = new Wt::WPushButton("zoom In");
    WPushButton* zoom_out = new Wt::WPushButton("zoom Out");
    zoom_in->clicked().connect(mv_, &MapViewer::zoom_in);
    zoom_out->clicked().connect(mv_, &MapViewer::zoom_out);
    //
    WContainerWidget* cw_of_zoom_to = new Wt::WContainerWidget();
    WLabel* label_of_zoom_to = new Wt::WLabel("set zoom:", cw_of_zoom_to);
    edit_of_zoom_to_ = new WLineEdit("9", cw_of_zoom_to);
    edit_of_zoom_to_->resize(30, WLength());
    label_of_zoom_to->setBuddy(edit_of_zoom_to_);
    edit_of_zoom_to_->setValidator(new WIntValidator(1, 17));
    label_of_zoom_to->setBuddy(edit_of_zoom_to_);
    WHBoxLayout* menu_hl_of_zoom_to = new WHBoxLayout();
    menu_hl_of_zoom_to->addWidget(cw_of_zoom_to, 0, AlignJustify);
    WContainerWidget* zoom_to_b_cw = new WContainerWidget();
    WPushButton* button_of_zoom_to = new Wt::WPushButton("OK", zoom_to_b_cw);
    button_of_zoom_to->clicked().connect(this, &Example::set_zoom_to);
    menu_hl_of_zoom_to->addWidget(zoom_to_b_cw, AlignJustify);
    //
    WContainerWidget* cw_of_pan_to_lng = new Wt::WContainerWidget();
    WContainerWidget* cw_of_pan_to_lat = new Wt::WContainerWidget();
    WLabel* label_of_pan_to_lng = new Wt::WLabel("set lng:", cw_of_pan_to_lng);
    WLabel* label_of_pan_to_lat = new Wt::WLabel("set lat:", cw_of_pan_to_lat);
    edit_of_pan_to_lng_ = new WLineEdit("20.4", cw_of_pan_to_lng);
    edit_of_pan_to_lng_->resize(50, WLength());
    label_of_pan_to_lng->setBuddy(edit_of_pan_to_lng_);
    edit_of_pan_to_lng_->setValidator(new WDoubleValidator(-180.0, 180.0));
    label_of_pan_to_lng->setBuddy(edit_of_pan_to_lng_);
    edit_of_pan_to_lat_ = new WLineEdit("54.7167", cw_of_pan_to_lat);
    edit_of_pan_to_lat_->resize(50, WLength());
    label_of_pan_to_lat->setBuddy(edit_of_pan_to_lat_);
    edit_of_pan_to_lat_->setValidator(new WDoubleValidator(-90.0, 90.0));
    label_of_pan_to_lat->setBuddy(edit_of_pan_to_lat_);
    WHBoxLayout* menu_hl_of_pan_to = new WHBoxLayout();
    menu_hl_of_pan_to->addWidget(cw_of_pan_to_lng, 0, AlignTop);
    menu_hl_of_pan_to->addWidget(cw_of_pan_to_lat, 0, AlignTop);
    WContainerWidget* pan_to_b_cw = new WContainerWidget();
    WPushButton* button_of_pan_to = new Wt::WPushButton("OK", pan_to_b_cw);
    button_of_pan_to->clicked().connect(this, &Example::set_pan_to);
    menu_hl_of_pan_to->addWidget(pan_to_b_cw, AlignTop);
    //
    click_pos_ = new WText();
    click_search_ = new WText();
    time_zone_ = new WText();
    //
    WVBoxLayout* vl_menu = new WVBoxLayout();
    WHBoxLayout* hl_menu_zooms = new WHBoxLayout();
    hl_menu_zooms->addWidget(zoom_in, 0, AlignTop);
    hl_menu_zooms->addWidget(zoom_out, 0, AlignTop);
    WGridLayout* shift_hl_menu = new WGridLayout();
    shift_hl_menu->addWidget(to_top, 0, 1, AlignTop);
    shift_hl_menu->addWidget(to_left, 1, 0, AlignTop);
    shift_hl_menu->addWidget(to_right, 1, 2, AlignTop);
    shift_hl_menu->addWidget(to_bottom, 2, 1, AlignTop);
    //
    WVBoxLayout* s_vl = new WVBoxLayout();
    WHBoxLayout* s_hl = new WHBoxLayout();
    WContainerWidget* s_cw = new WContainerWidget();
    s_cw->resize(220, 300);
    s_cw->setHidden(true);
    wApp->styleSheet().addRule(".mvSearchNode", "background-color:#e1d0d0;");
    wApp->styleSheet().addRule(".mvSearchText", "font-size:12px;");
    WContainerWidget* s_e_cw = new WContainerWidget();
    WLineEdit* s_edit = new WLineEdit("searsh", s_e_cw);
    s_edit->enterPressed().connect(boost::bind(&Example::search_pr, this,
                                   s_edit));
    WContainerWidget* s_b_cw = new WContainerWidget();
    s_b_cw->setContentAlignment(AlignLeft);
    WPushButton* s_button = new WPushButton("OK", s_b_cw);
    s_button->clicked().connect(boost::bind(&Example::search_pr, this,
                                            s_edit));
    s_hl->addWidget(s_e_cw, 0, AlignRight);
    s_hl->addWidget(s_b_cw, 0, AlignLeft);
    s_vl->addLayout(s_hl, 0, AlignTop);
    s_vl->addWidget(s_cw, 0, AlignTop);
    //
    mv_->found().connect(boost::bind(&Example::search_presenting,
                                     this, s_cw, _1));
    //
    vl_menu->addLayout(shift_hl_menu, 0, AlignTop);
    vl_menu->addLayout(hl_menu_zooms, 0, AlignTop);
    vl_menu->addLayout(menu_hl_of_zoom_to, 0, AlignJustify);
    vl_menu->addLayout(menu_hl_of_pan_to, 0, AlignTop);
    vl_menu->addWidget(click_pos_, 0, AlignTop);
    vl_menu->addWidget(click_search_, 0, AlignTop);
    vl_menu->addWidget(time_zone_, 0, AlignTop);
    hl->addLayout(vl_menu, 0, AlignTop);
    hl->addLayout(s_vl, 0, AlignTop);
    cw->setLayout(hl, AlignTop);
    mv_->set_search_panel();
    mv_->chosen().connect(this, &Example::get_search);
    MapViewer::GeoNodes marker_nodes;
    marker_nodes.push_back(std::make_pair(Coordinate(55.0151, 20.6122),
                                          ""));
    marker_nodes.push_back(std::make_pair(Coordinate(54.8, 20.25),
                                          ""));
    mv_->add_markers(marker_nodes);
}

void Example::set_zoom_to() {
    mv_->zoom_to(boost::lexical_cast<int>(edit_of_zoom_to_->text()));
}

void Example::set_pan_to() {
    double x = boost::lexical_cast<double>(edit_of_pan_to_lat_->text());
    double y = boost::lexical_cast<double>(edit_of_pan_to_lng_->text());
    mv_->pan_to(Coordinate(x, y));
}

void Example::left_shift() {
    mv_->left_shift(0.5);
}
void Example::right_shift() {
    mv_->right_shift(0.5);
}
void Example::top_shift() {
    mv_->top_shift(0.5);
}
void Example::bottom_shift() {
    mv_->bottom_shift(0.5);
}

void Example::get_pos(const Coordinate& pos) {
    click_pos_->setText("You clicked near: " +
                        str(boost::format("%.4f %.4f") % pos.longitude() % pos.latitude()));
    mv_->time_zone(pos).connect(this, &Example::get_time_zone);
}

void Example::get_search(MapViewer::GeoNode node) {
    Coordinate& pos = node.first;
    click_search_->setText("You have chosen: " + node.second + " <br />Position: " +
                           str(boost::format("%.4f %.4f") % pos.longitude() % pos.latitude()));
    mv_->time_zone(pos).connect(this, &Example::get_time_zone);
}

void Example::search_presenting(WContainerWidget* cw,
                                const MapViewer::GeoNodes& nodes) {
    WContainerWidget* cw_n = new WContainerWidget();
    WVBoxLayout* vl = new WVBoxLayout();
    BOOST_FOREACH (MapViewer::GeoNode node, nodes) {
        Coordinate& pos = node.first;
        vl->addWidget(new WText(node.second + "<br />" +
                                str(boost::format("%.4f %.4f") % pos.longitude() % pos.latitude())),
                      AlignTop);
    }
    cw->clear();
    cw_n->setLayout(vl, AlignTop);
    WScrollArea* scroll = new WScrollArea();
    scroll->setWidget(cw_n);
    scroll->resize(cw->width(), cw->height());
    scroll->setScrollBarPolicy(WScrollArea::ScrollBarAsNeeded);
    cw->addWidget(scroll);
    cw->animateShow(WAnimation(Wt::WAnimation::Fade));
}

void Example::search_pr(WLineEdit* edit) {
    mv_->search(edit->text());
}

void Example::get_time_zone(const MapViewer::TZ& tz) {
    std::string str = "Time zone: " + TO_S(tz.tz) + "<br />to Summer Time? ";
    if (tz.is_st) {
        str += "YES";
    } else {
        str += "NO";
    }
    time_zone_->setText(str);
}

class MapViewerApp : public WApplication {
public:
    MapViewerApp(const WEnvironment& env):
        WApplication(env) {
        new Example(root());
    }
};

WApplication* createMapViewerApp(const WEnvironment& env) {
    return new MapViewerApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createMapViewerApp);
}

