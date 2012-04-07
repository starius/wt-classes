#!/usr/bin/python

import sys
import re
from optparse import OptionParser

entrypoints = []
anchors = []

parser = OptionParser()
parser.add_option("--cpp", dest="cpp")
parser.add_option("--template", dest="template")
parser.add_option("--wrasterimage", dest="wrasterimage", action="store_true")
(options, args) = parser.parse_args()

remove_main = re.compile("int main.+\}", re.DOTALL)

for cpp in options.cpp.split():
    if not cpp.endswith('all.cpp'):
        sys.stdout.write(remove_main.sub("", open(cpp).read()))
        low = re.split(r'[/\\]', cpp)[-1].split('.')[0]
        if not options.wrasterimage and low == 'captcha':
            continue
        Cap = re.search(r"create([^\s]+)App", open(cpp).read()).groups()[0]
        args = {'low': low, 'Cap': Cap}
        entrypoints.append('''
        addEntryPoint(Wt::Application, create%(Cap)sApp, "/%(low)s");
        ''' % args)
        anchors.append('''
        new WAnchor("%(low)s", "%(Cap)s", root());
        new WBreak(root());
        ''' % args)
sys.stdout.write(open(options.template).read() %
    {'entrypoints': ''.join(entrypoints), 'anchors': ''.join(anchors)})

