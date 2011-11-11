/**
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#define BOOST_FILESYSTEM_VERSION 3

#include <cstdio>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WCompositeWidget>
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

#include "Wbi.hpp"
#include "TableForm.hpp"

namespace Wt {

namespace Wc {

AbstractArgument::AbstractArgument(const std::string& option_name):
    option_name_(option_name)
{ }

void AbstractArgument::add_args(const ArgUser& f) {
    set_option();
    if (!option_name_.empty()) {
        f(option_name_);
    }
    if (!option_value_.empty()) {
        f(option_value_);
    }
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

WFormWidget* AbstractInput::form_widget_impl() {
    return 0;
}

bool AbstractInput::is_valid() const {
    return true;
}

void AbstractInput::add_args(const ArgUser& f) {
    if (is_valid()) {
        AbstractArgument::add_args(f);
    }
}

FormWidgetInput::FormWidgetInput(WFormWidget* widget,
                                 const std::string& option_name):
    AbstractInput(option_name) {
    setImplementation(widget);
}

WFormWidget* FormWidgetInput::form_widget_impl() {
    return static_cast<WFormWidget*>(implementation());
}

bool FormWidgetInput::is_valid() const {
    WFormWidget* fw = const_cast<WFormWidget*>(form_widget()); // FIXME
    return fw->validate() == WValidator::Valid;
}

LineEditInput::LineEditInput(WLineEdit* widget, const std::string& option_name):
    FormWidgetInput(widget, option_name)
{ }

WLineEdit* LineEditInput::line_edit() {
    return static_cast<WLineEdit*>(form_widget());
}

const WLineEdit* LineEditInput::line_edit() const {
    return static_cast<const WLineEdit*>(form_widget());
}

void LineEditInput::set_option() {
    option_value_ = line_edit()->text().toUTF8();
};

FileInput::FileInput(const std::string& option_name):
    AbstractInput(option_name) {
    impl_ = new WContainerWidget();
    file_upload_ = new WFileUpload(impl_);
    file_upload_->changed().connect(file_upload_, &WFileUpload::upload);
    setImplementation(impl_);
}

void FileInput::set_option() {
    option_value_ = file_upload_->spoolFileName();
}

bool FileInput::is_valid() const {
    return !file_upload_->empty();
}

TextFileInput::TextFileInput(const std::string& option_name):
    FileInput(option_name) {
    new WBreak(impl_);
    text_area_ = new WTextArea(impl_);
    file_upload_->uploaded().connect(this, &TextFileInput::uploaded_handler);
    // TODO File too large
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
    std::ofstream file(filename.c_str());
    file << text_area_->text().toUTF8();
    file.close();
    option_value_ = filename;
}

BoolInput::BoolInput(const std::string& name_if_true, bool checked):
    AbstractInput(""),
    name_if_true_(name_if_true) {
    initialize(checked);
}

BoolInput::BoolInput(const std::string& name_if_true,
                     const std::string& name_if_false, bool checked):
    AbstractInput(""),
    name_if_true_(name_if_true), name_if_false_(name_if_false) {
    initialize(checked);
}

BoolInput::BoolInput(const std::string& name,
                     const std::string& value_if_true,
                     const std::string& value_if_false, bool checked):
    AbstractInput(""),
    name_if_true_(name), name_if_false_(name),
    value_if_true_(value_if_true), value_if_false_(value_if_false) {
    initialize(checked);
}

void BoolInput::set_option() {
    if (impl_->isChecked()) {
        option_name_ = name_if_true_;
        option_value_ = value_if_true_;
    } else {
        option_name_ = name_if_false_;
        option_value_ = value_if_false_;
    }
}

void BoolInput::initialize(bool checked) {
    setImplementation(impl_ = new WCheckBox());
    impl_->setChecked(checked);
}

AbstractOutput::AbstractOutput(const std::string& option_name):
    AbstractArgument(option_name),
    selectable_(true), selected_by_default_(true),
    selected_(selected_by_default_) {
    WCheckBox* box = new WCheckBox();
    box->setChecked(selected_by_default_);
    box->setEnabled(selectable_);
    if (selectable_) {
        box->changed().connect(this, &AbstractOutput::select_handler);
    }
    setImplementation(box);
}

bool AbstractOutput::is_needed() const {
    return !is_selectable() || is_selected();
}

void AbstractOutput::select_handler() {
    WCheckBox* box = static_cast<WCheckBox*>(sender());
    selected_ = box->isChecked();
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
    return boost::filesystem::unique_path().string();
}

std::string FileOutput::temp_notdir() const {
    return boost::filesystem::path(temp_file()).filename().string();
}

void FileOutput::set_option() {
    option_value_ = temp_file();
}

void FileOutput::task_finished_handler() {
    setImplementation(anchor());
}

WAnchor* FileOutput::anchor() const {
    WFileResource* r = new WFileResource(download_mime_, temp_file());
    r->suggestFileName(suggested_name());
    return new WAnchor(r, tr("wc.Download"));
}

ViewFileOutput::ViewFileOutput(const std::string& option_name,
                               const NameGen& temp_gen,
                               const std::string& download_mime,
                               const std::string& view_mime):
    FileOutput(option_name, temp_gen, download_mime),
    view_mime_(view_mime)
{ }

void ViewFileOutput::task_finished_handler() {
    WFileResource* r = new WFileResource(view_mime_, temp_file());
    WAnchor* a = new WAnchor(r, tr("wc.View"));
    WContainerWidget* c = new WContainerWidget();
    c->addWidget(FileOutput::anchor());
    c->addWidget(new WText(" | "));
    c->addWidget(a);
    setImplementation(c);
}

AbstractTaskRunner::AbstractTaskRunner():
    state_(NEW)
{ }

void AbstractTaskRunner::finish() {
    state_ = FINISHED;
    // TODO use helper function emitter
    WServer::instance()->post(wApp->sessionId(),
                              boost::bind(&AbstractTaskRunner::emit,
                                          this));
}

void AbstractTaskRunner::emit() const {
    finished_signal_.emit();
}

}

}

