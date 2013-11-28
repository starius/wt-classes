/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2013 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_ETAG_STORE_HPP_
#define WC_ETAG_STORE_HPP_

#include <map>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>

#include <Wt/WGlobal>
#include <Wt/WResource>
#include <Wt/WContainerWidget>

#include "AbstractStore.hpp"

namespace Wt {

namespace Wc {

/** A resource storing data on in browser cache (like cookies).
Should be used with EtagStore.
See EtagStore.
*/
class EtagStoreResource : public WResource {
public:
    /** Constructor */
    EtagStoreResource(const std::string& cookie_name = "wces",
                      WObject* parent = 0);

    /** Handles a request */
    void handleRequest(const Http::Request& request, Http::Response& response);

    /** Return name of cookie used to distinguish clients */
    const std::string& cookie_name() const {
        return cookie_name_;
    }

private:
    struct Etag {
        typedef boost::function<void(const boost::any&)> OneAnyFunc;
        OneAnyFunc handler;
        std::string from_client;
        std::string to_client;
        std::string def;
    };
    typedef std::map<std::string, Etag> Map;
    Map cookie_to_etag_;
    boost::mutex cookie_to_etag_mutex_;
    std::string cookie_name_;

    void handle_etag(const Http::Request& request, Http::Response& response);

    friend class EtagStore;
};

/** A widget storing data on in browser cache (like cookies).

For information on tracking using ETag,
see http://lucb1e.com/rp/cookielesscookies/.

Should be used with EtagStoreResource.

\code
// global
EtagStoreResource* resource = new EtagStoreResource;
WServer::instance()->addResource(resource, "/any-path.gif");
// in session
EtagStore* store = new EtagStore(resource);
Gather* gather = new Gather(...);
gather->add_store(store, Gather::ETAG);
\endcode

\note Key is meaningless. For any key, same value is stored/returned.
    To store several items using this method, use several EtagStoreResource
    instances on several paths and several EtagStore instances.

\ingroup bindings
*/
class EtagStore : public AbstractStore {
public:
    /** Constructor */
    EtagStore(EtagStoreResource* resource, WContainerWidget* parent = 0);

    /** Destructor */
    ~EtagStore();

protected:
    void clear_storage_impl();

    void set_item_impl(const std::string& key, const std::string& value);

    void remove_item_impl(const std::string& key);

    void get_value_of_impl(const std::string& key, const std::string& def);

private:
    EtagStoreResource* resource_;
    std::string cookie_value_;
    std::string key_;

    void update_image();
    void emit_value(const boost::any& result);
};

}

}

#endif

