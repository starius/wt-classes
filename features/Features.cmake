include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} wt)

check_include_file_cxx(Wt/WLink HAVE_WLINK)
check_include_file_cxx(Wt/WIOService HAVE_WIOSERVICE)
check_include_file_cxx(Wt/Http/Message HAVE_WHTTP_MESSAGE)
check_include_file_cxx(Wt/WRandom HAVE_WRANDOM)
check_cxx_source_compiles("#include <Wt/WFormWidget>\n int main() {
    Wt::WFormWidget* a; a->valueText(); }" HAVE_WFORMWIDGET_VALUETEXT)
check_cxx_source_compiles("#include <Wt/WApplication>\n int main() {
    Wt::WApplication::appRoot(); } " HAVE_WAPPLICATION_APPROOT)
check_cxx_source_compiles("#include <Wt/WContainerWidget>\n int main() {
    Wt::WContainerWidget c; c.doJavaScript(\"\"); } "
    HAVE_WIDGET_DO_JAVA_SCRIPT)
