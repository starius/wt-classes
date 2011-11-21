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
#include <list>
#include <set>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "global.hpp"

namespace Wt {

namespace Wc {

/** \defgroup wbi Web-based interface of a program
Classes to simplify creation of a WebUI for programs
running from command line.

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
    void add_args(const ArgUser& f);

protected:
    /** Name of a program option (i.e., "-i", "--output") */
    std::string option_name_;

    /** Value of a program option (i.e., "123", "in.txt", "out.pdf") */
    std::string option_value_;

    /** Call the function for each of adding arguments (implementation).
    Default implementation adds option_name_ and option_value_.
    If option name or value is an empty string, it is skipped.

    \note Reimplement this method if the number of added arguments
        is more than 2 (e.g. variadic argument list).
    */
    virtual void add_args_impl(const ArgUser& f);

    /** Call the function for option_name_ and option_value_.
    If option name or value is an empty string, it is skipped.
    */
    void add_option(const ArgUser& f);

    /** Set option value.
    You should implement this method in descendant classes.
    This method should get information from widgets and
    set correct value of option_value_.
    \note Even if add_args_impl() were reimplemented, this method also should be
        implemented (e.g. with with empty body).
    */
    virtual void set_option() = 0;
};

/** Abstract base class for input argument.

\ingroup wbi
*/
class AbstractInput : public AbstractArgument {
public:
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

    \todo This is detected as follows: if the function, passed to add_args()
    has not been called, the input is considered not to be provided.

    \note All switches must not be required.

    The default value is true.
    */
    void set_required(bool value) {
        required_ = value;
    }

protected:
    /** \copydoc form_widget()

    By default return 0.
    */
    virtual WFormWidget* form_widget_impl();

    /** Return if the value of the input is valid.
    By default return true.
    */
    virtual bool is_valid() const;

    /** Call the function for each of adding arguments, if is_valid() */
    void add_args_impl(const ArgUser& f);

private:
    bool required_;
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

protected:
    /** \copydoc form_widget()

    Widget passed to the constructor is returned.
    */
    WFormWidget* form_widget_impl();

    /** \copybrief AbstractInput::is_valid().
    Check WFormWidget::validate() == Valid.
    \note Empty value is not considered to be valid.
    */
    bool is_valid() const;
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

protected:
    /** Implementation widget */
    WContainerWidget* impl_;

    /** File upload widget */
    WFileUpload* file_upload_;

    /** \copybrief AbstractArgument::set_option()
    Set uploaded file name.
    */
    void set_option();

    bool is_valid() const;
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

protected:
    /** Text are widget to enter text */
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

private:
    void uploaded_handler();
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
class BoolInput : public AbstractInput {
public:
    /** Constructor.
    \param name_if_true Option name to be passed if true (name_if_true())
    \param checked If the checkbox is checked by default
    */
    BoolInput(const std::string& name_if_true, bool checked = true);

    /** Constructor.
    \param name_if_true Option name to be passed if true (name_if_true())
    \param name_if_false Option name to be passed if false (name_if_false())
    \param checked If the checkbox is checked by default
    */
    BoolInput(const std::string& name_if_true,
              const std::string& name_if_false, bool checked = true);

    /** Constructor.
    \param name Option name to be passed (name_if_true() = name_if_false())
    \param value_if_true Option value to be passed if true (value_if_true())
    \param value_if_false Option value to be passed if false (value_if_false())
    \param checked If the checkbox is checked by default
    */
    BoolInput(const std::string& name,
              const std::string& value_if_true,
              const std::string& value_if_false, bool checked = true);

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

protected:
    /** Implementation */
    WCheckBox* impl_;

    /** \copybrief AbstractArgument::set_option()
    Add or not add argument
    */
    void set_option();

private:
    std::string name_if_true_;
    std::string name_if_false_;
    std::string value_if_true_;
    std::string value_if_false_;

    void initialize(bool checked);
};

/** Abstract base class for output argument.
\attention Do call setImplementation() from descriptors of this class.
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
    void set_selectable(bool value) {
        selectable_ = value;
    }

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
    void set_selected_by_default(bool value) {
        selected_by_default_ = value;
    }

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
    The implementation of this method is finished_handler_impl()
    */
    void finished_handler();

protected:
    /** Implementation of finished_handler() */
    virtual void finished_handler_impl() = 0;

    /** Implementation */
    WContainerWidget* container();

private:
    bool selectable_;
    bool selected_by_default_;
    bool selected_;

    void select_handler();
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

    /** Get temponary file name */
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
    Boost.Filesystem's unique_path is used
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

/** Output file argument.
This argument allows the user to download and view the file.
File is openned in a new tab of a web-browser.

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
                   const std::string& download_mime = "application/octet-stream",
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
    UNSET, /** Runner is not \ref AbstractTask::set_runner "set" to task */
    NEW, /**< Not started yet */
    QUEUED, /**< Queued */
    WORKING, /**< Working */
    FINISHED /**< Finished */
};

/** Abstract base class of form for web-based interface of a program.

\ingroup wbi
*/
class AbstractTask : public WCompositeWidget {
public:
    /** Signal emitted after task state changing */
    typedef Signal<> ChangedSignal;

