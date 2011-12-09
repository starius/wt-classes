/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WApplication>
#include <Wt/WIntValidator>
#include <Wt/WLineEdit>
#include <Wt/WCheckBox>
#include <Wt/Wc/Wbi.hpp>

using namespace Wt;
using namespace Wt::Wc;

TaskNumberQueue queue(2);

class XxdApp : public WApplication {
public:
    XxdApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates(true);
        messageResourceBundle().use(Wt::WApplication::appRoot() +
                                    "locales/wtclasses");
        TableTask* task = new TableTask(root());
        task->add_input(new FileInput("<"), "Binary file");
        WLineEdit* cols = new WLineEdit("16");
        cols->setValidator(new WIntValidator(1, 256));
        task->add_input(new LineEditInput(cols, "-c"), "Octets per line");
        WCheckBox* bits = new WCheckBox();
        task->add_input(new BoolInput(bits, "-b"), "Bits");
        task->add_output(new ViewFileOutput(">"), "Hex dump");
        task->set_runner(new ForkingRunner("sleep 5; xxd"));
        task->set_queue(&queue);
        bits->checked().connect(boost::bind(&WLineEdit::setText, cols, "6"));
        bits->unChecked().connect(boost::bind(&WLineEdit::setText, cols, "16"));
    }
};

WApplication* createApplication(const WEnvironment& env) {
    return new XxdApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createApplication);
}

