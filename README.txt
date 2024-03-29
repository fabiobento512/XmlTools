Readme.txt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
XmlTools v2.0g
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

----------------------------------
Description:
----------------------------------

XmlTools is a command-line modding tool that operates on XML files. 

It supports many operations in XML files and even non implemented operations using custom javascript code.

It is developed in C++ and works in both Mac OS and Windows.

An additional guide with examples can be read here: http://wiki.oni2.net/XmlTools

----------------------------------
Full Features:
----------------------------------

-Update all values in a set of XML elements. (e.g., to reposition an OBAN animation or adjust pelvis height for a TRAM).

-Inverts a set of XML elements. (e.g., invert an OBAN animation).

-Add new values to a set of XML elements (e.g., add the 'unkillable' flag to some characters in a level).

-Remove values from a set of XML elements (e.g., remove boss shields from characters in a level).

-Replace values in a set XML elements (e.g., increase the health of characters by replacing the old HP value).

-Patch file support allows the modder to list multiple commands in a file, to all be performed at once.

-Add custom XML to existing files (patch only).

-Remove XML from existing files (patch only).

-Select elements using element name, parent element name, attribute name/value and XPath 1.0.

-Powerful custom XML editing using javascript (for what you can't do with native operations) (patch only). 

----------------------------------
Installation (standalone version):
----------------------------------

Extract XmlTools folder to any place in your computer. Open it with command line on windows or terminal in Mac OS.

----------------------------------
Contacts:
----------------------------------

s10k

Oni Central Forum:
http://oni.bungie.org
->Select forum

Oni Wiki:
http://wiki.oni2.net

Oni Discord (get invite at the forum)

----------------------------------
Change Log:
----------------------------------
2.0g, 09-03-2022
-Fixed bug on add-values when the element is empty (the application was adding a space)
-Qt libraries on macOS upgraded to 5.15.2 (minimum compatible macOS: macOS High Sierra)
----------------------------------
2.0f, 08-09-2019
-Fixed a deadlock in custom js code
----------------------------------
2.0e, 16-02-2019
-Fixed bug in the processing of patch files when the target XML contained an apostrophe (') in the path
----------------------------------
2.0d, 05-10-2017
-Fixed bug in the processing of javascript files for XML custom processing (some files were not being processed)
-Some code refactoring to make the code more modular and easier to build (now uses s10k's "shared" libraries for 
qtcreator projects)
----------------------------------
2.0c, 30-10-2016
-Fixed bug in --update-elements operation
-Added some exception handling
-Started migrating some of the source code to C++14
-Upgraded pugixml to latest 1.7 version
-Dropped openmp since apple clang doesn't support it, using Qt threads mechanisms now
----------------------------------
2.0b, 13-06-2014
-Minor code fixes (use of singleton pattern)
-Fixed segmentation fault when providing a non-existing element name in patch files operations
----------------------------------
2.0a, 02-03-2014
-Added multithreading support for @CUSTOM_CODE (performance may increase over 3 times in a quadcore)
-Now a warning is given if a user javascript code (@CUSTOM_CODE) takes too much time to finish
-Fixed small bug when applying patches with @COMMAND + --no-verbose where some std output should not be displayed
-Added double single quotes in @COMMAND to allow single quotes in XPath expressions
----------------------------------
2.0, 08-02-2014
-Rewrite XmlTools fom the scratch from C# to C++ in order to increase (much!) the performance of the program
-The program now uses the following libraries: Qt5, pugixml and jsxml js library
-The commands were simplified (they now use unix like syntax)
-Update node operation (old updatechainvalues) it now receives the DIFFERENCE between the old value 
and the new value instead of the new value
-Update node operation (old updatechainvalues) relation parameter was removed
-The program now only edits files with .xml extension
-The program now only reads patches files with .patch or .oni-patch extensions
-The patch files now have a XmlTools minimum version
-Some patch files operations were renamed and some extra arguments added
-Added option to select xml elements by attribute name and value
-Added option to select xml elements by XPath 1.0 (should reduce further the need of javascript)
-The insertion of xml via patch now support multiple nodes inside <xml></xml> tags
-Added exclusive option for AEI which allows AEI to pass a list of .oni-patches to process
