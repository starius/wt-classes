/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/version.hpp>
#if BOOST_VERSION >= 104400
#define BOOST_FILESYSTEM_VERSION 3
#endif

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WCheckBox>
#include <Wt/WFileUpload>
#include <Wt/WTextArea>
#include <Wt/WBreak>
#include <Wt/WFileResource>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WApplication>
#include <Wt/WServer>

#include "config.hpp"
#include "global.hpp"

#ifndef WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION
// FIXME nasty public morozov
#define private friend class Wt::Wc::AbstractOutput; \
                friend class Wt::Wc::TableTask; \
                friend class Wt::Wc::FormWidgetInput; private
#include <Wt/WCompositeWidget>
#undef private
#define implementation() Wt::WCompositeWidget::impl_
#endif // WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION

#include "Wbi.hpp"
#include "TableForm.hpp"
#include "FileView.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

AbstractArgument::AbstractArgument(const std::string& option_name):
    option_name_(option_name)
{ }

void AbstractArgument::add_args(const ArgUser& f) const {
    add_args_impl(f);
}

bool AbstractArgument::large() const {
    return large_impl();
}

void AbstractArgument::add_args_impl(const ArgUser& f) const {
    add_option(f);
}

void AbstractArgument::add_option(const ArgUser& f) const {
    const_cast<AbstractArgument*>(this)->set_option();
    if (!option_name_.empty()) {
        f(option_name_, /* escape */ false);
    }
    if (!option_value_.empty()) {
        f(option_value_, /* escape */ true);
    }
}

bool AbstractArgument::large_impl() const {
    return false;
}

AbstractInput::AbstractInput(const std::string& option_name):
    AbstractArgument(option_name), required_(true)
{ }

WFormWidget* AbstractInput::form_widget() {
    return form_widget_impl();
}

const WFormWidget* AbstractInput::form_widget() const {
    AbstractInput* nonconst_this = const_cast<AbstractInput*>(this);
    return nonconst_this->form_widget_impl();
}

AbstractInput::State AbstractInput::state() const {
    return VALID;
}

bool AbstractInput::accepted() const {
    return state() == VALID || (!is_required() && state() == EMPTY);
}

WFormWidget* AbstractInput::form_widget_impl() {
    return 0;
}

void AbstractInput::add_args_impl(const ArgUser& f) const {
    if (state() == VALID) {
        add_option(f);
    }
}

FormWidgetInput::FormWidgetInput(WFormWidget* widget,
                                 const std::string& option_name):
    AbstractInput(option_name) {
    if (widget) {
        set_widget(widget);
    }
}

AbstractInput::State FormWidgetInput::state() const {
    WFormWidget* fw = const_cast<WFormWidget*>(form_widget()); // FIXME
    WValidator::State validator_state = fw ? fw->validate() :
                                        WValidator::InvalidEmpty;
    AbstractInput::State result = VALID;
    if (validator_state == WValidator::Invalid) {
        result = INVALID;
        set_error_message(tr("wc.wbi.Error_invalid"));
    } else if (validator_state == WValidator::InvalidEmpty) {
        result = EMPTY;
        set_error_message(tr("wc.wbi.Error_empty"));
    }
    return result;
}

void FormWidgetInput::set_widget(WFormWidget* widget) {
    setImplementation(widget);
}

WFormWidget* FormWidgetInput::form_widget_impl() {
    return DOWNCAST<WFormWidget*>(implementation());
}

LineEditInput::LineEditInput(WLineEdit* widget, const std::string& option_name):
    FormWidgetInput(widget, option_name)
{ }

WLineEdit* LineEditInput::line_edit() {
    return DOWNCAST<WLineEdit*>(form_widget());
}

const WLineEdit* LineEditInput::line_edit() const {
    return DOWNCAST<const WLineEdit*>(form_widget());
}

void LineEditInput::set_option() {
    if (line_edit()) {
        option_value_ = line_edit()->text().toUTF8();
    }
};

FileInput::FileInput(const std::string& option_name):
    AbstractInput(option_name),
    too_large_(false) {
    impl_ = new WContainerWidget();
    file_upload_ = new WFileUpload(impl_);
    file_upload_->changed().connect(file_upload_, &WFileUpload::upload);
    file_upload_->uploaded().connect(boost::bind(&FileInput::set_too_large,
                                     this, false));
    file_upload_->fileTooLarge().connect(boost::bind(&FileInput::set_too_large,
                                         this, true));
    setImplementation(impl_);
}

