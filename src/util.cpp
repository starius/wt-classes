/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "config.hpp"

#define USE_SERVER_POST (defined(WC_HAVE_SERVER_POST) && \
    defined(WC_HAVE_ENVIRONMENT_SERVER))

#include <boost/version.hpp>
#if BOOST_VERSION >= 104400
#define BOOST_FILESYSTEM_VERSION 3
#endif

#include <sstream>
#include <climits>
#include <cstdio>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#if !USE_SERVER_POST
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#endif
#if !USE_WIOSERVICE
#include <boost/system/error_code.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#endif

#ifdef WC_USE_WT_MD5
#include <Wt/Utils>
#endif
#ifdef WC_USE_OPENSSL
#include <openssl/md5.h>
#endif
#ifdef WC_HAVE_WIOSERVICE
#include <Wt/WIOService>
#endif

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WServer>
#include <Wt/WLineEdit>
#include <Wt/WTextArea>
#include <Wt/WTextEdit>
#include <Wt/WPushButton>
#include <Wt/WComboBox>
#include <Wt/WAbstractToggleButton>
#include <Wt/WSlider>
#include <Wt/WDialog>
#include <Wt/WTableView>

#include "util.hpp"
#include "rand.hpp"
#include "TimeDuration.hpp"

namespace Wt {

namespace Wc {

#if USE_SERVER_POST
static void func_runner(const boost::function<void()>& func) {
    if (!wApp->isQuited()) {
        func();
    }
}

static void post(WServer* server, const std::string& app,
                 const boost::function<void()>& func) {
    server->post(app, boost::bind(func_runner, func));
}
#else
typedef boost::shared_ptr<bool> BoolPtr;

class AG : public WObject {
public:
    AG(const BoolPtr& ptr):
        ptr_(ptr)
    { }

