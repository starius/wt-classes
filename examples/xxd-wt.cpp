/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

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
        task->add_input(new FileInput("<"), "Binary file");
        task->add_output(new ViewFileOutput(">"), "Hex dump");
        task->set_runner(new ForkingRunner("xxd"));
        task->finished().connect(this, &WApplication::triggerUpdate);
    }
};

WApplication* createApplication(const WEnvironment& env) {
    return new XxdApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createApplication);
}

