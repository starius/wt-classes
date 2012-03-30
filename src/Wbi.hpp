/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_WBI_HPP_
#define WC_WBI_HPP_

#include <string>
#include <vector>
#include <list>
#include <set>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "global.hpp"
#include "Countdown.hpp"

namespace Wt {

namespace Wc {

/** \defgroup wbi Web-based interface of a program
Classes to simplify creation of a WebUI for programs
running from command line.

This module is the layer between Wt user interface widgets
and command line options of a program.

<h3>task</h3>

The central class of this module is \ref AbstractTask "a task".
It is connected with:
 - \ref AbstractArgument "arguments",
 - \ref AbstractRunner "runner of the task",
 - \ref AbstractQueue "task queue",
 - \ref TaskCountup "time count-up".

In UI, the task is represented as a complex widget,
all arguments are parented into the task.

At any moment, the task (and the runner) is in some \ref RunState "state".
Changes of task state are followed by \ref AbstractTask::changed "the signal".

<h3>arguments</h3>

Each \ref AbstractArgument "argument" represents
an argument of a command, as well as widgets, needed
to input the information from user or to show it to him.

Base class of an argument is WCompositeWidget.

While creating of the command to be executed, all the arguments are asked
\ref AbstractArgument::add_args "to be added" to the command line.
Most of arguments are paired (e.g., "-o", "main.o").
Normally, the name of an option (e.g., "-o") is passed to the constructor
of the argument.

Arguments are divided into two groups:
\ref AbstractInput "input arguments" and
\ref AbstractOutput "output arguments".

Input arguments are divided into
\ref AbstractInput::is_required "required and optional".
An optional input can be left \ref AbstractInput::EMPTY "empty"
(or with its default value).
In case of an error, input should set appropriate
\ref AbstractInput::error_message "error message"
to be shown by the task to the user.
Use AbstractTask::add_input() to add input argument to the task.

\ref AbstractOutput "Output argument" represents the information,
returned by the program.
Some outputs can be checked or unchecked, so only needed part of the output
is produced.
Use AbstractTask::add_output() to add output argument to the task.

<h3>runner</h3>

\ref AbstractRunner "A runner" is a class, dealing with actual executing.
It takes the task and add all its arguments to the command line.
Use AbstractTask::set_runner() to set the runner to the task.

Currently, there is only one implementation of a runner: ForkingRunner.
It calls a program using system() function.
Program name is passed to the constructor of ForkingRunner.
Maybe in the future other runner types would be implemented,
for example, a runner, passing command line directly to Python interpreter.

<h3>task queue (optional)</h3>

\ref AbstractQueue "Task queue" can be used to control the launch of a program.
A queue can hold over the task is state prior to the runner calling.
Use AbstractTask::set_queue() to let the queue control the task.

There is one queue implemented: TaskNumberQueue.
It controls the number of tasks running, putting extra tasks to the queue.

<h3>count-up (optional)</h3>

TaskCountup is a widget, showing the time from task start.
It is not located inside task widget, it is up to you where to put it.
Just pass the task to TaskCountup constructor.

<h3>example</h3>

The code below creates Web UI for \c xxd.
The command is like "xxd < in > out".
\c sleep command was added to simulate time consuming task.

\include examples/xxd-wt-lite.cpp

Detailed example:

\include examples/xxd-wt.cpp
*/

/** Abstract base class for any command-line argument of a program.

\ingroup wbi
*/
class AbstractArgument : public WCompositeWidget {
public:
    /** Function to be applied to command line argument.
    Arguments:
     - string -- argument
     - bool -- if the argument must (or must not) be escaped.
       Option names must not escaped, options values must be escaped.
    */
    typedef boost::function<void(const std::string&, bool)> ArgUser;

    /** Constructor.
    \param option_name Name of a program option (i.e., "-i", "--output").
    */
    AbstractArgument(const std::string& option_name = "");

    /** Call the function for each of adding arguments.
    The implementation of this method is add_args_impl()
    */
    void add_args(const ArgUser& f) const;

