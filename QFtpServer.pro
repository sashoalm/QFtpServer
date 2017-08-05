TEMPLATE = subdirs
SUBDIRS += QFtpServer \
    QFtpServerLib

QFtpServer.depends = QFtpServerLib
