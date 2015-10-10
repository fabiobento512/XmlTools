# XmlTools
Readme.txt
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
XmlTools v2.0b
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

Script10k, "faob2@hotmail.com"

Oni Central Forum:
http://oni.bungie.org
->Select forum

Oni Wiki:
http://wiki.oni2.net

----------------------------------
Change Log:
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
