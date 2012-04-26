
#include <Wt/WApplication>
#include <Wt/WImage>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/Wc/MapImage.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

class MapImageApp : public WApplication {
public:
    MapImageApp(const WEnvironment& env):
        WApplication(env) {
        setInternalPath("/lena/");
        MapImage* image = new MapImage(new WImage("http://www.kitware.com/blog/"
                                       "files/128_1232259669.jpg"), root());
        new WBreak(root());
        text_ = new WText(root());
        image->clicked().connect(this, &MapImageApp::click_handler);
        image->image()->setAlternateText("Lena");
        image->image()->resize(512, 512); // not necessarily
    }

private:
    WText* text_;

    void click_handler(WMouseEvent::Coordinates xy) {
        text_->setText(TO_S(xy.x) + "," + TO_S(xy.y));
    }
};

WApplication* createMapImageApp(const WEnvironment& env) {
    return new MapImageApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createMapImageApp);
}