    /** Get if the widget is large.
    A widget is considered large if it is unlikely to fit on one line.

    The implementation of the method is large_impl()
    */
    bool large() const;

protected:
    /** Name of a program option (i.e., "-i", "--output") */
    std::string option_name_;

    /** Value of a program option (i.e., "123", "in.txt", "out.pdf") */
    std::string option_value_;

    /** Call the function for each of adding arguments (implementation).
    Default implementation adds option_name_ and option_value_
    using add_option().
    If option name or value is an empty string, it is skipped.

    \note Reimplement this method if the number of added arguments
        is more than 2 (e.g. variadic argument list).
    */
    virtual void add_args_impl(const ArgUser& f) const;

    /** Call the function for option_name_ and option_value_.
    If option name or value is an empty string, it is skipped.

    This method calls set_option() method.
    */
    void add_option(const ArgUser& f) const;

    /** Set option value.
    You should implement this method in descendant classes.
    This method should get information from widgets and
    set correct value of option_value_.
    \note Even if add_args_impl() were reimplemented, this method also should be
        implemented (e.g. with empty body).
    */
    virtual void set_option() = 0;

    /** Implementation of large().
    Default implementation returns false.
    */
    virtual bool large_impl() const;
};

/** Abstract base class for input argument.

\ingroup wbi
*/
class AbstractInput : public AbstractArgument {
public:
    /** Whether it is specified and valid */
    enum State {
        EMPTY, /**< Unspecified (validness depends on is_required()) */
        INVALID, /**< Specified and invalid */
        VALID /**< Specified and valid */
    };

    /** Constructor.
    \copydetails AbstractArgument()
    */
    AbstractInput(const std::string& option_name);

    /** Return internal WFormWidget.
    Reimplement form_widget_impl() virtual method.
    */
    WFormWidget* form_widget();

    /** \copydoc form_widget() */
    const WFormWidget* form_widget() const;

    /** Get if the input is required.
    \sa set_required
    */
    bool is_required() const {
        return required_;
    }

    /** Set if the input is required.
    If input is required but not provided (or provided incorrectly),
    a program is not started and error message is shown to the user.

    The default value is true.

    \see AbstractTask::run()
    */
    void set_required(bool value) {
        required_ = value;
    }

    /** Get error message.
    The helper message is displayed if the input value is not accepted.
    */
    const WString& error_message() {
        return error_message_;
    }

    /** Return the state of the argument.
    By default return AbstractInput::VALID.

    When reimplementing this method, use set_error_message() is the returned
    state is not VALID.
    */
    virtual State state() const;

    /** Return if the value of the input needs not to be fixed.
    This means \code VALID || (!is_required() && EMPTY) \endcode

    \see AbstractTask::run()
    */
    bool accepted() const;

protected:
    /** \copydoc form_widget()

    By default return 0.
    */
    virtual WFormWidget* form_widget_impl();

    /** Call the function for each of adding arguments, if state() == VALID */
    void add_args_impl(const ArgUser& f) const;

    /** Set error message */
    void set_error_message(const WString& error_message) const {
        error_message_ = error_message;
    }

private:
    bool required_;
    mutable WString error_message_;
};

/** Abstract input wrapping WFormWidget.

\ingroup wbi
*/
class FormWidgetInput : public AbstractInput {
public:
    /** Constructor.
    \copydetails AbstractInput()
    \param widget Widget to use as implementation.

    You can modify widget, i.e. by adding WValidator.
    */
    FormWidgetInput(WFormWidget* widget, const std::string& option_name);

    /** \copybrief AbstractInput::state().
    Return WFormWidget::validate().
    \note You can make empty value valid (WValidator::setMandatory(true))
    */
    State state() const;

protected:
    /** \copydoc form_widget()

    Widget passed to the constructor is returned.
    */
    WFormWidget* form_widget_impl();
};

/** Input argument using WLineEdit.

\ingroup wbi
*/
class LineEditInput : public FormWidgetInput {
public:
    /** Constructor.
    \copydetails FormWidgetInput()
    */
    LineEditInput(WLineEdit* widget, const std::string& option_name);

