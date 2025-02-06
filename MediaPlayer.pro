QT += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qt-media-player
TEMPLATE = app

# Configure C++17
CONFIG += c++17

# Source files
SOURCES += \
    main.cpp \
    mediaplayer.cpp

# Header files
HEADERS += \
    MediaPlayer.h \
    mediaplayer.h

RESOURCES += \
    resources.qrc \
    resources.qrc \
    resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Platform specific configurations
win32 {
    LIBS += -lole32 -loleaut32
    RC_ICONS = player.ico  # If you have an icon file
}

macx {
    ICON = player.icns  # If you have an icon file
}

# Development configurations
CONFIG(debug, debug|release) {
    DEFINES += DEBUG
    CONFIG += warn_on
} else {
    DEFINES += QT_NO_DEBUG_OUTPUT
    CONFIG += optimize_full
}
M
# Set output directories
DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc
UI_DIR = $$PWD/build/ui

# Add resources if you have any
# RESOURCES += resources.qrc

DISTFILES += \
    style.qss
