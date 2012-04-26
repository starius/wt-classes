/*
 * Copyright (C) 2011 Ramil Mintaev
 *
 * See the LICENSE.MapViewer file for terms of use.
 */

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

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
#include <Wt/Wc/MapViewer.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

class Example : public Wt::WContainerWidget {
public:
    Example(WContainerWidget* p = 0);

private:
    void set_zoom_to();
    void set_pan_to();
    void get_pos(const MapViewer::Coordinate& pos);
    void left_shift();
    void right_shift();
    void top_shift();
    void bottom_shift();

    MapViewer* mv_;
    WLineEdit* edit_of_zoom_to_;
    WLineEdit* edit_of_pan_to_lng_;
    WLineEdit* edit_of_pan_to_lat_;
    WText* click_pos_;
};

Example::Example(WContainerWidget* p):
    WContainerWidget(p) {
    WContainerWidget* cw = new WContainerWidget(this);
    WHBoxLayout* hl = new WHBoxLayout();
    mv_ = new MapViewer();
    mv_->resize(400, 300);
    mv_->set_center(MapViewer::Coordinate(54.8, 20.25), 9);
    mv_->clicked().connect(this, &Example::get_pos);
    hl->addWidget(mv_, AlignTop);
    //
    WPushButton* to_left = new Wt::WPushButton("<");
    to_left->resize(80, 10);
    WPushButton* to_right = new Wt::WPushButton(">");
    to_right->resize(80, 10);
    to_left->clicked().connect(this, &Example::left_shift);
    to_right->clicked().connect(this, &Example::right_shift);
    WPushButton* to_top = new Wt::WPushButton("^");
    to_top->resize(80, 10);
    WPushButton* to_bottom = new Wt::WPushButton("v");
    to_bottom->resize(80, 10);
    to_top->clicked().connect(this, &Example::top_shift);
    to_bottom->clicked().connect(this, &Example::bottom_shift);
    //
    WPushButton* zoom_in = new Wt::WPushButton("zoom In");
    zoom_in->resize(80, 10);
    WPushButton* zoom_out = new Wt::WPushButton("zoom Out");
    zoom_out->resize(80, 10);
    zoom_in->clicked().connect(mv_, &MapViewer::zoom_in);
    zoom_out->clicked().connect(mv_, &MapViewer::zoom_out);
    //
    WContainerWidget* cw_of_zoom_to = new Wt::WContainerWidget();
    WLabel* label_of_zoom_to = new Wt::WLabel("set zoom:", cw_of_zoom_to);
    edit_of_zoom_to_ = new WLineEdit("9", cw_of_zoom_to);
    edit_of_zoom_to_->setValidator(new WIntValidator(1, 17));
    label_of_zoom_to->setBuddy(edit_of_zoom_to_);
    WHBoxLayout* menu_hl_of_zoom_to = new WHBoxLayout();
    menu_hl_of_zoom_to->addWidget(cw_of_zoom_to, AlignTop);
    WPushButton* button_of_zoom_to = new Wt::WPushButton("OK");
    button_of_zoom_to->resize(40, 10);
    button_of_zoom_to->clicked().connect(this, &Example::set_zoom_to);
    menu_hl_of_zoom_to->addWidget(button_of_zoom_to, AlignCenter);
    //
    WContainerWidget* cw_of_pan_to_lng = new Wt::WContainerWidget();
    WContainerWidget* cw_of_pan_to_lat = new Wt::WContainerWidget();
    WLabel* label_of_pan_to_lng = new Wt::WLabel("set lng:", cw_of_pan_to_lng);
    WLabel* label_of_pan_to_lat = new Wt::WLabel("set lat:", cw_of_pan_to_lat);
    edit_of_pan_to_lng_ = new WLineEdit("20.4", cw_of_pan_to_lng);
    edit_of_pan_to_lng_->setValidator(new WDoubleValidator(-180.0, 180.0));
    label_of_pan_to_lng->setBuddy(edit_of_pan_to_lng_);
    edit_of_pan_to_lat_ = new WLineEdit("54.7167", cw_of_pan_to_lat);
    edit_of_pan_to_lat_->setValidator(new WDoubleValidator(-90.0, 90.0));
    label_of_pan_to_lat->setBuddy(edit_of_pan_to_lat_);
    WHBoxLayout* menu_hl_of_pan_to = new WHBoxLayout();
    menu_hl_of_pan_to->addWidget(cw_of_pan_to_lng, AlignTop);
    menu_hl_of_pan_to->addWidget(cw_of_pan_to_lat, AlignTop);
    WPushButton* button_of_pan_to = new Wt::WPushButton("OK");
    button_of_pan_to->resize(40, 10);
    button_of_pan_to->clicked().connect(this, &Example::set_pan_to);
    menu_hl_of_pan_to->addWidget(button_of_pan_to, AlignTop);
    //
    click_pos_ = new WText();
    //
    WVBoxLayout* vl_menu = new WVBoxLayout();
    WHBoxLayout* hl_menu_zooms = new WHBoxLayout();
    hl_menu_zooms->addWidget(zoom_in, AlignTop);
    hl_menu_zooms->addWidget(zoom_out, AlignTop);
    WGridLayout* shift_hl_menu = new WGridLayout();
    shift_hl_menu->addWidget(to_top, 0, 1, AlignTop);
    shift_hl_menu->addWidget(to_left, 1, 0, AlignTop);
    shift_hl_menu->addWidget(to_right, 1, 2, AlignTop);
    shift_hl_menu->addWidget(to_bottom, 2, 1, AlignTop);
    //
    vl_menu->addLayout(shift_hl_menu, AlignTop);
    vl_menu->addLayout(hl_menu_zooms, AlignTop);
    vl_menu->addLayout(menu_hl_of_zoom_to, AlignTop);
    vl_menu->addLayout(menu_hl_of_pan_to, AlignTop);
    vl_menu->addWidget(click_pos_, AlignTop);
    hl->addLayout(vl_menu, AlignTop);
    cw->setLayout(hl, AlignTop);
}

void Example::set_zoom_to() {
    mv_->zoom_to(boost::lexical_cast<int>(edit_of_zoom_to_->text()));
}

void Example::set_pan_to() {
    int x = boost::lexical_cast<double>(edit_of_pan_to_lat_->text());
    int y = boost::lexical_cast<double>(edit_of_pan_to_lng_->text());
    mv_->pan_to(MapViewer::Coordinate(x, y));
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

void Example::get_pos(const MapViewer::Coordinate& pos) {
    click_pos_->setText("You clicked near: " +
                        TO_S(pos.longitude()) + " " + TO_S(pos.latitude()));
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

