### eqmake4 was here ###
CONFIG -= debug_and_release debug
CONFIG += release

# Paths
M_PREFIX = /usr
M_INSTALL_BIN = $$INSTALL_ROOT$$M_PREFIX/bin/
M_INSTALL_DATA = $$INSTALL_ROOT$$M_PREFIX/share/
M_THEME_DIR = $$M_INSTALL_DATA/themes/

XDG_AUTOSTART_DIR = $$INSTALL_ROOT/etc/xdg/autostart/

# This variable was being set previously as /usr/lib/qt4/...
# which is not right.  It should be in /usr/lib/qmf/...
# So we set that unconditionally.
#isEmpty(QMF_INSTALL_ROOT) {
    QMF_INSTALL_ROOT = $$INSTALL_ROOT$$M_PREFIX/lib/qmf
#}

