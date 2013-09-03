/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_CACHED_CONTENTS_HPP_
#define WC_CACHED_CONTENTS_HPP_

#include <set>
#include <map>
#include <string>
#include <utility>

#include <Wt/WContainerWidget>

namespace Wt {

namespace Wc {

/** Widget caching its contents for URL.

This widget should be used as main container for the site.
It stores cache of widgets previously seen.
It increases loading of previously seen URL at the cost of
additional memory used for widgets cache.

How to use with Parser.
\code
class MyParser : public Parser {
    MyParser() {
        // add nodes and slots
    }
};

class MainWidget : public CachedContents {
protected:
    void open_url_impl(const std::string& url) {
        parser->open(url);
    }
};

wApp->internalPathChanged().connect(CachedContents::open_url);
\endcode

\ingroup url
*/
class CachedContents : public WContainerWidget {
public:
    /** Constructor */
    CachedContents(WContainerWidget* parent = 0);

    /** Destructor */
    virtual ~CachedContents();

    /** Update visible contents of widget according to the URL.
    If the URL is not ignored according to rules set by ignore_url()
    of ignore_prefix(), the widget is cached and is used if the URL
    is opened next time.
    If cache_size() is exceeded, the earliest widget is deleted.
    most unpopular widgets are deleted.
    */
    void open_url(const std::string& url);

    /** Return current widget or 0 */
    WWidget* current_widget() const {
        return current_widget_;
    }

    /** Return if window title is cached.
    Defaults to true.
    */
    bool cache_title() const {
        return cache_title_;
    }

    /** Set if title is cached */
    void set_cache_title(bool cache_title) {
        cache_title_ = cache_title;
    }

    /** Set the widget as contents */
    void set_contents_raw(WWidget* widget);

    /** Return max number of widgets in cache (excluding currently shown).
    Defaults to 10.
    */
    int cache_size() const {
        return cache_size_;
    }

    /** Set max number of widgets in cache (excluding currently shown).
    If cache size exceeds new value, it is reduced.
    */
    void set_cache_size(int cache_size);

    /** Do not cache widgets at given URL */
    void ignore_url(const std::string& url);

    /** Do not cache widgets at URLs starting with given prefix */
    void ignore_prefix(const std::string& prefix);

    /** Clear cache and widget (as WContainerWidget.clear()).
    Do it on login/logout.
    */
    void clear();

    /** Clear cache and widget */
    void clear_cache();

protected:
    /** Update visible contents of widget according to the URL (implementation).
    The function is invoked from open_url() if this URL is not in cache.
    The function must call set_contents_raw() with actual widget.
    */
    virtual void open_url_impl(const std::string& url) = 0;

private:
    typedef std::set<std::string> StringsSet;
    StringsSet ignored_urls_;
    StringsSet ignored_prefixes_;
    typedef std::pair<WWidget*, WString> WidgetAndTitle;
    typedef std::map<std::string, WidgetAndTitle> Url2Widget;
    Url2Widget url_to_widget_;
    typedef std::list<std::string> StringsList;
    StringsList visited_urls_;
    int cache_size_;
    WWidget* current_widget_;
    bool cache_title_;

    bool is_ignored(const std::string& url);
    void resize_cache();
};

}

}

#endif