AbstractInput::State FileInput::state() const {
    State result = VALID;
    if (file_upload_->emptyFileName()) {
        set_error_message(tr("wc.wbi.Error_empty"));
        result = EMPTY;
    }
    if (too_large_) {
        set_error_message(tr("wc.wbi.Error_too_large"));
        result = INVALID;
    }
    return result;
}

void FileInput::set_option() {
    option_value_ = file_upload_->spoolFileName();
}

TextFileInput::TextFileInput(const std::string& option_name):
    FileInput(option_name) {
    new WBreak(impl_);
    text_area_ = new WTextArea(impl_);
    new WBreak(impl_);
    error_ = new WText(impl_);
    file_upload_->uploaded().connect(this, &TextFileInput::uploaded_handler);
    file_upload_->uploaded().connect(boost::bind(&WText::setText, error_, ""));
    file_upload_->fileTooLarge().connect(boost::bind(&WText::setText, error_,
                                         tr("wc.wbi.Error_too_large")));
}

AbstractInput::State TextFileInput::state() const {
    State result = FileInput::state();
    if (result == EMPTY && !text_area_->text().empty()) {
        set_error_message("");
        result = VALID;
    }
    return result;
}

void TextFileInput::uploaded_handler() {
    std::string filename = file_upload_->spoolFileName();
    std::ifstream file(filename.c_str());
    text_area_->setText(std::string(std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>()));
}

WFormWidget* TextFileInput::form_widget_impl() {
    return text_area_;
}

void TextFileInput::set_option() {
    std::string filename = file_upload_->spoolFileName();
    if (filename.empty()) {
        filename = unique_filename();
    }
    std::ofstream file(filename.c_str());
    file << text_area_->text().toUTF8();
    file.close();
    option_value_ = filename;
}

bool TextFileInput::large_impl() const {
    return true;
}

BoolInput::BoolInput(WCheckBox* widget, const std::string& name_if_true):
    FormWidgetInput(widget, ""),
    name_if_true_(name_if_true) {
}

BoolInput::BoolInput(WCheckBox* widget, const std::string& name_if_true,
                     const std::string& name_if_false):
    FormWidgetInput(widget, ""),
    name_if_true_(name_if_true), name_if_false_(name_if_false) {
}

BoolInput::BoolInput(WCheckBox* widget, const std::string& name,
                     const std::string& value_if_true,
                     const std::string& value_if_false):
    FormWidgetInput(widget, ""),
    name_if_true_(name), name_if_false_(name),
    value_if_true_(value_if_true), value_if_false_(value_if_false) {
}

WCheckBox* BoolInput::check_box() {
    return DOWNCAST<WCheckBox*>(form_widget());
}

const WCheckBox* BoolInput::check_box() const {
    return DOWNCAST<const WCheckBox*>(form_widget());
}

void BoolInput::set_option() {
    if (check_box() && check_box()->isChecked()) {
        option_name_ = name_if_true_;
        option_value_ = value_if_true_;
    } else {
        option_name_ = name_if_false_;
        option_value_ = value_if_false_;
    }
}

AbstractOutput::AbstractOutput(const std::string& option_name):
    AbstractArgument(option_name),
    selectable_(true), selected_by_default_(true),
    selected_(selected_by_default_) {
    setImplementation(new WContainerWidget());
    WCheckBox* box = new WCheckBox();
    box->changed().connect(this, &AbstractOutput::select_handler);
    container()->addWidget(box);
    update_checkbox();
}

void AbstractOutput::set_selectable(bool value) {
    selectable_ = value;
    update_checkbox();
}

void AbstractOutput::set_selected_by_default(bool value) {
    selected_by_default_ = value;
    update_checkbox();
}

bool AbstractOutput::is_needed() const {
    return !is_selectable() || is_selected();
}

void AbstractOutput::select_handler() {
    WCheckBox* box = DOWNCAST<WCheckBox*>(sender());
    selected_ = box->isChecked();
}

void AbstractOutput::update_checkbox() {
    WWidget* widget = container()->widget(0);
    if (isinstance<WCheckBox>(widget)) {
        WCheckBox* box = DOWNCAST<WCheckBox*>(widget);
        box->setChecked(selected_by_default_);
        box->setEnabled(selectable_);
    }
}

WContainerWidget* AbstractOutput::container() {
    return DOWNCAST<WContainerWidget*>(implementation());
}

void AbstractOutput::finished_handler() {
    if (is_needed()) {
        finished_handler_impl();
    }
}

void AbstractOutput::add_args_impl(const ArgUser& f) const {
    if (is_needed()) {
        add_option(f);
    }
}

