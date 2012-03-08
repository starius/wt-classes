#!/bin/bash

rm src/GravatarImage.?pp                            # no md5
rm examples/gravatar.cpp                            # no GravatarImage
sed '/std::string md5/,/^$/d' -i src/util.?pp                   # no md5
sed '/ssl/d' -i Makefile INSTALL debian/control.in src/util.cpp # no openssl

