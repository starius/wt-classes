include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} wt)

check_include_file_cxx(Wt/WLink HAVE_WLINK)
check_include_file_cxx(Wt/WIOService HAVE_WIOSERVICE)
check_include_file_cxx(Wt/Http/Message HAVE_WHTTP_MESSAGE)
check_cxx_source_compiles("#include <Wt/WFormWidget>\n int main() {
    Wt::WFormWidget* a; a->valueText(); }" HAVE_WFORMWIDGET_VALUETEXT)

