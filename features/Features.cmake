include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)
include(FindOpenSSL)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES}
    wt boost_filesystem boost_system)

check_include_file_cxx(Wt/WLink WC_HAVE_WLINK)
check_include_file_cxx(Wt/WIOService WC_HAVE_WIOSERVICE -lboost_system)
check_include_file_cxx(Wt/Http/Message WC_HAVE_WHTTP_MESSAGE)
check_include_file_cxx(Wt/WRandom WC_HAVE_WRANDOM)
check_include_file_cxx(Wt/WRasterImage WC_HAVE_WRASTERIMAGE)
check_include_file_cxx(Wt/WSpinBox WC_HAVE_WSPINBOX)
check_include_file_cxx(Wt/WDoubleSpinBox WC_HAVE_WDOUBLESPINBOX)
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

if(WC_HAVE_WT_MD5)
    set(WC_HAVE_MD5 ON)
    set(WC_USE_WT_MD5 ON)
elseif(OPENSSL_FOUND)
    set(WC_HAVE_MD5 ON)
    set(WC_USE_OPENSSL ON)
endif()

set(WC_HAVE_GRAVATAR ${WC_HAVE_MD5})
set(WC_HAVE_RECAPTCHA ${WC_HAVE_WHTTP_MESSAGE})

