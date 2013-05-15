### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

include(../common.pri)

TEMPLATE = app
TARGET = smsinit
QT += dbus
CONFIG += link_pkgconfig
PKGCONFIG += qmfmessageserver qmfclient
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen

target.path = $$M_INSTALL_BIN

SOURCES += main.cpp \
           smsinit.cpp

HEADERS += smsinit.h

MAKE_CLEAN += $$OBJECTS_DIR/*.o
MAKE_DISTCLEAN += \
        $$MOC_DIR/* $$MOC_DIR \
        $$OBJECTS_DIR/* $$OBJECTS_DIR \
        $$MGEN_OUTDIR/* $$MGEN_OUTDIR

# Install
INSTALLS += target