    ~AG() {
        *ptr_ = true;
    }

private:
    BoolPtr ptr_;
};

boost::mutex do_func_mutex;

static void do_func(boost::function<void()> func, WApplication* app,
                    BoolPtr b) {
    boost::mutex::scoped_lock do_func_lock(do_func_mutex);
    if (!*b && !app->isQuited()) {
        WApplication::UpdateLock app_lock = app->getUpdateLock();
        if (!*b && !app->isQuited()) {
            func();
        }
    }
}

static void thread_func(boost::function<void()> func, WApplication* app,
                        BoolPtr b) {
    schedule_action(td::TD_NULL, boost::bind(do_func, func, app, b));
}
#endif

boost::function<void()> bound_post(boost::function<void()> func) {
    if (wApp) {
#if USE_SERVER_POST
        WServer* server = DOWNCAST<WServer*>(wApp->environment().server());
        return boost::bind(post, server, wApp->sessionId(), func);
#else
        BoolPtr ptr = boost::make_shared<bool>();
        *ptr = false;
        wApp->addChild(new AG(ptr));
        return boost::bind(thread_func, func, wApp, ptr);
#endif
    } else {
        return boost::bind(schedule_action, td::TD_NULL, func);
    }
}

typedef std::vector<boost::any> Anys;

struct OneData {
    Anys anys;
    boost::mutex mutex;
    bool allow_merge;
};

struct OneAnyFuncBinder {
    void operator()() {
        boost::mutex& mutex = arg_ptr->mutex;
        Anys& anys = arg_ptr->anys;
        bool allow_merge = arg_ptr->allow_merge;
        if (allow_merge) {
            mutex.lock();
            Anys anys_copy = anys;
            anys.clear();
            mutex.unlock();
            BOOST_FOREACH (const boost::any& arg, anys_copy) {
                func(arg);
            }
        } else {
            mutex.lock();
            boost::any arg = anys.back();
            anys.pop_back();
            mutex.unlock();
            func(arg);
        }
    }
    OneAnyFunc func;
    boost::shared_ptr<OneData> arg_ptr;
};

struct OneAnyFuncHolder {
    void operator()(const boost::any& arg) {
        boost::mutex& mutex = arg_ptr->mutex;
        Anys& anys = arg_ptr->anys;
        bool allow_merge = arg_ptr->allow_merge;
        mutex.lock();
        bool post_needed = !allow_merge || anys.empty();
        anys.push_back(arg);
        mutex.unlock();
        if (post_needed) {
            posted_binder();
        }
    }
    boost::function<void()> posted_binder;
    boost::shared_ptr<OneData> arg_ptr;
};

OneAnyFunc one_bound_post(const OneAnyFunc& func, bool allow_merge) {
    OneAnyFuncBinder binder;
    OneAnyFuncHolder holder;
    binder.func = func;
    binder.arg_ptr = boost::make_shared<OneData>();
    binder.arg_ptr->allow_merge = allow_merge;
    holder.arg_ptr = binder.arg_ptr;
    holder.posted_binder = bound_post(binder);
    return holder;
}

void updates_trigger() {
    if (wApp && wApp->updatesEnabled() && !wApp->isQuited()) {
        wApp->triggerUpdate();
    }
}

void updates_poster(WServer* server, WApplication* app) {
#if USE_SERVER_POST
    server->post(app->sessionId(), updates_trigger);
#else
    bound_post(updates_trigger)();
#endif
}

std::string unique_filename() {
    using namespace boost::filesystem;
    using namespace std;
#if BOOST_FILESYSTEM_VERSION == 3
    const char* const model = "wt-classes-%%%%-%%%%-%%%%-%%%%";
    return unique_path(temp_directory_path() / model).string();
#else
    string result;
    for (int attempt = 0; attempt < 10; attempt++) {
        string path = tmpnam(NULL);
        ofstream file_out(path.c_str());
        if (file_out.is_open()) {
            int secret = rr();
            file_out << secret << endl;
            file_out.close();
            if (exists(path)) {
                ifstream file_in(path.c_str());
                if (file_in.is_open()) {
                    int test;
                    file_in >> test;
                    file_in.close();
                    file_out.open(path.c_str(), ios::out | ios::trunc);
                    file_out.close();
                    if (test == secret) {
                        result = path;
                        break;
                    }
                }
            }
        }
    }
    return result;
#endif
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

#ifdef WC_HAVE_MD5
std::string md5(const std::string& data) {
#ifdef WC_USE_WT_MD5
    return Wt::Utils::hexEncode(Wt::Utils::md5(data));
#elif defined(WC_USE_OPENSSL)
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
#endif
}
#endif

std::string urlencode(const std::string& url) {
    std::stringstream result;
    result.setf(std::ios::hex, std::ios::basefield);
    result.setf(std::ios::uppercase);
    result.width(2);
    result.fill('0');
    BOOST_FOREACH (char c, url) {
        if (c == ' ') {
            result.put('+');
        } else if (c == '-' || c == '_' || c == '.') {
            result.put(c);
        } else if (isalnum(c) && (0 < c && c < 127)) {
            result.put(c);
        } else {
            result.put('%');
            result << short(c);
        }
    }
    return result.str();
}

std::string urldecode(const std::string& text) {
    // source: src/Wt/Utils.C
    std::stringstream result;
    for (unsigned i = 0; i < text.length(); ++i) {
        char c = text[i];
        if (c == '+') {
            result << ' ';
        } else if (c == '%' && i + 2 < text.length()) {
            std::string h = text.substr(i + 1, 2);
            char* e = 0;
            int hval = std::strtol(h.c_str(), &e, 16);
            if (*e == 0) {
                result << (char)hval;
                i += 2;
            } else {
                // not a proper %XX with XX hexadecimal format
                result << c;
            }
        } else {
            result << c;
        }
    }
    return result.str();
}

void set_hidden(WWidget* widget, bool hidden) {
    if (hidden) {
        widget->hide();
    } else {
        widget->show();
    }
}

std::string bool_to_string(bool value) {
    return value ? "true" : "false";
}

#define USE_WIOSERVICE (defined(WC_HAVE_WIOSERVICE) && \
        defined(WC_HAVE_ENVIRONMENT_SERVER))

#if !USE_WIOSERVICE
typedef boost::asio::deadline_timer Timer;
typedef boost::shared_ptr<Timer> TimerPtr;

struct WcIoService {
    WcIoService():
        work(new boost::asio::io_service::work(io)) {
        for (int i = 0; i < boost::thread::hardware_concurrency(); i++) {
            gr.create_thread(boost::bind(&boost::asio::io_service::run, &io));
        }
    }

    ~WcIoService() {
        delete work;
        io.stop();
    }

