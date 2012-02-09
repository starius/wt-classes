#!/usr/bin/python

# wt-classes, utility classes used by Wt applications
# Copyright (C) 2011 Boris Nagaev
#
# See the LICENSE file for terms of use.

import codecs
import os.path
from xml.etree.ElementTree import parse
from StringIO import StringIO
import fnmatch
import os
import sys
import re
import argparse
import glob
from functools import partial

ampersand = '-locales_test_ampersand'

def error(stream, message, type='error', file=None, line=None, id=None):
    if id:
        if file:
            stream.write(file + ': ')
        stream.write('In message "%s":\n' % id)
    if file:
        stream.write(file + ':')
        if line:
            stream.write(str(line + 1) + ':')
        stream.write(' ')
    stream.write(type + ': ' + message + "\n")

def get_wt_ids(wt_files, e):
    wt_ids = set()
    for wt_xml in wt_files:
        try:
            wt_messages = parse(wt_xml).getroot()
            if wt_messages.get('id') != 'messages':
                e('incorrent Wt XML file', file=wt_xml.name)
            for message in wt_messages:
                wt_ids.add(message.get('id'))
        except:
            e('incorrent Wt XML file', file=wt_xml.name)
    return wt_ids

def get_messages(filename, e):
    xml_file = codecs.open(filename, 'r', 'utf-8')
    if xml_file.read().count('\t'):
        e('no tabs are allowed', file=filename)
    xml_file.seek(0)
    for line_index, line in enumerate(xml_file):
        if len(line) > 120:
            e('the line is too long', file=filename, line=line_index)
    contents = open(filename).read().replace('&', ampersand).replace('if:', '')
    xml_file = StringIO(contents)
    messages = parse(xml_file).getroot()
    return messages

def analyze_message(message, prev_message, ids, wt_ids, prefix, sections, e):
    Id = message.get('id')
    if not message.text:
        message.text = ''
    text = message.text
    if message.tag != 'message':
        e('wrong tag "%s"' % message.tag, file=filename, id=Id)
    if text and '\n' not in text and (text[0] == ' ' or text[-1] == ' '):
        e('message should not start or end with space')
    if Id in ids:
        e('duplicate message')
    ids.add(Id)
    if not Id.startswith('Wt.') and not Id.startswith(prefix):
        e('message id should start with Wt. or %s' % prefix)
    if Id.startswith('Wt.'):
        if Id not in wt_ids:
            if wt_ids:
                e('unknown Wt message (old wt.xml?)')
            else:
                e('unknown Wt message (provide --wt option)')
    else:
        try:
            prefix_copy, section, id = Id.split('.')
        except:
            e("can't match to %s.section.id" % prefix)
        message.section = section
        if section not in sections:
            e('unknown section "%s"' % section)
        if '-' in id:
            e('id should not contain "-"')
        if text and id[0].islower() != text[0].islower() and \
                text[0].isalpha():
            e('id should start with letter of same case as message')
        if '${' in text and not id.endswith('_template'):
            e('id of template message should end with "_template"')
        if hasattr(prev_message, 'section'):
            if prev_message.section != section and \
                    '\n\n' not in prev_message.tail:
                e('separate sections %s and %s with empty line' %
                        (prev_message.section, section))
            elif prev_message.section == section:
                if '\n\n' in prev_message.tail:
                    e('do not separate section %s' % section)
                if '\n' in prev_message.text and '\n' not in message.text:
                    e('non-multiline message after multiline messages')
                if '\n' in prev_message.text and '\n' in message.text \
                    and prev_message.get('id') > Id and not 'board' in Id:
                    e('multiline messages %s and %s are unordered' %
                            (prev_message.get('id'), Id))

def analyze_messages(messages, ids, id2text, wt_ids, prefix, sections, e):
    prev_message = None
    for message in messages:
        Id = message.get('id')
        text = message.text
        id2text[Id] = text
        analyze_message(message, prev_message, ids, wt_ids, prefix,
                sections, partial(e, id=Id))
        prev_message = message
    short_messages = [m.get('id').lower() for m in messages
        if m.text and '\n' not in m.text and
            m.get('id').startswith(prefix)]
    for m1, m2 in zip(short_messages, sorted(short_messages)):
        if m1 != m2:
            e('messages are unsorted, started from %s != %s' % (m1, m2))

def locales_test(wt, prefix, sections):
    def e(*args, **kwargs):
        error(sys.stderr, *args, **kwargs)

    wt_ids = get_wt_ids(wt or [], e)

    filename2ids = {}
    id2text = {}

    for filename in glob.glob('locales/*.xml'):
        ids = filename2ids[filename] = set()
        messages = get_messages(filename, e)
        analyze_messages(messages, ids, id2text, wt_ids, prefix, sections,
                partial(e, file=filename))

    all_ids = reduce(lambda a,b:a|b, filename2ids.values(), set())
    cleaner = re.compile(r'{.+}|<.+>|%s.{1,8};' % ampersand)

    for filename, ids in filename2ids.items():
        for Id in all_ids - ids:
            if Id.startswith("Wt."):
                continue
            if not any(c.isalpha() for c in re.sub(cleaner, "", id2text[Id])):
                continue
            e('the id not found in some .xml files', type='warning',
                    file=filename, id=Id)

    l9n_re = re.compile(r'"(%s\.[^"]+)"' % re.escape(prefix))
    used_ids = set()

    for root, dirnames, filenames in os.walk('src'):
        for filename in fnmatch.filter(filenames, '*.?pp'):
            path = os.path.join(root, filename)
            for match in re.findall(l9n_re, open(path).read()):
                used_ids.add(match)
                if match not in all_ids:
                    e("can't find message for the id", file=path, id=match)

    for Id in all_ids - used_ids:
        if not Id.startswith('Wt.'):
            e('the id defined but not used', type='warning', id=Id)

def main():
    p = argparse.ArgumentParser(description='Wt l9n checker')
    p.add_argument('--wt', help='path to wt.xml', metavar='FILE', nargs='*',
            type=argparse.FileType('r'))
    p.add_argument('--prefix', help='Current project prefix', metavar='STR',
            required=True)
    p.add_argument('--sections', help='The list of allowed sections', nargs='+',
            required=True)
    args = p.parse_args()
    locales_test(args.wt, args.prefix, args.sections)

if __name__ == '__main__':
    try:
        main()
    except Exception, e:
        print e