    /** Return line edit instance passed to constructor */
    WLineEdit* line_edit();

    /** \copydoc line_edit() */
    const WLineEdit* line_edit() const;

protected:
    /** \copybrief AbstractArgument::set_option()
    Set WLineEdit::text() as an option value (UTF-8).
    */
    void set_option();
};

/** Input argument using WFileUpload.

\ingroup wbi
*/
class FileInput : public AbstractInput {
public:
    /** Constructor.
    \copydetails AbstractInput()
    */
    FileInput(const std::string& option_name);

    State state() const;

    /** Get file upload */
    WFileUpload* file_upload() {
        return file_upload_;
    }

protected:
    /** Implementation widget */
    WContainerWidget* impl_;

    /** File upload widget */
    WFileUpload* file_upload_;

    /** \copybrief AbstractArgument::set_option()
    Set uploaded file name.
    */
    void set_option();

private:
    bool too_large_;

    void set_too_large(bool value) {
        too_large_ = value;
    }
};

/** Input argument using WFileUpload and WTextArea.
This input allows the user to select file using WFileUpload or
enter text using WTextArea.
The contents of the file is loaded to WTextArea.

\ingroup wbi
*/
class TextFileInput : public FileInput {
public:
    /** Constructor.
    \copydetails FileInput()
    */
    TextFileInput(const std::string& option_name);

    State state() const;

    /** Get text area */
    WTextArea* text_area() {
        return text_area_;
    }

protected:
    /** Textarea widget to enter text */
    WTextArea* text_area_;

    /** \copydoc form_widget()

    Return the instance of WTextArea used.
    */
    WFormWidget* form_widget_impl();

    /** \copybrief AbstractArgument::set_option()
    Save contains of the WTextArea to the file, uploaded by WFileUpload
    and set the name of the file.
    */
    void set_option();

    /** Return true, this input is large */
    bool large_impl() const;

private:
    void uploaded_handler();
    WText* error_;
};

/** Boolean input.
This input may pass option name and/or option value.

This is installed by four parameters:
 - name_if_true()
 - name_if_false()
 - value_if_true()
 - value_if_false()

\ingroup wbi
*/
class BoolInput : public FormWidgetInput {
public:
    /** Constructor.
    \param widget Widget to use as implementation
    \param name_if_true Option name to be passed if true (name_if_true())
    */
    BoolInput(WCheckBox* widget, const std::string& name_if_true);

    /** Constructor.
    \param widget Widget to use as implementation
    \param name_if_true Option name to be passed if true (name_if_true())
    \param name_if_false Option name to be passed if false (name_if_false())
    */
    BoolInput(WCheckBox* widget, const std::string& name_if_true,
              const std::string& name_if_false);

    /** Constructor.
    \param widget Widget to use as implementation
    \param name Option name to be passed (name_if_true() = name_if_false())
    \param value_if_true Option value to be passed if true (value_if_true())
    \param value_if_false Option value to be passed if false (value_if_false())
    */
    BoolInput(WCheckBox* widget, const std::string& name,
              const std::string& value_if_true,
              const std::string& value_if_false);

    /** Get the value to be set as option name, if true */
    const std::string& name_if_true() const {
        return name_if_true_;
    }

    /** Set the value to be set as option name, if true */
    void set_name_if_true(const std::string& v) {
        name_if_true_ = v;
    }

    /** Get the value to be set as option name, if false */
    const std::string& name_if_false() const {
        return name_if_false_;
    }

    /** Set the value to be set as option name, if false */
    void set_name_if_false(const std::string& v) {
        name_if_false_ = v;
    }

    /** Get the value to be set as option value, if true */
    const std::string& value_if_true() const {
        return value_if_true_;
    }

    /** Set the value to be set as option value, if true */
    void set_value_if_true(const std::string& v) {
        value_if_true_ = v;
    }

    /** Get the value to be set as option value, if false */
    const std::string& value_if_false() const {
        return value_if_false_;
    }

