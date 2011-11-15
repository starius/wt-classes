
#include <Wt/WApplication>
#include <Wt/Wc/Wbi.hpp>

using namespace Wt;
using namespace Wt::Wc;

class XxdApp : public WApplication {
public:
    XxdApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates(true);
        messageResourceBundle().use(Wt::WApplication::appRoot() +
                                    "locales/wtclasses");
        TableTask* task = new TableTask(root());
        task->add_input(new FileInput(""), "Binary file");
        task->add_output(new ViewFileOutput(""), "Hex dump");
        task->set_runner(new ForkingTaskRunner("xxd"));
        task->finished().connect(this, &WApplication::triggerUpdate);
    }
};

WApplication* createApplication(const WEnvironment& env) {
    return new XxdApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createApplication);
}

