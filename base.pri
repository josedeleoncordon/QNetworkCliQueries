#PHPINSTALL = REMI
#PHPINSTALL = SERVER

#CONFIG+=debug_and_release

CONFIG(release, debug|release) {
APP_BUILD_TREE = $$PWD/build/release
}
CONFIG(debug, debug|release) {
APP_BUILD_TREE = $$PWD/build/debug
}

