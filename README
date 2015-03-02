This project started because I couldn't find a free FTP server for my Symbian phone, so I decided to try and make one of my own.

I've tested the server against Filezilla, Chrome, Firefox, Safari, Windows Explorer, and various other FTP clients.

What it supports right now:
* Both PORT and PASV.
* All the directory manipulation commands, including random access for files, so uploads and downloads can be continued without discarding already uploaded or downloaded data.
* Secure FTP, also known as FTP-ES. It uses SSL/TLS sockets.

What it doesn't support:
* IPv6.
* FTPS, because this standard is obsolete and FTP-ES should be used instead.
* Multiple user accounts. This is because it started as a phone FTP server, where this feature didn't make sense.

EVENT-DRIVEN! I am particularly proud with the fact that although the FTP server is non-blocking, it achieves it without using threads. It uses only asynchronous network I/O, so it never blocks the main thread. I've also made sure that there are no nested event loops. You just create the object in the main thread and forget about it, and it will do its job unobtrusively. You can delete it at any time. You can also create several objects listening at different ports, all living in the main thread (or any thread).

Example usage:

server = new FtpServer(this, "/ftp/root/path", 21, "MyUserName", "MyPassword", false, false);