    /** Set the value to be set as option value, if false */
    void set_value_if_false(const std::string& v) {
        value_if_false_ = v;
    }

    /** Return check box instance passed to constructor */
    WCheckBox* check_box();

    /** \copydoc check_box() */
    const WCheckBox* check_box() const;

protected:
    /** \copybrief AbstractArgument::set_option()
    Add or not add argument
    */
    void set_option();

private:
    std::string name_if_true_;
    std::string name_if_false_;
    std::string value_if_true_;
    std::string value_if_false_;
};

/** Abstract base class for output argument.
\attention Do not call setImplementation() from descendants of this class.
    Use container() instead.

\ingroup wbi
*/
class AbstractOutput : public AbstractArgument {
public:
    /** Constructor.
    \copydetails AbstractArgument()
    */
    AbstractOutput(const std::string& option_name);

    /** Get whether the argument can be deselected.
    \sa set_selectable()
    */
    bool is_selectable() const {
        return selectable_;
    }

    /** Set whether the argument can be deselected.
    This adds a checkbox to the form.
    Using this checkbox the user can tell the form not to create this output.

    Do not use this with required outputs.

    The default value is true.
    */
    void set_selectable(bool value);

    /** Get if the checkbox is checked by default.
    \sa set_selected_by_default()
    */
    bool is_selected_by_default() const {
        return selected_by_default_;
    }

    /** Set if the checkbox is checked by default.
    This make sense only if is_selectable() == true.

    The default value is true.
    */
    void set_selected_by_default(bool value);

    /** Get if the checkbox is checked.
    This make sense only if is_selectable() == true.
    */
    bool is_selected() const {
        return selected_;
    }

    /** Return if the output is needed.
    Current implementation returns !is_selectable() || is_selected().
    */
    bool is_needed() const;

    /** This method is triggered when the program is finished.
    If not is_needed(), this method does nothing.
    The implementation of this method is finished_handler_impl()
    */
    void finished_handler();

protected:
    /** Implementation of finished_handler() */
    virtual void finished_handler_impl() = 0;

    /** Call the function for each of adding arguments, if is_needed() */
    void add_args_impl(const ArgUser& f) const;

    /** Implementation */
    WContainerWidget* container();

private:
    bool selectable_;
    bool selected_by_default_;
    bool selected_;

    void select_handler();
    void update_checkbox();
};

/** Output file argument.
This argument allows the user to download the file.

\ingroup wbi
*/
class FileOutput : public AbstractOutput {
public:
    /** Generator of file name to be used */
    typedef boost::function<std::string()> NameGen;

    /** Constructor.
    \copydetails AbstractOutput()
    \param temp_gen Temporary file name generator.
        This file is tried to be deleted in destructor.
        The function is called at most once by one FileOutput instance.
    \param download_mime Mime-type for a downloaded file
    */
    FileOutput(const std::string& option_name,
               const NameGen& temp_gen = &FileOutput::unique_name,
               const std::string& download_mime = "application/octet-stream");

    /** Destructor.
    Try to delete temp_file().
    */
    virtual ~FileOutput();

    /** Get temporary file name */
    const std::string& temp_file() const;

    /** Get mime-type for a downloaded file */
    const std::string& download_mime() const {
        return download_mime_;
    }

    /** Set generator of a suggested name of a downloaded file */
    void set_suggested_gen(const NameGen& value) {
        suggested_gen_ = value;
    }

    /** Return suggested name of a downloaded file */
    std::string suggested_name() const;

    /** Return unique temp file name.
    unique_filename() is used.
    */
    static std::string unique_name();

    /** Return not-dir part of temp_file() */
    std::string temp_notdir() const;

protected:
    /** \copybrief AbstractArgument::set_option()
    Create a file with a name provided by NameGen passed to the constructor
    and set it's name to option value.
    */
    void set_option();

    /** Create a HTML reference downloading the file */
    void finished_handler_impl();

