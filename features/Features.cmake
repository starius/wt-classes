include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} wt)

check_include_file_cxx(Wt/WLink HAVE_WLINK)
check_include_file_cxx(Wt/WIOService HAVE_WIOSERVICE)
check_include_file_cxx(Wt/Http/Message HAVE_WHTTP_MESSAGE)
check_include_file_cxx(Wt/WRandom HAVE_WRANDOM)
check_include_file_cxx(Wt/WRasterImage HAVE_WRASTERIMAGE)
check_include_file_cxx(Wt/WSpinBox HAVE_WSPINBOX)
check_include_file_cxx(Wt/WDoubleSpinBox HAVE_WDOUBLESPINBOX)
check_cxx_source_compiles("#include <Wt/WFormWidget>\n int main() {
    Wt::WFormWidget* a; a->valueText(); }" HAVE_WFORMWIDGET_VALUETEXT)
check_cxx_source_compiles("#include <Wt/WApplication>\n int main() {
    Wt::WApplication::appRoot(); } " HAVE_WAPPLICATION_APPROOT)
check_cxx_source_compiles("#include <Wt/WContainerWidget>\n int main() {
    Wt::WContainerWidget c; c.doJavaScript(\"\"); } "
    HAVE_WIDGET_DO_JAVA_SCRIPT)
check_cxx_source_compiles("#include <Wt/WCompositeWidget>\n
    class A : public Wt::WCompositeWidget { A() { implementation(); } };
    int main() { }" HAVE_WCOMPOSITEWIDGET_IMPLEMENTATION)

set(HAVE_RECAPTCHA, ${HAVE_WHTTP_MESSAGE})

