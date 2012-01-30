/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#define BOOST_FILESYSTEM_VERSION 3

#include <cstdio>
#include <boost/filesystem.hpp>

#include <Wt/WApplication>

#include "Wbi.hpp" // FIXME
#include "FilterResource.hpp"

namespace Wt {

namespace Wc {

FilterResource::FilterResource(const WString& cmd, WObject* parent):
    WFileResource(parent), cmd_(cmd)
{ }

FilterResource::~FilterResource() {
    beingDeleted();
    remove(output_file_.c_str());
}

void FilterResource::handleRequest(const Http::Request& request,
                                   Http::Response& response) {
    using namespace boost::filesystem;
    mutex_.lock();
    if (output_file_.empty()) {
        std::string input_file = FileOutput::unique_name(); // FIXME
        output_file_ = FileOutput::unique_name(); // FIXME
        std::ofstream input(input_file.c_str());
        if (input.is_open()) {
            try {
                write_input(input);
                input.close();
                system(cmd_.arg(input_file).arg(output_file_).toUTF8().c_str());
                remove(input_file.c_str());
                if (exists(output_file_)) {
                    setFileName(output_file_);
                }
            } catch (std::exception& e) {
                wApp->log("warning") << "FilterResource: " << e.what();
            } catch (...) {
                wApp->log("warning") << "FilterResource: error";
            }
        }
    }
    mutex_.unlock();
    WFileResource::handleRequest(request, response);
}

}

}