FileOutput::FileOutput(const std::string& option_name,
                       const FileOutput::NameGen& temp_gen,
                       const std::string& download_mime):
    AbstractOutput(option_name),
    temp_gen_(temp_gen),
    suggested_gen_(boost::bind(&FileOutput::temp_notdir, this)),
    download_mime_(download_mime)
{ }

FileOutput::~FileOutput() {
    if (!temp_file_.empty()) {
        remove(temp_file_.c_str());
        temp_file_ = "";
    }
}

const std::string& FileOutput::temp_file() const {
    if (temp_file_.empty()) {
        temp_file_ = temp_gen_();
    }
    return temp_file_;
}

std::string FileOutput::suggested_name() const {
    return suggested_gen_();
}

std::string FileOutput::unique_name() {
    return unique_filename();
}

std::string FileOutput::temp_notdir() const {
#if BOOST_FILESYSTEM_VERSION == 3
    return boost::filesystem::path(temp_file()).filename().string();
#else
    return boost::filesystem::path(temp_file()).filename();
#endif
}

void FileOutput::set_option() {
    option_value_ = temp_file();
}

void FileOutput::finished_handler_impl() {
    container()->clear();
    container()->addWidget(anchor());
}

WAnchor* FileOutput::anchor() const {
    WFileResource* r = new WFileResource(download_mime_, temp_file(), wApp);
    r->suggestFileName(suggested_name());
    return new WAnchor(r, tr("wc.wbi.Download"));
}

ViewFileOutput::ViewFileOutput(const std::string& option_name,
                               const NameGen& temp_gen,
                               const std::string& download_mime,
                               const std::string& view_mime):
    FileOutput(option_name, temp_gen, download_mime),
    view_mime_(view_mime)
{ }

void ViewFileOutput::finished_handler_impl() {
    container()->clear();
    WFileResource* r = new WFileResource(view_mime_, temp_file(), wApp);
    WAnchor* a = new WAnchor(r, tr("wc.wbi.View"));
    a->setTarget(TargetNewWindow);
    container()->addWidget(FileOutput::anchor());
    container()->addWidget(new WText(" | "));
    container()->addWidget(a);
}

PrintFileOutput::PrintFileOutput(const std::string& option_name,
                                 const NameGen& temp_gen,
                                 const std::string& download_mime):
    FileOutput(option_name, temp_gen, download_mime) {
    file_view_ = new FileView();
}

PrintFileOutput::~PrintFileOutput() {
    delete file_view_;
}

void PrintFileOutput::finished_handler_impl() {
    container()->removeWidget(file_view_);
    container()->clear();
    container()->addWidget(FileOutput::anchor());
    container()->addWidget(new WBreak());
    file_view_->set_filename(temp_file());
    container()->addWidget(file_view_);
}

AbstractTask::AbstractTask(WContainerWidget* p):
    WCompositeWidget(p), runner_(0), queue_(0), queued_(false) {
    if (wApp->updatesEnabled()) {
        bound_trigger_updates_ = bound_post(updates_trigger);
        changed_.connect(this, &AbstractTask::trigger_updates);
    }
}

void AbstractTask::add_input(AbstractInput* input, const WString& name,
                             const WString& description) {
    args_.push_back(input);
    add_input_impl(input, name, description);
}

void AbstractTask::add_output(AbstractOutput* output, const WString& name,
                              const WString& description) {
    args_.push_back(output);
    add_output_impl(output, name, description);
}

void AbstractTask::set_runner(AbstractRunner* runner) {
    if (runner_) {
        WObject::removeChild(runner_);
    }
    runner_ = runner;
    WObject::addChild(runner);
    runner_->set_task(this);
}

void AbstractTask::set_queue(AbstractQueue* queue) {
    if (state() == UNSET || state() == NEW) {
        queue_ = queue;
    }
}

void AbstractTask::run() {
    if (check_task()) {
        if (queue_) {
            queue_->add(this);
            changed_.emit();
        } else {
            run_impl(/* check */ false);
        }
    }
}

void AbstractTask::cancel() {
    if (state() == QUEUED) {
        queue_->remove(this);
    } else {
        runner_->cancel();
    }
    changed_.emit();
}

void AbstractTask::visit_args(const AbstractArgument::ArgUser& f) const {
    BOOST_FOREACH (AbstractArgument* arg, args_) {
        arg->add_args(f);
    }
}

RunState AbstractTask::state() const {
    return queued_ ? QUEUED : runner_ ? runner_->state() : UNSET;
}

const char* AbstractTask::state_to_string(RunState s) {
    if (s == NEW) {
        return "wc.wbi.New";
    } else if (s == QUEUED) {
        return "wc.wbi.Queued";
    } else if (s == WORKING) {
        return "wc.wbi.Working";
    } else if (s == FINISHED) {
        return "wc.wbi.Finished";
    } else {
        return "";
    }
}

