/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2013 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WImage>
#include <Wt/Http/Request>
#include <Wt/Http/Response>

#include "EtagStore.hpp"
#include "rand.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

EtagStoreResource::EtagStoreResource(const std::string& cookie_name,
                                     const std::string& send_header,
                                     const std::string& receive_header,
                                     WObject* parent):
    WResource(parent), cookie_name_(cookie_name),
    send_header_(send_header), receive_header_(receive_header) {
    setDispositionType(WResource::Inline);
}

static const char EMPTY_GIF[] = {
    0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00, 0x01, 0x00,
    0x80, 0x00, 0x00, 0xdb, 0xdf, 0xef, 0x00, 0x00, 0x00, 0x21,
    0xf9, 0x04, 0x01, 0x00, 0x00, 0x00, 0x00, 0x2c, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44,
    0x01, 0x00, 0x3b
};

const int EMPTY_GIF_SIZE = 43;

void EtagStoreResource::handleRequest(const Http::Request& request,
                                      Http::Response& response) {
    handle_etag(request, response);
    response.setMimeType("image/gif");
    response.addHeader("Content-Length", TO_S(EMPTY_GIF_SIZE));
    response.out().write(EMPTY_GIF, EMPTY_GIF_SIZE);
}

void EtagStoreResource::handle_etag(const Http::Request& request,
                                    Http::Response& response) {
    // TODO http://redmine.webtoolkit.eu/issues/2471
    // const std::string* cookie_value = request.getCookieValue(cookie_name_);
    std::string cookies = request.headerValue("Cookie");
    if (cookies.empty()) {
        return;
    }
    std::string pattern = cookie_name_ + "=";
    int cookie_begin = cookies.find(pattern);
    if (cookie_begin == std::string::npos) {
        return;
    }
    cookie_begin += pattern.length();
    int cookie_end = cookies.find(';', cookie_begin);
    int cookie_length = (cookie_end == -1) ? -1 : (cookie_end - cookie_begin);
    std::string cookie_value = cookies.substr(cookie_begin, cookie_length);
    //
    std::string etag_value = request.headerValue(receive_header());
    boost::mutex::scoped_lock lock(cookie_to_etag_mutex_);
    Map::iterator it = cookie_to_etag_.find(cookie_value);
    if (it == cookie_to_etag_.end()) {
        return;
    }
    Etag& etag = it->second;
    if (etag_value.empty()) {
        etag_value = etag.def;
    }
    etag.from_client = etag_value;
    if (!etag.to_client.empty()) {
        response.addHeader(send_header(), etag.to_client);
        etag.to_client.clear();
    } else {
        etag.handler(etag.from_client);
    }
    if (!etag.from_client.empty()) {
        response.addHeader(send_header(), etag.from_client);
    }
}

EtagStore::EtagStore(EtagStoreResource* resource, WContainerWidget* parent):
    AbstractStore(parent), resource_(resource) {
    cookie_value_ = rand_string();
    int day = 3600 * 24;
    wApp->setCookie(resource_->cookie_name(), cookie_value_, day);
    {
        boost::mutex::scoped_lock lock(resource_->cookie_to_etag_mutex_);
        typedef EtagStoreResource::Etag Etag;
        Etag& etag = resource_->cookie_to_etag_[cookie_value_];
        etag.handler = one_bound_post(boost::bind(&EtagStore::emit_value,
                                      this, _1));
    }
    resize(0, 0);
    wApp->enableUpdates();
}

EtagStore::~EtagStore() {
    boost::mutex::scoped_lock lock(resource_->cookie_to_etag_mutex_);
    resource_->cookie_to_etag_.erase(cookie_value_);
}

void EtagStore::clear_storage_impl() {
    set_item_impl(key_, "");
}

void EtagStore::set_item_impl(const std::string& key,
                              const std::string& value) {
    {
        boost::mutex::scoped_lock lock(resource_->cookie_to_etag_mutex_);
        typedef EtagStoreResource::Etag Etag;
        Etag& etag = resource_->cookie_to_etag_[cookie_value_];
        etag.to_client = value;
    }
    key_ = key;
    update_image();
}

void EtagStore::remove_item_impl(const std::string& key) {
    set_item_impl(key, "");
}

void EtagStore::get_value_of_impl(const std::string& key,
                                  const std::string& def) {
    {
        boost::mutex::scoped_lock lock(resource_->cookie_to_etag_mutex_);
        resource_->cookie_to_etag_[cookie_value_].def = def;
    }
    key_ = key;
    update_image();
}

void EtagStore::update_image() {
    clear();
    WImage* image = new WImage(resource_->url());
    addWidget(image);
    image->resize(0, 0);
    wApp->triggerUpdate();
}

void EtagStore::emit_value(const boost::any& result) {
    value().emit(key_, boost::any_cast<std::string>(result));
}

}

}

