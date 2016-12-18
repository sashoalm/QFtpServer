QT += core
QT -= gui

TARGET = QFtpServerCommandLine
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QFtpServerLib/release/ -lQFtpServerLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QFtpServerLib/debug/ -lQFtpServerLib
else:unix: LIBS += -L$$OUT_PWD/../QFtpServerLib/ -lQFtpServerLib

INCLUDEPATH += $$PWD/../QFtpServerLib
DEPENDPATH += $$PWD/../QFtpServerLib
