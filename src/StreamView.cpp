/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <sstream>
#include <fstream>

#include <Wt/WText>
#include <Wt/WResource>

#include "StreamView.hpp"
#include "FileView.hpp"
#include "ResourceView.hpp"

namespace Wt {

namespace Wc {

StreamView::StreamView(std::istream* input_stream, WContainerWidget* parent):
    WViewWidget(parent),
    input_stream_(input_stream),
    encoding_(UTF8)
{ }

StreamView::StreamView(WContainerWidget* parent):
    WViewWidget(parent),
    input_stream_(0),
    encoding_(UTF8),
    format_(XHTMLText),
    word_wrap_(true)
{ }

void StreamView::set_stream(std::istream* input_stream) {
    input_stream_ = input_stream;
    update();
}

WWidget* StreamView::renderView() {
    return create_text(input_stream_);
}

WText* StreamView::create_text(std::istream* input_stream) const {
    std::string str;
    if (input_stream) {
        input_stream->seekg(0, std::ios::end);
        str.reserve(input_stream->tellg());
        input_stream->seekg(0, std::ios::beg);
        str.assign((std::istreambuf_iterator<char>(*input_stream)),
                   std::istreambuf_iterator<char>());
    }
    WText* result = new WText(WString(str, encoding_), format_);
    result->setWordWrap(word_wrap_);
    return result;
}

FileView::FileView(const std::string& filename, WContainerWidget* parent):
    StreamView(parent),
    filename_(filename)
{ }

FileView::FileView(WContainerWidget* parent):
    StreamView(parent)
{ }

void FileView::set_filename(const std::string& filename) {
    filename_ = filename;
    update();
}

WWidget* FileView::renderView() {
    std::fstream f(filename_.c_str());
    return create_text(filename_.empty() ? 0 : &f);
}

ResourceView::ResourceView(WResource* resource, WContainerWidget* parent):
    StreamView(parent),
    resource_(resource)
{ }

ResourceView::ResourceView(WContainerWidget* parent):
    StreamView(parent),
    resource_(0)
{ }

void ResourceView::set_resource(WResource* resource) {
    resource_ = resource;
    update();
}

WWidget* ResourceView::renderView() {
    std::stringstream ss;
    if (resource_) {
        resource_->write(ss);
    }
    return create_text(&ss);
}

}

}

