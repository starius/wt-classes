#!/bin/bash

rm src/Planning.?pp                            # depends on WIOService
sed '/WLink /,/}/d' -i src/Url.cpp             # remove methods returning WLink
sed '/WLink/d' -i src/Url.hpp                  # remove methods returning WLink
sed 's/link/full_path/g' -i examples/url.cpp   # use string instead of WLink
sed 's/setLink/setRefInternalPath/g' -i examples/url.cpp
rm src/Recaptcha.?pp                           # depends on Http::Client
rm examples/recaptcha.cpp
sed 's/WLink/std::string/g' -i src/GravatarImage.?pp
sed 's/link.url()/link/g' -i src/GravatarImage.cpp
sed 's/setImageLink/setImageRef/g' -i src/GravatarImage.cpp

# remove PaintedCaptcha::set_input
sed 's/valueText()/text()/g' -i src/PaintedCaptcha.cpp
sed 's/WFormWidget\* edit_/WLineEdit\* edit_/g' -i src/PaintedCaptcha.cpp
sed '/set_input/,/^ \{0,4\}}/d' -i src/PaintedCaptcha.cpp
sed '/set_input/d' -i src/PaintedCaptcha.hpp

# patch debian files
sed 's/libwt\(.\+\)(\(.\+ \)\?\([0-9.]\+\))/libwt\1(\23.1.10)/g' \
    -i debian/control.in

