
#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/WTextEdit>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

class TextEditApp : public WApplication {
public:
    TextEditApp(const WEnvironment& env):
        WApplication(env) {
        new WText("This text should be before text edit", root());
        fix_text_edit(new WTextEdit(root()));
        new WText("This text should be after text edit", root());
    }
};

WApplication* createTextEditApp(const WEnvironment& env) {
    return new TextEditApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createTextEditApp);
}

