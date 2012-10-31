include(../base.pri)
TARGET = tests
TEMPLATE = app
CONFIG += console
QT -= core \
    gui
include(../templates.pri)
include(../utilities.pri)
include(tests.pri)
