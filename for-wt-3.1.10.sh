#!/bin/bash

rm src/Planning.?pp                            # depends on WIOService
sed '/WLink /,/}/d' -i src/Url.cpp             # remove methods returning WLink
sed '/WLink/d' -i src/Url.hpp                  # remove methods returning WLink

