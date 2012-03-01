#!/usr/bin/python

# wt-classes, utility classes used by Wt applications
# Copyright (C) 2011 Boris Nagaev
#
# See the LICENSE file for terms of use.

import os.path
import os
import sys
import argparse

def sh(cmd):
    os.system(cmd)

def set_value(name, value):
    sh('''sed 's@%(name)s = ".\+"@%(name)s = "%(value)s"@' \
            -i `find src/ examples/ -iname "*.?pp"` ''' %
            {'name': name, 'value': value})

consts = [
    ('SWFSTORE_FILE', '/usr/share/javascript/yui/swfstore/swfstore.swf'),
]

def name_to_attr(name):
    return name.lower()

def name_to_option(name):
    return '--' + name.lower().replace('_', '-')

def name_to_help(name):
    return name.lower().replace('_', ' ')

def configure(args):
    args_dict = vars(args)
    for name, default_value in consts:
        attr = name_to_attr(name)
        if attr in args_dict:
            set_value(name, args_dict[attr])

def main():
    p = argparse.ArgumentParser(description='Wt-classes configurator',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    for name, default_value in consts:
        p.add_argument(name_to_option(name), help=name_to_help(name),
                default=default_value)
    args = p.parse_args()
    configure(args)

if __name__ == '__main__':
    try:
        main()
    except Exception, e:
        print e

