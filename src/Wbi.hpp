/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef WT_WBI_HPP_
#define WT_WBI_HPP_

#include <string>
#include <vector>

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "global.hpp"

namespace Wt {

namespace Wc {

class AbstractArgument : WCompositeWidget {
public:
    AbstractArgument(const std::string& option_name="");
    //void set_task(AbstractTask* task);
    virtual void add_args(std::vector<std::string>& args);

protected:
    std::string option_value_;
    std::string option_name_;
    virtual void set_option()=0;
};

class AbstractInput : public AbstractArgument {
public:
    AbstractInput(const std::string& option_name);
    /** Return internal WFormWidget.
    By default return 0
    */
    virtual WFormWidget* form_widget();
    void set_required(bool value);

};

class FormWidgetInput : public AbstractInput {
public:
    FormWidgetInput(WFormWidget* widget, const std::string& option_name);
    WFormWidget* form_widget();
};

class LineEditInput : public FormWidgetInput {
public:
    LineEditInput(WLineEdit* widget, const std::string& option_name);

protected:
    void set_option();
};

class FileInput : public AbstractInput {
public:
    FileInput(const std::string& option_name);

protected:
    void set_option();
};

class TextFileInput : public FileInput {
public:
    TextFileInput(const std::string& option_name);
    WFormWidget* form_widget();

protected:
    void set_option();
};

class AbstractOutput : public AbstractArgument {
public:
    AbstractOutput(const std::string& option_name);
    void set_selectable(bool value);
    void set_selected(bool value);
    void virtual task_finished_handler()=0;
};

class FileOutput : public AbstractOutput {
public:
    FileOutput(const std::string& option_name);

protected:
    void set_option();
    void task_finished_handler();
};

class TextFileOutput : public FileOutput {
public:
    TextFileOutput(const std::string& option_name);

protected:
    void task_finished_handler();
};

class AbstractTask : public WCompositeWidget {
public:
    AbstractTask(WContainerWidget* p=0);
    void add_input(AbstractInput* input, const WString& name,
            const WString& description="");
    void add_output(AbstractOutput* output, const WString& name,
            const WString& description="");
    void set_runner(AbstractTaskRunner* runer);
};

class AbstractTaskRunner : public WObject {
public:
    AbstractTaskRunner();
    virtual void run(AbstractTask* task)=0;
    virtual bool is_finished()=0;
};

}

}

#endif