    /** Generate new anchor to be shown */
    WAnchor* anchor() const;

private:
    NameGen temp_gen_;
    NameGen suggested_gen_;
    mutable std::string temp_file_;
    std::string download_mime_;
};

/** Output file argument with View feature.
This argument allows the user to download and view the file.
File is opened in a new tab of a web-browser.

\ingroup wbi
*/
class ViewFileOutput : public FileOutput {
public:
    /** Constructor.
    \copydetails FileOutput()
    \param view_mime Mime-type for a viewed file
    */
    ViewFileOutput(const std::string& option_name,
                   const NameGen& temp_gen = &FileOutput::unique_name,
                   const std::string& download_mime =
                       "application/octet-stream",
                   const std::string& view_mime = "text/plain");

    /** Get mime-type for a viewed file */
    const std::string& view_mime() const {
        return view_mime_;
    }

protected:
    /** Create a HTML references downloading and viewing the file in new tab */
    void finished_handler_impl();

private:
    std::string view_mime_;
};

/** Execution state */
enum RunState {
    UNSET, /**< Runner is not \ref AbstractTask::set_runner "set" to task */
    NEW, /**< Not started yet */
    QUEUED, /**< Queued */
    WORKING, /**< Working */
    FINISHED /**< Finished */
};

/** Abstract base class of form for web-based interface of a program.

\note This method must be called from a code, where wApp macro is defined.

\section CSS
 - Message (error or warning): wc_task_message
 - State (working, finished etc): wc_wbi_state

\ingroup wbi
*/
class AbstractTask : public WCompositeWidget {
public:
    /** Signal emitted after task state changing */
    typedef Signal<> ChangedSignal;

    /** Validator functor */
    typedef boost::function<bool()> Validator;

    /** Constructor */
    AbstractTask(WContainerWidget* p = 0);

    /** Add input argument to the program.
    Argument is added to the vector of arguments which is used by visit_args().

    Form takes ownership of the argument.

    This method calls add_input_impl().
    */
    void add_input(AbstractInput* input, const WString& name,
                   const WString& description = "");

    /** Add output argument to the program.
    Argument is added to the vector of arguments which is used by visit_args().

    Form takes ownership of the argument.

    This method calls add_output_impl().
    */
    void add_output(AbstractOutput* output, const WString& name,
                    const WString& description = "");

    /** Set the program runner.

    Form takes ownership of the runner.
    Previous runner is deleted.
    */
    void set_runner(AbstractRunner* runner);

    /** Set the task queue.
    State must be UNSET or NEW.

    Ownership is not transferred.
    */
    void set_queue(AbstractQueue* queue);

    /** Run a program.
    It is checked, that all input arguments are
    \ref AbstractInput::accepted() "accepted".
    Error messages are updated using update_error_message().
    If any of input arguments is not accepted, the function is interrupted.

    Call AbstractRunner::run() if task runner \ref set_runner "is set".
    If \ref set_queue() "a queue was set",
    the task is added to the queue instead.

    Implementing this class, connect .clicked() of Start button to this method
    */
    void run();

    /** Cancel a task.
     - the program is stopped
     - changed() signal is emitted

    Implementing this class, connect .clicked() of Cancel button to this method
    */
    void cancel();

    /** Apply the function to all arguments.
    This method calls \ref AbstractArgument::add_args "add_args(f)"
    of each argument.
    */
    void visit_args(const AbstractArgument::ArgUser& f) const;

    /** Return signal emitted after task state changing.
    \note You need not connect this signal to triggerUpdate(),
        since triggerUpdate() is called through WServer::post() after
        treating of other connected slots.
    */
    ChangedSignal& changed() {
        return changed_;
    }

    /** Get state.
    If runner is set, its state is returned, otherwise UNSET.
    */
    RunState state() const;

    /** Return i18n message id for the state */
    static const char* state_to_string(RunState state);

    /** \copybrief state_to_string(RunState) */
    const char* state_to_string();

