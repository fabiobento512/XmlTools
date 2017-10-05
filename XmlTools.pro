TEMPLATE = app
CONFIG += console
CONFIG   += c++14
QT       += core
QT       -= gui # not needed
#CONFIG -= qt
#QT += qml #for use new google v8 qtscript engine
QT += script #for use old qtscript engine

# More optimization
QMAKE_CFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE += -O3

include(../CommonUtils/CommonUtils.pri)
include(../CommonLibs/CommonLibs.pri)

macx {
CONFIG -= app_bundle # We only want the binary on mac os not a bundle
}


INCLUDEPATH += ./libs

SOURCES += main.cpp \
    xmltools.cpp \
    xmlpatch.cpp \
    multidimvar.cpp \
    utilxmltools.cpp \
    xmlfilter.cpp \
    optionsparser.cpp \
    xmlcustomcode.cpp

HEADERS += main.h \
    xmltools.h \
    xmlpatch.h \
    multidimvar.h \
    utilxmltools.h \
    xmlfilter.h \
    optionsparser.h \
    xmlcustomcode.h \
    utilglobalvars.h

OTHER_FILES +=

RESOURCES += \
    resources.qrc
