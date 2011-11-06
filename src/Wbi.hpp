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

/** Abstract base class for any command-line argument of a program */
class AbstractArgument : public WCompositeWidget {
public:
    /** Constructor.
    \param option_name Name of a program option (i.e., "-i", "--output").
    */
    AbstractArgument(const std::string& option_name="");

    /** Add arguments to the vector of arguments.
    Default implementation adds option_name_ and option_value_.
    If option name or value is an empty string, it is skipped.

    \note Reimplement this method if the number of added arguments
        is not equal to 2 (name and value).
        Examples: argument lists; switches (flags).
    */
    virtual void add_args(std::vector<std::string>& args);

protected:
    /** Name of a program option (i.e., "-i", "--output") */
    std::string option_name_;

    /** Value of a program option (i.e., "123", "in.txt", "out.pdf") */
    std::string option_value_;

    /** Set option value.
    You should implement this method in descendant classes.
    This method should get information from widgets and
    set correct value of option_value_.
    \note This method is used by default implementation of add_args().
        In case you reimplemented add_args(), you should implement
        this method with empty body (doing nothing).
    */
    virtual void set_option()=0;
};

/** Abstract base class for input argument */
class AbstractInput : public AbstractArgument {
public:
    /** Constructor.
    \copydetails AbstractArgument()
    */
    AbstractInput(const std::string& option_name);

    /** Return internal WFormWidget.
    By default return 0
    */
    virtual WFormWidget* form_widget() const;

    /** Get if the input is required.
    \sa set_required
    */
    bool is_required() const {
        return required_;
    }

    /** Set if the input is required.
    If input is required but not provided (or provided incorrectly),
    a program is not started and error message is shown to the user.
    This is detected as follows: if size of args vector is not
    changed by add_args(), the input is considered not to be provided.
    \note All switches must not be required.

    The default value is true.
    */
    void set_required(bool value) {
        required_ = value;
    }

private:
    bool required_;
};

/** Abstract input wrapping WFormWidget */
class FormWidgetInput : public AbstractInput {
public:
    /** Constructor.
    \copydetails AbstractInput()
    \param widget Widget to use as implementation.

    You can change widget, i.e. by adding WValidator.
    */
    FormWidgetInput(WFormWidget* widget, const std::string& option_name);

    /** Return internal WFormWidget.
    Widget passed to the constructor is returned.
    */
    WFormWidget* form_widget() const;
};

/** Input argument using WLineEdit */
class LineEditInput : public FormWidgetInput {
public:
    /** Constructor.
    \copydetails FormWidgetInput()
    */
    LineEditInput(WLineEdit* widget, const std::string& option_name);

    /** Return line edit instance passed to constructor */
    WLineEdit* line_edit() const;

protected:
    /** Set option value.
    Set WLineEdit::text() as an option value.
    */
    void set_option();
};

/** Input argument using WFileUpload */
class FileInput : public AbstractInput {
public:
    /** Constructor.
    \copydetails AbstractInput()
    */
    FileInput(const std::string& option_name);

protected:
    /** File upload widget */
    WFileUpload* file_upload_;

    /** Set option value.
    Set uploaded file name.
    */
    void set_option();

    /** File uploaded handler */
    virtual void file_uploaded_handler();
};

/** Input argument using WFileUpload and WTextArea.
This input allows the user to select file using WFileUpload or
enter text using WTextArea.
The contents of the file is loaded to WTextArea.
*/
class TextFileInput : public FileInput {
public:
    /** Constructor.
    \copydetails FileInput()
    */
    TextFileInput(const std::string& option_name);

    /** Return the instance of WTextArea used */
    WFormWidget* form_widget() const;

protected:
    /** Text are widget to enter text */
    WTextArea* text_area_;

    /** Set option value.
    Save contains of the WTextArea to the file, uploaded by WFileUpload
    and set the name of the file.
    */
    void set_option();

    void file_uploaded_handler();
};

/** Abstract base class for output argument */
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

    /** This method is triggered when the program is finished */
    void virtual task_finished_handler()=0;

private:
    bool selectable_;
    bool selected_by_default_;
    bool selected_;

    void select_handler_();
};

/** Output file argument.
This argument allows the user to download the file.
*/
class FileOutput : public AbstractOutput {
public:
    /** Constructor.
    \copydetails AbstractOutput()
    \param temp_dir Temporary directory to create a file
    \param download_mime Mime-type for a downloaded file
    */
    FileOutput(const std::string& temp_dir, const std::string& option_name,
               const std::string& download_mime="application/octet-stream");

    /** Get temponary directory to acreate a file */
    const std::string& temp_dir() const {
        return temp_dir_;
    }

    /** Get mime-type for a downloaded file */
    const std::string& download_mime() const {
        return download_mime_;
    }

protected:
    /** Set option value.
    Create a file with a ranmod name in the temp_dir
    and set it's name to option value.
    \note File as well as temp_dir are not removed by this class.
        Temporary directory should be removed latter by the form.
    */
    void set_option();

    /** Create a HTML reference downloading the file */
    void task_finished_handler();

private:
    std::string temp_dir_;
    std::string download_mime_;
};

/** Output file argument.
This argument allows the user to download and view the file.
File is openned in a new tab of a web-browser.
*/
class TextFileOutput : public FileOutput {
public:
    /** Constructor.
    \copydetails FileOutput()
    \param view_mime Mime-type for a viewed file
    */
    TextFileOutput(const std::string& temp_dir, const std::string& option_name,
                   const std::string& download_mime="application/octet-stream",
                   const std::string& view_mime="text/plain");

    /** Get mime-type for a viewed file */
    const std::string& view_mime() const {
        return view_mime_;
    }

protected:
    /** Create a HTML references downloading and viewing the file in new tab */
    void task_finished_handler();

private:
    std::string view_mime_;
};

/** Base class of form for web-based interface of a program */
class BaseForm : public WCompositeWidget {
public:
    /** Constructor */
    BaseForm(WContainerWidget* p=0);

    /** Add input agrument to the program */
    void add_input(AbstractInput* input, const WString& name,
            const WString& description="");

    /** Add output agrument to the program */
    void add_output(AbstractOutput* output, const WString& name,
            const WString& description="");

    /** Set the program runner */
    void set_runner(AbstractTaskRunner* runer);
};

/** Abstract base class for runner of a program */
class AbstractTaskRunner : public WObject {
public:
    /** Constructor */
    AbstractTaskRunner();

    /** Run a program */
    virtual void run(BaseForm* form)=0;

    /** Return if a program has finished */
    virtual bool is_finished()=0;
};

}

}

#endif

