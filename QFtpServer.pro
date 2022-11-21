TEMPLATE = subdirs
SUBDIRS += QFtpServer \
    QFtpServerLib \
    QFtpServerCommandLine

QFtpServer.depends = QFtpServerLib
QFtpServerCommandLine.depends = QFtpServerLib

equals(QT_MAJOR_VERSION, 4) {
    SUBDIRS += QFtpServerTests
    QFtpServerTests.depends = QFtpServerLib
} else {
    message(Unit tests project is available only for Qt4 because it depends on QFtp)
}

OTHER_FILES += README.md \
    .travis.yml \
    appveyor.yml \
    ci/* \
    tag.sh \
    CMakeLists.txt
