### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

include(../common.pri)

TARGET = sms
TEMPLATE = subdirs

# Themes
theme.files = images style svg
theme.path = $$M_THEME_DIR/base/meegotouch/$$TARGET/

# Icon
desktop_icon.files = icons-Applications-sms.svg
desktop_icon.path  = $$M_INSTALL_DATA/pixmaps

# Install
INSTALLS += desktop_icon \
            theme
