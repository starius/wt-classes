/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WFileResource>
#include <Wt/Wc/SWFStore.hpp>

using namespace Wt;
using namespace Wt::Wc;

class SWFStoreApp : public WApplication {
public:
    SWFStoreApp(const WEnvironment& env):
        WApplication(env) {
        SWFStore* swf = new SWFStore(root());
        WPushButton* clear_storage = new WPushButton("Clear", root());
        WPushButton* one = new WPushButton("Set k=1", root());
        WPushButton* two = new WPushButton("Set k=2", root());
        WPushButton* get = new WPushButton("Get k", root());
        WText* k = new WText(root());
        clear_storage->clicked().connect(swf, &SWFStore::clear_storage);
        one->clicked().connect(boost::bind(&SWFStore::set_item, swf, "k", "1"));
        two->clicked().connect(boost::bind(&SWFStore::set_item, swf, "k", "2"));
        get->clicked().connect(boost::bind(&SWFStore::get_value_of,
                                           swf, "k", "nothing"));
        swf->value().connect(boost::bind(&WText::setText, k, _2));
    }
};

WApplication* createApplication(const WEnvironment& env) {
    return new SWFStoreApp(env);
}

// FIXME Debian hardcode!!
const char* SWFSTORE_FILE = "/usr/share/javascript/yui/swfstore/swfstore.swf";

class SWFStoreServer : public WServer {
public:
    SWFStoreServer():
        swfstore_("application/x-shockwave-flash", SWFSTORE_FILE) {
        addResource(&swfstore_, "/swfstore.swf");
        addResource(&storage_whitelist_, "/storage-whitelist.xml");
        addEntryPoint(Wt::Application, createApplication);
    }

private:
    WFileResource swfstore_;
    WFileResource storage_whitelist_; // 404 Not Found
};

int main(int argc, char** argv) {
    SWFStoreServer server;
    server.setServerConfiguration(argc, argv);
    if (server.start()) {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}

