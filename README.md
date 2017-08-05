QFtpServer - an FTP server written in Qt
----------------------------------------

This project started because I couldn't find a free FTP server for my Symbian phone so I decided to try and make one of my own. I've tested it against Filezilla, Chrome, Firefox, Safari, Windows Explorer, and others.

It supports:

* Active connections.
* Passive connections.
* Random access for files, so stopped uploads and downloads can be continued.
* Secure FTP, also known as FTP-ES which uses SSL/TLS sockets.

What it doesn't support:
* IPv6.
* FTPS, because this standard is obsolete and FTP-ES should be used instead.
* Multiple user accounts.

Although the FTP server is non-blocking, it achieves it without using threads. It uses only asynchronous network I/O, so it never blocks the main thread. There are no nested event loops. You just create the object in the main thread and forget about it, and it will do its job unobtrusively. You can delete it at any time. You can also create several objects listening at different ports, all living in the main thread (or any thread).

Example usage:

    server = new FtpServer(this, "/ftp/root/path", 21, "MyUserName", "MyPassword", false, false);
