/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */


#include <Wt/WServer>
#include <Wt/WApplication>
#include <Wt/WFileResource>
#include <Wt/WLogger>
#include <Wt/Wc/SWFStore.hpp>
#include <Wt/Wc/Gather.hpp>

using namespace Wt;
using namespace Wt::Wc;

void explore(Gather::DataType type, const std::string& value) {
    wApp->log("info") << "Gathered: type=" << type << ", value=" << value;
} // gather

class SWFStoreApp : public WApplication {
public:
    SWFStoreApp(const WEnvironment& env):
        WApplication(env) {
        SWFStore* swf = new SWFStore(root());
        Gather* gather = new Gather(explore, this);
        gather->set_swfstore(swf);
    }
};

WApplication* createSWFStoreApp(const WEnvironment& env) {
    return new SWFStoreApp(env);
}

// FIXME Debian hardcode!!
const char* SWFSTORE_FILE = "/usr/share/javascript/yui/swfstore/swfstore.swf";

class SWFStoreServer : public WServer {
public:
    SWFStoreServer(int argc, char** argv):
        swfstore_("application/x-shockwave-flash", SWFSTORE_FILE),
        storage_whitelist_("", "") {
        setServerConfiguration(argc, argv);
        addResource(&swfstore_, "/swfstore.swf");
        addResource(&storage_whitelist_, "/storage-whitelist.xml");
        addEntryPoint(Wt::Application, createSWFStoreApp);
    }

private:
    WFileResource swfstore_;
    WFileResource storage_whitelist_; // 404 Not Found
};

int main(int argc, char** argv) {
    SWFStoreServer server(argc, argv);
    if (server.start()) {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}

