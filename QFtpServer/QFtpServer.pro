# Add files and directories to ship with the application 
# by adapting the examples below.
# file1.source = myfile
# dir1.source = mydir
DEPLOYMENTFOLDERS = # file1 dir1

symbian:TARGET.UID3 = 0xE6728FD6

# Smart Installer package's UID
# This UID is from the protected range 
# and therefore the package will fail to install if self-signed
# By default qmake uses the unprotected range value if unprotected UID is defined for the application
# and 0x2002CCCF value if protected UID is given to the application
#symbian:DEPLOYMENT.installer_header = 0x2002CCCF

# Allow network access on Symbian
symbian:TARGET.CAPABILITY += NetworkServices

# If your application uses the Qt Mobility libraries, uncomment
# the following lines and add the respective components to the 
# MOBILITY variable. 
# CONFIG += mobility
# MOBILITY +=

SOURCES += main.cpp \
    mainwindow.cpp \
    debuglogdialog.cpp \
    ftpthread.cpp
HEADERS += mainwindow.h \
    ftpthread.h \
    debuglogdialog.h 
FORMS += mainwindow.ui \
    debuglogdialog.ui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Please do not modify the following two lines. Required for deployment.
include(deployment.pri)
qtcAddDeployment()

RESOURCES += \
    res.qrc

OTHER_FILES += \
    cacert.pem 

# android
android:include(android/android.pri)

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../QFtpServerLib/release/ -lQFtpServerLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../QFtpServerLib/debug/ -lQFtpServerLib
else:unix: LIBS += -L$$OUT_PWD/../QFtpServerLib/ -lQFtpServerLib

INCLUDEPATH += $$PWD/../QFtpServerLib
DEPENDPATH += $$PWD/../QFtpServerLib
