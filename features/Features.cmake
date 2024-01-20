include(CheckCXXSourceCompiles)
include(FindOpenSSL)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES}
    wt boost_filesystem boost_system)

check_cxx_source_compiles("#include <Wt/WLink>\n int main() {
    Wt::WLink l; }" WC_HAVE_WLINK)
check_cxx_source_compiles("#include <Wt/WIOService>\n int main() {
    Wt::WIOService io; }" WC_HAVE_WIOSERVICE)
check_cxx_source_compiles("#include <Wt/Http/Message>\n int main() {
    Wt::Http::Message m; }" WC_HAVE_WHTTP_MESSAGE)
check_cxx_source_compiles("#include <Wt/WRandom>\n int main() {
    Wt::WRandom::get(); }" WC_HAVE_WRANDOM)
check_cxx_source_compiles("#include <Wt/WRasterImage>\n
    int main() { Wt::WRasterImage r(\"\",0,0); }"
    WC_HAVE_WRASTERIMAGE)
check_cxx_source_compiles("#include <Wt/WSpinBox>\n int main() {
    Wt::WSpinBox box; }" WC_HAVE_WSPINBOX)
check_cxx_source_compiles("#include <Wt/WDoubleSpinBox>\n int main() {
    Wt::WDoubleSpinBox box; }" WC_HAVE_WDOUBLESPINBOX)
check_cxx_source_compiles("#include <Wt/WFormWidget>\n int main() {
    Wt::WFormWidget* a; a->valueText(); }" WC_HAVE_WFORMWIDGET_VALUETEXT)
check_cxx_source_compiles("#include <Wt/WApplication>\n int main() {
    Wt::WApplication::appRoot(); } " WC_HAVE_WAPPLICATION_APPROOT)
check_cxx_source_compiles("#include <Wt/WContainerWidget>\n int main() {
    Wt::WContainerWidget c; c.doJavaScript(\"\"); } "
    WC_HAVE_WIDGET_DO_JAVA_SCRIPT)
check_cxx_source_compiles("#include <Wt/WCompositeWidget>\n
    class A : public Wt::WCompositeWidget { A() { implementation(); } };
    int main() { }" WC_HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION)
check_cxx_source_compiles("#include <Wt/Dbo/SqlConnection>\n
    int main() { Wt::Dbo::SqlTime; }" WC_HAVE_SQLTIME)
check_cxx_source_compiles("#include <Wt/WServer>\n int main() { Wt::WServer* s;
    s->post(\"\", main); }" WC_HAVE_SERVER_POST)
check_cxx_source_compiles("#include <Wt/WEnvironment>\n
    #include <Wt/WApplication>\n int main() {
    wApp->environment().server(); }" WC_HAVE_ENVIRONMENT_SERVER)
check_cxx_source_compiles("#include <Wt/Utils>\n int main() {
    Wt::Utils::md5(\"\"); }" WC_HAVE_WT_MD5)
check_cxx_source_compiles("#include <Wt/WFont>\n int main() {
    Wt::WFont().setSize(200); }" WC_HAVE_WFONT_SETSIZE_DOUBLE)
check_cxx_source_compiles("#include <Wt/WDialog>\n int main() {
    Wt::WDialog().setClosable(true); }" WC_HAVE_WDIALOG_SET_CLOSABLE)
check_cxx_source_compiles("#include <Wt/WDialog>\n int main() {
    Wt::WDialog().titleBar(); }" WC_HAVE_WDIALOG_TITLEBAR)
check_cxx_source_compiles("#include <Wt/Json/Object>\n int main() {
    Wt::Json::Object obj; }" WC_HAVE_JSON_OBJECT)
check_cxx_source_compiles("#include <Wt/WTableView>\n int main() {
    Wt::WTableView v; v.setCurrentPage(0); }" WC_HAVE_ITEMVIEW_PAGING)
check_cxx_source_compiles("#include <string>\n #include <Wt/WApplication>\n
    int main() { std::string l = wApp->locale(); }" WC_HAVE_STRING_LOCALE)

if(WC_HAVE_WT_MD5)
    set(WC_HAVE_MD5 ON)
    set(WC_USE_WT_MD5 ON)
elseif(OPENSSL_FOUND)
    set(WC_HAVE_MD5 ON)
    set(WC_USE_OPENSSL ON)
endif()

set(WC_HAVE_GRAVATAR ${WC_HAVE_MD5})
set(WC_HAVE_RECAPTCHA ${WC_HAVE_WHTTP_MESSAGE})
set(WC_HAVE_PAGER ${WC_HAVE_ITEMVIEW_PAGING})

