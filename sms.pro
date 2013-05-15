### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

include(common.pri)

TEMPLATE = subdirs
SUBDIRS = src smsinit plugin theme translations

# Desktop
desktop_entry.files = sms.desktop
desktop_entry.path = $$M_INSTALL_DATA/applications

# smsinit autostart file
smsinit_autostart.files = smsinit.desktop
smsinit_autostart.path = $$XDG_AUTOSTART_DIR

# DBUS Service
dbus_service.files = sms.service
dbus_service.path = $$M_INSTALL_DATA/dbus-1/services/

# Install
INSTALLS += desktop_entry \
            smsinit_autostart \
            dbus_service