const char* AbstractTask::state_to_string() {
    return state_to_string(state());
}

void AbstractTask::set_message(const WString& message) {
    set_message_impl(message);
}

void AbstractTask::update_error_message(AbstractInput* /* input */)
{ }

bool AbstractTask::check_inputs() {
    bool accepted = true;
    BOOST_FOREACH (AbstractArgument* arg, args_) {
        if (isinstance<AbstractInput>(arg)) {
            AbstractInput* input = DOWNCAST<AbstractInput*>(arg);
            accepted = accepted && input->accepted();
            update_error_message(input);
        }
    }
    return accepted;
}

bool AbstractTask::check_task() {
    bool inputs_accepted = check_inputs();
    bool validator_accepted = validator_.empty() || validator_();
    return inputs_accepted && validator_accepted;
}

void AbstractTask::set_message_impl(const WString& /* message */)
{ }

void AbstractTask::changed_emitter() {
    BOOST_FOREACH (AbstractArgument* arg, args_) {
        if (isinstance<AbstractOutput>(arg)) {
            DOWNCAST<AbstractOutput*>(arg)->finished_handler();
        }
    }
    changed_.emit();
}

void AbstractTask::run_impl(bool check) {
    if (!check || check_task()) {
        if (runner_) {
            runner_->run();
        }
    }
    changed_.emit();
}

void AbstractTask::trigger_updates() {
    bound_trigger_updates_();
}

class TTImpl : public WContainerWidget {
public:
    TTImpl() {
        inputs_ = new TableForm(this);
        inputs_->section(tr("wc.wbi.Inputs"));
        outputs_ = new TableForm(this);
        outputs_->section(tr("wc.wbi.Outputs"));
        run_ = new WPushButton(tr("wc.wbi.Run"), this);
        cancel_ = new WPushButton(tr("wc.wbi.Cancel"), this);
        cancel_->hide();
        new WBreak(this);
        state_ = new WText(this);
        state_->setStyleClass("wc_wbi_state");
        new WBreak(this);
        message_ = new WText(this);
        message_->setStyleClass("wc_task_message");
    }

private:
    TableForm* inputs_;
    TableForm* outputs_;
    WPushButton* run_;
    WPushButton* cancel_;
    WText* state_;
    WText* message_;

    friend class TableTask;
};

TableTask::TableTask(WContainerWidget* p):
    AbstractTask(p) {
    TTImpl* impl = new TTImpl();
    setImplementation(impl);
    impl->run_->clicked().connect(this, &AbstractTask::run);
    impl->cancel_->clicked().connect(this, &AbstractTask::cancel);
    changed().connect(this, &TableTask::changed_handler);
}

void TableTask::add_input_impl(AbstractInput* input, const WString& name,
                               const WString& description) {
    TTImpl* impl = DOWNCAST<TTImpl*>(implementation());
    bool row = !input->large();
    impl->inputs_->item(name, description, input->form_widget(), input, row);
}

void TableTask::add_output_impl(AbstractOutput* output, const WString& name,
                                const WString& description) {
    TTImpl* impl = DOWNCAST<TTImpl*>(implementation());
    bool row = !output->large();
    impl->outputs_->item(name, description, 0, output, row);
}

void TableTask::update_error_message(AbstractInput* input) {
    TTImpl* impl = DOWNCAST<TTImpl*>(implementation());
    const WString& message = input->accepted() ? WString::Empty :
                             input->error_message();
    impl->inputs_->set_comment(input, message);
}

void TableTask::set_message_impl(const WString& message) {
    TTImpl* impl = DOWNCAST<TTImpl*>(implementation());
    impl->message_->setText(message);
}

bool waiting_state(RunState state) {
    return state == WORKING || state == QUEUED;
}

void TableTask::changed_handler() {
    TTImpl* impl = DOWNCAST<TTImpl*>(implementation());
    bool cancel = waiting_state(state());
    bool run = !cancel;
    set_hidden(impl->run_, !run);
    set_hidden(impl->cancel_, !cancel);
    impl->state_->setText(tr("wc.wbi.State_is").arg(tr(state_to_string())));
}

AbstractRunner::AbstractRunner():
    state_(UNSET),
    task_(0) {
    bound_finished_handler_ = bound_post(boost::bind(
            &AbstractRunner::finished_handler, this));
}

AbstractRunner::~AbstractRunner() {
    remove_from_queue();
}

void AbstractRunner::run() {
    if (!(state() == UNSET || state() == WORKING)) {
        run_impl();
    }
}

