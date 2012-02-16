#!/bin/bash

rm src/Planning.?pp                            # depends on WIOService
sed '/WLink /,/}/d' -i src/Url.cpp             # remove methods returning WLink
sed '/WLink/d' -i src/Url.hpp                  # remove methods returning WLink
sed 's/link/full_path/g' -i examples/url.cpp   # use string instead of WLink
sed 's/setLink/setRefInternalPath/g' -i examples/url.cpp
rm src/Recaptcha.?pp                           # depends on Http::Client

