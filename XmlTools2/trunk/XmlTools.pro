TEMPLATE = app
CONFIG += console
QT       += core
QT       -= gui # not needed
#CONFIG -= qt
#QT += qml #for use new google v8 qtscript engine
QT += script #for use old qtscript engine
QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp #OpenMP (multithreading) support

# More optimization
QMAKE_CFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_RELEASE += -O3




macx {
CONFIG -= app_bundle # We only want the binary on mac os not a bundle
}


INCLUDEPATH += ./libs

SOURCES += main.cpp \
    libs/pugixml.cpp \
    xmltools.cpp \
    util.cpp \
    xmlpatch.cpp \
    multidimvar.cpp \
    utilxmltools.cpp \
    xmlfilter.cpp \
    optionsparser.cpp \
    xmlcustomcode.cpp

HEADERS += main.h \
    libs/pugiconfig.hpp \
    libs/pugixml.hpp \
    util.h \
    xmltools.h \
    xmlpatch.h \
    multidimvar.h \
    utilxmltools.h \
    xmlfilter.h \
    optionsparser.h \
    xmlcustomcode.h

OTHER_FILES +=

RESOURCES += \
    resources.qrc
