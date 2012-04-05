#!/bin/bash

./for-wt-3.1.10.sh

rm src/ConstrainedSpinBox.?pp  # WSpinBox
rm src/TimeDurationWidget.?pp  # ConstrainedSpinBox
rm src/Notify.?pp              # too complicated post() scheme
rm src/PaintedCaptcha.?pp      # WRasterImage
rm examples/captcha.cpp        # no PaintedCaptcha
rm src/AbstractCaptcha.?pp     # no impls
rm src/TimeDurationDbo.?pp     # no SqlTime (time duration)

# no upload->empty()
sed 's@file_upload_->empty()@file_upload_->emptyFileName()@' -i src/Wbi.cpp

# no implementation()
sed 's@ WCompositeWidget@ MyCompositeWidget@' -i src/Wbi.?pp
sed '/namespace Wc {/rpatches/MyCompositeWidget' -i src/Wbi.hpp

# no post
sed '/namespace Wc {/rpatches/post' -i src/Wbi.cpp
sed 's@updates_trigger@boost::bind(\&WApplication::triggerUpdate, wApp)@' \
    -i src/Wbi.cpp
sed '/updates_poster/,/^$/d' -i src/util.?pp # remove updates_poster()
sed '/bound_post(boost::function/,/^$/d' -i src/util.?pp # remove bound_post()
sed '/void post/,/^$/d' -i src/util.cpp # remove post()

sed 's@Wt::WApplication::appRoot() +@@' -i examples/*.cpp # no appRoot

# no WRandom
sed '/Wt\/WRandom/d' -i src/*.cpp examples/*.cpp
sed 's@\(Wt::\)\?WRandom::generateId()@("r" + TO_S(rand()))@' \
    -i src/*.cpp examples/*.cpp
sed '1i#include "rand.hpp"' -i src/Gather.cpp
sed '1i#include <Wt/Wc/util.hpp>' -i `grep TO_S -l examples/*.cpp`
sed '/Wt\/WRandom/d' -i src/rand.cpp
sed '1i#include <cstdlib>' -i src/rand.cpp
sed '1i#include <ctime>' -i src/rand.cpp
sed '/namespace Wc {/rpatches/srand' -i src/rand.cpp
sed 's@WRandom::get()@rand()@' -i src/rand.cpp

# no widget->doJavaScript
sed 's@  doJavaScript@  wApp->doJavaScript@' -i src/*.cpp

# problems with WString in Wt/WJavaScript
sed '/value/s@WString@std::string@' -i src/SWFStore.?pp src/Gather.?pp
sed 's@\.toUTF8()@@' -i src/SWFStore.cpp src/Gather.cpp

# patch debian files
sed 's/libwt\(.\+\)(\(.\+ \)\?\([0-9.]\+\))/libwt\1(\23.1.2)/g' \
    -i debian/control