void AbstractRunner::cancel() {
    if (state() == WORKING) {
        cancel_impl();
    }
    remove_from_queue();
    set_state(NEW);
}

RunState AbstractRunner::state() const {
    return task_ ? state_ : UNSET;
}

void AbstractRunner::finish() {
    set_state(FINISHED);
    remove_from_queue();
    bound_finished_handler_();
}

void AbstractRunner::remove_from_queue() {
    if (task_ && task_->queue_) {
        task_->queue_->remove(task_);
    }
}

void AbstractRunner::set_task(AbstractTask* task) {
    task_ = task;
    set_state(NEW);
}

void AbstractRunner::finished_handler() {
    task()->changed_emitter();
}

ForkingRunner::ForkingRunner(const std::string& command,
                             const std::string& suffix):
    command_(command), suffix_(suffix), pid_file_(FileOutput::unique_name()),
    signal_(9)
{ }

ForkingRunner::~ForkingRunner() {
    if (state() == WORKING) {
        cancel_impl();
    }
    remove(pid_file_.c_str());
}

void ForkingRunner::run_impl() {
    if (state() == FINISHED) {
        set_state(NEW);
    }
    if (state() == NEW) {
        set_state(WORKING);
        thread_ = boost::thread(&ForkingRunner::start_process, this, command());
    }
}

void ForkingRunner::cancel_impl() {
    thread_.interrupt();
    std::stringstream cmd;
    cmd << "pkill -" << signal_ << " -P `cat " << pid_file_ << "`";
    system(cmd.str().c_str());
}

std::string ForkingRunner::escape_arg(const std::string& arg) {
    return std::string("'") + boost::replace_all_copy(arg, "'", "'\''") + "'";
}

void arg_to_stream(std::stringstream& stream, const std::string& arg,
                   bool escape) {
    stream << " ";
    if (escape) {
        stream << ForkingRunner::escape_arg(arg);
    } else {
        stream << arg;
    }
    stream << " ";
}

std::string ForkingRunner::command() const {
    std::stringstream cmd;
    cmd << command_ << " ";
    task()->visit_args(boost::bind(arg_to_stream, boost::ref(cmd), _1, _2));
    cmd << suffix_ << " ";
    std::stringstream cmd_wrapper;
    cmd_wrapper << "echo $$ > " << pid_file_ << ";";
    cmd_wrapper << "exec bash -c " << ForkingRunner::escape_arg(cmd.str());
    cmd_wrapper << ";";
    return cmd_wrapper.str();
}

void ForkingRunner::start_process(std::string cmd) {
    system(cmd.c_str());
    if (!boost::this_thread::interruption_requested()) {
        finish();
    }
}

AbstractQueue::AbstractQueue(WObject* p):
    WObject(p)
{ }

void AbstractQueue::add(AbstractTask* task) {
    mutex_.lock();
    task2run_[task] = bound_post(boost::bind(&AbstractTask::run_impl,
                                 task, /* check */ true));
    add_impl(task);
    mutex_.unlock();
}

void AbstractQueue::remove(AbstractTask* task) {
    mutex_.lock();
    remove_impl(task);
    task2run_.erase(task);
    mutex_.unlock();
}

void AbstractQueue::run_task(AbstractTask* task) {
    task2run_[task]();
}

TaskNumberQueue::TaskNumberQueue(int max_tasks, WObject* p):
    AbstractQueue(p), max_tasks_(max_tasks)
{ }

void TaskNumberQueue::add_impl(AbstractTask* task) {
    waiting_.push_back(task);
    set_queued(task);
    try_to_run();
}

void TaskNumberQueue::remove_impl(AbstractTask* task) {
    waiting_.remove(task);
    running_.erase(task);
    try_to_run();
}

void TaskNumberQueue::try_to_run() {
    if (!waiting_.empty()) {
        if (max_tasks_ == -1 || (int)running_.size() < max_tasks_) {
            AbstractTask* task = waiting_.front();
            waiting_.pop_front();
            set_queued(task, false);
            run_task(task);
            running_.insert(task);
        }
    }
}

TaskCountup::TaskCountup(AbstractTask* task, WContainerWidget* parent):
    Countdown(parent),
    task_(task),
    prev_state_(NEW) {
    task_->changed().connect(this, &TaskCountup::changed_handler);
    hide();
}

void TaskCountup::changed_handler() {
    if (waiting_state(task_->state()) && !waiting_state(prev_state_)) {
        resume();
        set_since(now());
        show();
    } else if (!waiting_state(task_->state()) && waiting_state(prev_state_)) {
        pause();
    }
    prev_state_ = task_->state();
}

}

}

