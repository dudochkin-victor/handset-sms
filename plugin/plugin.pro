### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

include(../common.pri)

TEMPLATE = lib 
TARGET = sms 
QT += dbus
CONFIG += meegotouch mobility link_pkgconfig plugin
MOBILITY += contacts
PKGCONFIG += qmfmessageserver qmfclient
LIBS += -lseaside
MOC_DIR = .moc
OBJECTS_DIR = .obj
MGEN_OUTDIR = .gen

target.path = $$QMF_INSTALL_ROOT/plugins/messageservices

!exists(smshistoryinterface.h) {
    system(qdbusxml2cpp -c SmsHistoryInterface -p smshistoryinterface -i ../src/dbustypes.h -N dbus/smshistory.xml)
}
!exists(cellbroadcastinterface.h) {
    system(qdbusxml2cpp -c CellBroadcastInterface -p cellbroadcastinterface -N dbus/cellbroadcast.xml)
}
!exists(managerinterface.h) {
    system(qdbusxml2cpp -c ManagerInterface -p managerinterface -i ../src/dbustypes.h -N ../src/dbus/manager.xml)
}
!exists(messagemanagerinterface.h) {
    system(qdbusxml2cpp -c MessageManagerInterface -p messagemanagerinterface -i ../src/dbustypes.h -N ../src/dbus/messagemanager.xml)
}

HEADERS += smsservice.h \
           smssettings.h \
           smshistoryinterface.h \
           managerinterface.h \
           messagemanagerinterface.h \
           cellbroadcastinterface.h \
           ../src/dbustypes.h \
           smsconfiguration.h

SOURCES += smsservice.cpp \
           smssettings.cpp \
           smshistoryinterface.cpp \
           managerinterface.cpp \
           messagemanagerinterface.cpp \
           cellbroadcastinterface.cpp \
           ../src/dbustypes.cpp \
           smsconfiguration.cpp

INSTALLS += target

