QFtpServer - an FTP server written in Qt
----------------------------------------

### Description

QFtpServer is an event-driven server which is written entirely in Qt.
It is non-blocking and supports multiple connections.

This project started because I couldn't find a free FTP server for my Symbian
phone so I decided to try and make one of my own.  I've tested it against
Filezilla, Chrome, Firefox, Safari, Windows Explorer, and others.

### QFtpServerLib

#### Features

It supports:

* Active connections (PORT command).
* Passive connections (PASV command).
* Random access for files (APPE, RETR), so stopped uploads and downloads can
  be continued.
* Secure FTP, also known as FTP-ES which uses SSL/TLS sockets.

It doesn't support:
* IPv6.
* FTPS (this standard is obsolete and FTP-ES should be used instead).
* Multiple user accounts.

#### Example usage:

    server = new FtpServer(this, "/ftp/root/path", 21, "MyUserName",
                           "MyPassword", false, false);


### QFtpServer

#### Features

It supports:

+ Windows
+ linux
+ android
+ Symbian phone

### Build

    git clone --recursive https://github.com/sashoalm/QFtpServer.git
    cd QFtpServer
    qmake QFtpServer.pro
    make 
    
