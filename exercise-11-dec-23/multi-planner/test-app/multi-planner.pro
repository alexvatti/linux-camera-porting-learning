TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

V4LPATH = $${PWD}/v4l-utils
V4LINCS = $${V4LPATH}/include
V4LUTILS = $${V4LPATH}/utils
V4LUCOMMON = $${V4LUTILS}/common

INCLUDEPATH += \
    $${V4LINCS} \
    $${V4LUCOMMON}

SOURCES += \
        test-app.c
