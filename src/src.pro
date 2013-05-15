### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

include(../common.pri)

TEMPLATE = app
TARGET = sms
QT += dbus
CONFIG += meegotouch mobility link_pkgconfig
MOBILITY += contacts
PKGCONFIG += qmfmessageserver qmfclient
LIBS += -lseaside
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen

target.path = $$M_INSTALL_BIN

!exists(managerinterface.h) {
    system(qdbusxml2cpp -c ManagerInterface -p managerinterface -i dbustypes.h -N dbus/manager.xml)
}
!exists(dialerinterface.h) {
    system(qdbusxml2cpp -c DialerInterface -p dialerinterface -N dbus/com.meego.dialer.xml)
}
!exists(messagemanagerinterface.h) {
    system(qdbusxml2cpp -c MessageManagerInterface -p messagemanagerinterface -i ../src/dbustypes.h -N ../src/dbus/messagemanager.xml)
}

STYLE_HEADERS += contactitemstyle.h \
                 inboxitemstyle.h \
                 dialogitemstyle.h \
                 dialogliststyle.h

SOURCES += main.cpp \
           smsapplication.cpp \
           smsdbusadaptor.cpp \
           managerinterface.cpp \
           messagemanagerinterface.cpp \
           dialerinterface.cpp \
           dbustypes.cpp \
           inboxpage.cpp \
           dialogpage.cpp \
           settingspage.cpp \
           contactdata.cpp \
           contactitem.cpp \
           contactitemmodel.cpp \
           contactsortfiltermodel.cpp \
           contactitemcreator.cpp \
           backend.cpp \
           smsdata.cpp \
           smsthread.cpp \
           inboxitem.cpp \
           inboxitemmodel.cpp \
           inboxitemcreator.cpp \
           dialogitem.cpp \
           dialoglist.cpp \
           util.cpp \
    highlighter.cpp

HEADERS += smsapplication.h \
           smsdbusadaptor.h \
           managerinterface.h \
           messagemanagerinterface.h \
           dialerinterface.h \
           dbustypes.h \
           inboxpage.h \
           dialogpage.h \
           settingspage.h \
           contactdata.h \
           contactitem.h \
           contactitemmodel.h \
           contactsortfiltermodel.h \
           contactitemcreator.h \
           backend.h \
           smsdata.h \
           smsthread.h \
           inboxitem.h \
           inboxitemmodel.h \
           inboxitemcreator.h \
           dialogitem.h \
           dialoglist.h \
           util.h \
           $$STYLE_HEADERS \
    highlighter.h

MAKE_CLEAN += $$OBJECTS_DIR/*.o
MAKE_DISTCLEAN += \
        $$MOC_DIR/* $$MOC_DIR \
        $$OBJECTS_DIR/* $$OBJECTS_DIR \
        $$MGEN_OUTDIR/* $$MGEN_OUTDIR

# Notification
notifications.files = notifications/*.conf
notifications.path = $$M_INSTALL_DATA/meegotouch/notifications/eventtypes/

# Install
INSTALLS += target notifications
