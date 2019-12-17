TEMPLATE = subdirs
SUBDIRS += QFtpServer \
    QFtpServerLib \
    QFtpServerCommandLine

QFtpServer.depends = QFtpServerLib
QFtpServerCommandLine.depends = QFtpServerLib

OTHER_FILES += README.md \
    .travis.yml \
    appveyor.yml \
    ci/* \
    tag.sh