    /** Set validator.
    Validator is applied to check validness of set of agruments.
    Unless it returns true, task is interrupted.
    Inputs should be also \ref AbstractInput::accepted() "accepted".

    This can be used to implement complex validation, for example,
    if one (not necessary all) of the input arguments is needed.

    If error or warning has occured, call set_message().
    */
    void set_validator(const Validator& validator) {
        validator_ = validator;
    }

    /** Set message (error or warning).
    The message is shown to user.

    This method is implemented in set_message_impl().
    */
    void set_message(const WString& message);

protected:
    /** Runner running the task */
    AbstractRunner* runner_;

    /** Implementation of add_input() */
    virtual void add_input_impl(AbstractInput* input, const WString& name,
                                const WString& description = "") = 0;

    /** Implementation of add_output() */
    virtual void add_output_impl(AbstractOutput* output, const WString& name,
                                 const WString& description = "") = 0;

    /** Update (display/hide) widgets representing error message for the input.
    Default implementation does nothing.

    \see AbstractInput::error_message(), AbstractInput::accepted()
    */
    virtual void update_error_message(AbstractInput* input);

    /** Update error messages of inputs and return if they are all accepted */
    bool check_inputs();

    /** Update error messages and return if the task is accepted.
    This implies check_inputs().
    */
    bool check_task();

    /** Set message (error or warning) (implementation).
    The message is shown to user.

    By default, does nothing.
    */
    virtual void set_message_impl(const WString& message);

private:
    ChangedSignal changed_;
    std::vector<AbstractArgument*> args_;
    AbstractQueue* queue_;
    bool queued_;
    Validator validator_;
    boost::function<void()> bound_trigger_updates_;

    void changed_emitter();
    void run_impl(bool check);
    void trigger_updates();

    friend class AbstractRunner;
    friend class AbstractQueue;
};

/** Task form implementation using TableForm.

\ingroup wbi
*/
class TableTask : public AbstractTask {
public:
    /** Constructor */
    TableTask(WContainerWidget* p = 0);

protected:
    void add_input_impl(AbstractInput* input, const WString& name,
                        const WString& description = "");

    void add_output_impl(AbstractOutput* output, const WString& name,
                         const WString& description = "");

    void update_error_message(AbstractInput* input);

    void set_message_impl(const WString& message);

private:
    void changed_handler();
};

/** Abstract base class for runner of a program.
Destructor of the class should kill working treads/processes if any.

\ingroup wbi
*/
class AbstractRunner : public WObject {
public:
    /** Constructor.
    A runner must be created from a code, where wApp macro is defined.
    */
    AbstractRunner();

    /** Destructor.
    Try to remove the task from the queue.
    */
    ~AbstractRunner();

    /** Get state */
    RunState state() const;

protected:
    /** Method to be called when the program is finished.
     - change the state() to FINISHED,
     - call AbstractQueue::remove() if needed,
     - call AbstractOutput::finished_handler() through WServer::post(),
     - emit AbstractTask::changed() through WServer::post().
    \note Thread-safe method
    */
    void finish();

    /** Set state */
    void set_state(RunState v) {
        state_ = v;
    }

    /** Get task */
    AbstractTask* task() {
        return task_;
    }

    /** Get task */
    const AbstractTask* task() const {
        return task_;
    }

    /** Run a program.
    This method should return immediately.
     - If state is UNSET or WORKING, do nothing;
     - call run_impl()

    This method should be only called by AbstractTask.
    */
    void run();

    /** Cancel running program.
     - if state is WORKING, the program is stopped;
     - try to remove the task from the queue;
     - state is set to NEW.

    This method should be only called by AbstractTask and AbstractRunner.
    */
    void cancel();

    /** Implementation of run().
    This method should return immediately.
     - if state is FINISHED, this should firstly make it NEW
       (not only set state=NEW, but also prepare the object, if needed);
     - then the state is set to WORKING;
     - start the program;
     - when the program is finished, finish() should be called.
    */
    virtual void run_impl() = 0;

    /** Implementation of cancel().
    This method should return immediately.
    */
    virtual void cancel_impl() = 0;

