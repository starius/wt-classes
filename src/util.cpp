/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#define BOOST_FILESYSTEM_VERSION 3

#include <sstream>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <openssl/md5.h>
#include <curl/curl.h>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>

#include "util.hpp"

namespace Wt {

namespace Wc {

void post(WServer* server, const std::string& app,
          const boost::function<void()>& func) {
    server->post(app, func);
}

boost::function<void()> bound_post(boost::function<void()> func) {
    WServer* server = downcast<WServer*>(wApp->environment().server());
    return boost::bind(post, server, wApp->sessionId(), func);
}

void updates_trigger() {
    wApp->triggerUpdate();
}

void updates_poster(WServer* server, WApplication* app) {
    server->post(app->sessionId(), updates_trigger);
}

std::string unique_filename() {
    using namespace boost::filesystem;
    const char* const model = "wt-classes-%%%%-%%%%-%%%%-%%%%";
    return unique_path(temp_directory_path() / model).string();
}

std::string config_value(const std::string& name, const std::string& def) {
    std::string value = def;
    if (wApp) {
        wApp->readConfigurationProperty(name, value);
    }
    return value;
}

WDateTime now() {
    return WDateTime::currentDateTime();
}

std::string md5(const std::string& data) {
    const unsigned char* d;
    d = reinterpret_cast<const unsigned char*>(data.c_str());
    unsigned long n = data.size();
    unsigned char* digest = MD5(d, n, NULL);
    std::stringstream result;
    result << std::hex;
    result.width(2);
    result.fill('0');
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
        result.width(2);
        result << static_cast<unsigned int>(digest[i]);
    }
    return result.str();
}

std::string urlencode(const std::string& url) {
    std::string result;
    CURL* curl_handle = curl_easy_init();
    char* encoded = curl_easy_escape(curl_handle,
                                     url.c_str(), url.size());
    if (encoded) {
        result = encoded;
        curl_free(encoded);
    }
    curl_easy_cleanup(curl_handle);
    return result;
}

}

}

