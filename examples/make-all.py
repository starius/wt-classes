#!/usr/bin/python

import sys
import glob
import re

entrypoints = []
anchors = []

for cpp in glob.glob('examples/*.cpp'):
    if cpp != 'examples/all.cpp':
        low = cpp.split('/')[-1].split('.')[0] # name without path and extension
        Cap = re.search(r"create([^\s]+)App", open(cpp).read()).groups()[0]
        args = {'low': low, 'Cap': Cap}
        entrypoints.append('''
        addEntryPoint(Wt::Application, create%(Cap)sApp, "/%(low)s");
        ''' % args)
        anchors.append('''
        new WAnchor("%(low)s", "%(Cap)s", root());
        new WBreak(root());
        ''' % args)
sys.stdout.write(sys.stdin.read() % {'entrypoints': ''.join(entrypoints),
    'anchors': ''.join(anchors)})

