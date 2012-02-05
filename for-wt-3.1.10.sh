#!/bin/bash

rm Planning.?pp                       # depends on WIOService
sed '/WLink /,/}/d' -i Url.cpp        # remove methods returning WLink
sed '/WLink/d' -i Url.hpp             # remove methods returning WLink