    boost::asio::io_service io;
    boost::asio::io_service::work* work;
    boost::thread_group gr;
} wc_io;

static void handle_timeout(TimerPtr /* timer */,
                           const boost::function<void()>& func,
                           const boost::system::error_code& e) {
    if (!e) {
        func();
    }
}
#endif

void schedule_action(const td::TimeDuration& wait,
                     const boost::function<void()>& func) {
#if USE_WIOSERVICE
    int ms = wait.total_milliseconds();
    if (ms < 0) {
        ms = INT_MAX;
    }
    WIOService& io = WServer::instance()->ioService();
    io.schedule(ms, func);
#else
    TimerPtr timer = boost::make_shared<Timer>(boost::ref(wc_io.io), wait);
    timer->async_wait(boost::bind(handle_timeout, timer, func,
                                  boost::asio::placeholders::error));
#endif
}

std::string approot() {
#ifdef WC_HAVE_WAPPLICATION_APPROOT
    return WApplication::appRoot();
#else
    return config_value("approot");
#endif
}

WString value_text(const WFormWidget* form_widget) {
#ifdef WC_HAVE_WFORMWIDGET_VALUETEXT
    return form_widget->valueText();
#else
    if (isinstance<WLineEdit>(form_widget)) {
        return DOWNCAST<const WLineEdit*>(form_widget)->text();
    } else if (isinstance<WTextArea>(form_widget)) {
        return DOWNCAST<const WTextArea*>(form_widget)->text();
    } else if (isinstance<WPushButton>(form_widget)) {
        return DOWNCAST<const WPushButton*>(form_widget)->text();
    } else if (isinstance<WComboBox>(form_widget)) {
        return DOWNCAST<const WComboBox*>(form_widget)->currentText();
    } else if (isinstance<WAbstractToggleButton>(form_widget)) {
        return DOWNCAST<const WAbstractToggleButton*>(form_widget)->text();
    } else if (isinstance<WSlider>(form_widget)) {
        return TO_S(reinterpret_cast<const WSlider*>(form_widget)->value());
        // NOTE: WSlider used to be WCompositeWidget's descendant
        // In that case reinterpret_cast newer happens, since isinstance check
    } else {
        return "";
    }
#endif
}

void set_closable(WDialog* dialog) {
#ifdef WC_HAVE_WDIALOG_SET_CLOSABLE
    dialog->setClosable(true);
#else
    WPushButton* close = new WPushButton("X");
    close->clicked().connect(dialog, &WDialog::reject);
#ifdef WC_HAVE_WDIALOG_TITLEBAR
    dialog->titleBar()->insertWidget(0, close);
#else
    dialog->contents()->addWidget(close);
#endif
#endif
}

class DeleteSender : public WObject {
public:
    void delete_sender() {
        delete sender();
    }
} delete_sender;

void delete_closed(WDialog* dialog) {
    dialog->finished().connect(&delete_sender, &DeleteSender::delete_sender);
}

void fix_text_edit(WTextEdit* text_edit) {
    if (wApp && !wApp->environment().ajax()) {
        return;
    }
    WWidget* parent_widget = text_edit->parent();
    if (isinstance<WContainerWidget>(parent_widget)) {
        WContainerWidget* parent = DOWNCAST<WContainerWidget*>(parent_widget);
        WContainerWidget* wrapper = new WContainerWidget;
        parent->insertBefore(wrapper, text_edit);
        parent->removeWidget(text_edit);
        wrapper->addWidget(text_edit);
        if (text_edit->width() == WLength::Auto ||
                text_edit->height() == WLength::Auto) {
            text_edit->resize(525, 130);
        }
        wrapper->resize(text_edit->width(), text_edit->height().toPixels() + 5);
    }
}

std::string json_escape_utf8(const std::string& utf8) {
    const std::wstring& wstr = WString().fromUTF8(utf8).value();
    std::stringstream strm;
    for (unsigned i = 0; i < wstr.size(); ++i) {
        if (wstr[i] < 128) {
            strm.put(char(wstr[i]));
        } else {
            strm.put('\\');
            strm.put('u');
            strm << std::hex
                 << std::setfill('0')
                 << std::setw(4)
                 << short(wstr[i]);
        }
    }
    return strm.str();
}

void scroll_to_last(WTableView* view) {
    if (!wApp || !wApp->environment().ajax() || view->pageCount() > 0) {
        view->setCurrentPage(view->pageCount() - 1);
    } else if (wApp) {
        wApp->doJavaScript("$('#" + view->id() + " div')"
                           ".scrollTop(999999999);");
    }
}

int str2int(const std::string& str, int bad) {
    try {
        return boost::lexical_cast<int>(str);
    } catch (boost::bad_lexical_cast&) {
        return bad;
    }
}

}

}