    /** Try to remove the task from the queue */
    void remove_from_queue();

private:
    RunState state_;
    AbstractTask* task_;
    boost::function<void()> bound_finished_handler_;

    void set_task(AbstractTask* task);
    void finished_handler();

    friend class AbstractTask;
};

/** Task runner, starting a waiting thread and a process.

Currently it is implemented using system() function.
Uses pkill command and $$ shell expression.
Command "bash -c" is used as "sh -c".
To create temporary pid file, unique_name() is used.
It seems to work only under UNIX.

\ingroup wbi
*/
class ForkingRunner : public AbstractRunner {
public:
    /** Constructor.
    \param command Command to use (part of command preceding the arguments)
    \param suffix  Part of command following the arguments
    */
    ForkingRunner(const std::string& command, const std::string& suffix = "");

    /** Destructor.
     - If state is WORKING, call cancel_impl()
     - remove pid file.
    */
    ~ForkingRunner();

    /** Escape an argument to be used as a shell argument.
    Add single quotes around an argument and
    quotes/escapes any existing single quotes.
    */
    static std::string escape_arg(const std::string& arg);

    /** Get the signal, sent to the task to cancel it */
    int signal() const {
        return signal_;
    }

    /** Set the signal, sent to the task to cancel it.
    Defaults to 9 (SIGKILL).
    */
    void set_signal(int signal) {
        signal_ = signal;
    }

protected:
    void run_impl();
    void cancel_impl();

private:
    std::string command_;
    std::string suffix_;
    std::string pid_file_;
    boost::thread thread_;
    int signal_;

    std::string command() const;
    void start_process(std::string cmd);
};

/** Queue controlling tasks.
The instance of this class should be created once per WServer.
Each task should be connected to this queue.

\ingroup wbi
*/
class AbstractQueue : public WObject {
public:
    /** Constructor */
    AbstractQueue(WObject* p = 0);

    /** Add new task to queue.
    This method should be called from the session of the task.
    The implementation of this method is add_impl().

    \note This method must be called from a code, where wApp macro is defined.
    */
    void add(AbstractTask* task);

    /** Remove the task from queue.
    The implementation of this method is remove_impl().
    */
    void remove(AbstractTask* task);

protected:
    /** Mutex.
    This mutex is kept by all public methods of the class.
    Inheriting from this class, take care of all code being under this mutex.
    */
    boost::mutex mutex_;

    /** Implementation of add() */
    virtual void add_impl(AbstractTask* task) = 0;

    /** Implementation of remove().
    This method is called when the task is finished or canceled.
    The task may be in queue or running.
    */
    virtual void remove_impl(AbstractTask* task) = 0;

    /** Run the task in the session of the task */
    void run_task(AbstractTask* task);

    /** Set if task is queued.
    This method changes task internal state.
    */
    static void set_queued(AbstractTask* task, bool queued = true) {
        task->queued_ = queued;
    }

private:
    /** Map task to session id.
    Entry is added before calling add_impl()
    and removed after calling remove_impl().
    */
    std::map<AbstractTask*, boost::function<void()> > task2run_;
};

/** Queue controlling the number of tasks running at the same time.

\ingroup wbi
*/
class TaskNumberQueue : public AbstractQueue {
public:
    /** Constructor.
    \param max_tasks The maximum number of tasks running at the same time.
        If set to -1, this means infinite number.
    \param p Parent object
    */
    TaskNumberQueue(int max_tasks, WObject* p = 0);

protected:
    void add_impl(AbstractTask* task);

    void remove_impl(AbstractTask* task);

private:
    int max_tasks_;
    std::list<AbstractTask*> waiting_;
    std::set<AbstractTask*> running_;

    void try_to_run();
};

/** A count-up, displaying the time, spent by the task.

\ingroup wbi
\ingroup time
*/
class TaskCountup : public Countdown {
public:
    /** Constructor */
    TaskCountup(AbstractTask* task, WContainerWidget* parent = 0);

private:
    AbstractTask* task_;
    RunState prev_state_;

    void changed_handler();
};

}

}

#endif

