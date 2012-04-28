/*
 * Copyright (C) 2011 Ramil Mintaev
 *
 * See the LICENSE.MapViewer file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/Wc/MapViewer.hpp>

using namespace Wt;
using namespace Wt::Wc;

class MapViewerLiteApp : public WApplication {
public:
    MapViewerLiteApp(const WEnvironment& env):
        WApplication(env) {
        MapViewer* map_viewer = new MapViewer(root());
        map_viewer->resize(512, 420);
        map_viewer->set_center(MapViewer::Coordinate(54.8, 20.25), 9);
        // go to Kaliningrad, zoom 9
    }
};

WApplication* createMapViewerLiteApp(const WEnvironment& env) {
    return new MapViewerLiteApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createMapViewerLiteApp);
}

