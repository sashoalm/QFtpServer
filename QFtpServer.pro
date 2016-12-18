TEMPLATE = subdirs
SUBDIRS += QFtpServer \
    QFtpServerLib \
    QFtpServerCommandLine

QFtpServer.depends = QFtpServerLib
QFtpServerCommandLine.depends = QFtpServerLib
