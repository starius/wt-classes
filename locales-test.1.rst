==============
 locales-test
==============

----------------------------------------------
checks xml-based localizations used by Wt apps
----------------------------------------------

:Author:         Boris Nagaev <bnagaev@gmail.com>
:Manual section: 1
:Manual group:   Wt-classes library

Synopsis
--------
**locales-test** [-h] [--wt *wt.xml*] --prefix *prefix* --sections *sections*...

Options
-------
-h
    show help message and exit

--wt *wt.xml*
    path to wt.xml (original wt.xml, from Wt library)

--prefix *prefix*
    current project prefix

--sections *sections*...
    list of allowed sections

Description
-----------
The **locales-test** command provides a tool
to check xml-based localizations of Wt apps.

Wt applications use xml-based localization files.
These files consist of **messages**.
A message consists of **message identifier** and **mesage translation**.
Each locale is represented with one xml file (**localization**).
Message identifiers are the same in all the localizations,
while message translations are different.
For more information about localization of Wt applications,
see Wt documentation.

Wt does not require much from message identifiers and message order.
To reduce to a system, this tool makes demands:

 * message identifier should be like ``prefix.SECTION.ID``
   (prefix and setion list are provided as command line options)
 * first letter of message identifier should be of the same case,
   as message translation
 * words inside message identifier should be separated with "_",
   regardless of case-style
 * messages should be grouped by section (groups are separated by empty line)
 * messages should be ordered by message identifier (case is ignored)
 * multi-line messages should be moved to the end of group and also be ordered
 * message identifiers of template messages should have suffix "_template"
 * max length of line: 120
 * messages should not start or end with space
 * no tabs are allowed
 * Wt itself translations are in the beginning and need not be sorted

If **--wt** option is provided, this file is used to check traslations
of Wt messages itself (e.g. "Wt.WDatePicker.Close").

To use message translations in Wt app, **Wt::WString::tr()** function is used.
The tool checks if message identifiers in .cpp and .hpp files
and in localization correspond each other.

Examples
--------
Part of ``locales/wtclasses.xml`` used by library wt-classes::

    <message id='wc.wbi.Download'>Download</message>
    <message id='wc.wbi.Run'>Run</message>
    <message id='wc.wbi.View'>View</message>

To test this file and the whole library, located in current folder:

$ locales-test --prefix=wc --sections wbi

Resources
---------
Distributed version control system: https://bitbucket.org/starius/wt-classes

Copying
-------
Copyright (C) 2011 Boris Nagaev.
Free use of this software is granted under the terms of the GNU General
Public License version 2.

