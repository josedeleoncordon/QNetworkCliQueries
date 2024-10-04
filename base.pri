#PHPINSTALL = REMI
#PHPINSTALL = SERVER

#CONFIG+=debug_and_release
#DEFINES += QT_NO_INFO_OUTPUT

CONFIG(release, debug|release) {
APP_BUILD_TREE = $$PWD/build/release
DEFINES += QT_NO_DEBUG_OUTPUT
}
CONFIG(debug, debug|release) {
APP_BUILD_TREE = $$PWD/build/debug
}

