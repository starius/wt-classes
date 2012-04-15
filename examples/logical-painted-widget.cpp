
#include <Wt/WApplication>
#include <Wt/WPaintedWidget>
#include <Wt/WPainter>
#include <Wt/WPainterPath>
#include <Wt/Wc/LogicalPaintedWidget.hpp>

using namespace Wt;
using namespace Wt::Wc;

class TestPainting : public LogicalPaintedWidget {
public:
    TestPainting(WContainerWidget* p = 0) :
        LogicalPaintedWidget(p) {
        resize(500, 500);
        set_logical_window(Wt::WRectF(0, 0, 1.0, 1.0));
    }

protected:
    void paintEvent(WPaintDevice* device) {
        Wt::WPainter painter(device);
        use_logical(painter);
        painter.drawEllipse(0, 0, 1, 1);
        Wt::WPainterPath path;
        path.moveTo(0, 1);
        path.lineTo(1, 0);
        painter.drawPath(path);
    }
};

class LogicalPaintedApp : public WApplication {
public:
    LogicalPaintedApp(const WEnvironment& env):
        WApplication(env) {
        new TestPainting(root());
    }
};

WApplication* createLogicalPaintedApp(const WEnvironment& env) {
    return new LogicalPaintedApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createLogicalPaintedApp);
}