    /** Constructor */
    AbstractTask(WContainerWidget* p = 0);

    /** Add input agrument to the program.
    Argument is added to the vector of arguments which is used by visit_args().

    This method calls add_input_impl().
    */
    void add_input(AbstractInput* input, const WString& name,
                   const WString& description = "");

    /** Add output agrument to the program.
    Argument is added to the vector of arguments which is used by visit_args().

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
    */
    void set_queue(AbstractQueue* queue);

    /** Run a program.
    Call AbstractRunner::run() if task runner \ref set_runner "is set".

    Implementing this class, connect .clicked() of Start button to this method
    */
    void run();

    /** Apply the function to all arguments.
    This method calls \ref AbstractArgument::add_args "add_args(f)"
    of each argument.
    */
    void visit_args(const AbstractArgument::ArgUser& f);

    /** Return signal emitted after task state changing.
    \note Connect this signal to WApplication::triggerUpdate()
    */
    ChangedSignal& changed() {
        return changed_;
    }

    /** Get state.
    If runner is set, its state is returned, otherwise UNSET.
    */
    RunState state() const;

protected:
    /** Runner running the task */
    AbstractRunner* runner_;

    /** Implementation of add_input() */
    virtual void add_input_impl(AbstractInput* input, const WString& name,
                                const WString& description = "") = 0;

    /** Implementation of add_output() */
    virtual void add_output_impl(AbstractOutput* output, const WString& name,
                                 const WString& description = "") = 0;

private:
    ChangedSignal changed_;
    std::vector<AbstractArgument*> args_;
    AbstractQueue* queue_;
    bool queued_;

    void changed_emitter();
    void run_impl();

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
    /** Container for inputs */
    TableForm* inputs_;

    /** Container for outputs */
    TableForm* outputs_;

    void add_input_impl(AbstractInput* input, const WString& name,
                        const WString& description = "");

    void add_output_impl(AbstractOutput* output, const WString& name,
                         const WString& description = "");
};

/** Abstract base class for runner of a program.
Destructor of the class should kill working treads/processes if any.

\ingroup wbi
*/
class AbstractRunner : public WObject {
public:
    /** Constructor */
    AbstractRunner();

    /** Run a program.
    This method should return immediately.
     - If state is UNSET or WORKING, do nothing;
     - call run_impl()
    */
    void run();

    /** Get state */
    RunState state() const;

    /** Set server.
    The server is used to perform WServer::post().

    Default value is WServer::instance() calculated once, by the constructor.
    */
    void set_server(WServer* server) {
        server_ = server;
    }

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

    /** Implementation of run().
    This method should return immediately.
     - if state is FINISHED, this should firstly make it NEW
       (not only set state=NEW, but also prepare the object, if needed);
     - then the state is set to WORKING;
     - start the program;
     - when the program is finished, finish() should be called.
    */
    virtual void run_impl() = 0;

private:
    RunState state_;
    AbstractTask* task_;
    WServer* server_;
    std::string session_id_;

    void emit() const;

    void set_task(AbstractTask* task);

    friend class AbstractTask;
};

/** Task runner, starting a waiting thread and a process.

\ingroup wbi
*/
class ForkingRunner : public AbstractRunner {
public:
    /** Constructor.
    \param command Command to use (program name)
    */
    ForkingRunner(const std::string& command);

    /** Destructor.
    \todo Should kill a thread and a process if any
    */
    ~ForkingRunner();

    /** Escape an argument to be used as a shell argument.
    Add single quotes around an argument and
    quotes/escapes any existing single quotes.
    */
    static std::string escape_arg(const std::string& arg);

protected:
    void run_impl();

private:
    std::string command_;

    void start_process();
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
    */
    void add(AbstractTask* task);

    /** Remove the task from queue.
    The implementation of this method is remove_impl().
    */
    void remove(AbstractTask* task);

    /** Set server.
    The server is used to perform WServer::post().

    If value was not set, WServer::instance() is called each time.
    */
    void set_server(WServer* server) {
        server_ = server;
    }

protected:
    /** Mutex.
    This mutex is kept by all public methods of the class.
    Inheriting from this class, take care of all code being under this mutex.
    */
    boost::mutex mutex_;

    /** Map task to session id.
    Entry is added before calling add_impl()
    and removed after calling remove_impl().
    */
    std::map<AbstractTask*, std::string> task2session_;

    /** Implementation of add() */
    virtual void add_impl(AbstractTask* task) = 0;

    /** Implementation of remove().
    This method is called when the task is finished or cancelled.
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
    WServer* server_;
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

}

}

#endif

