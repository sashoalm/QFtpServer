#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpserver.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostAddress>
#include <QtGui/QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadSettings();
    server = 0;
    startServer();
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
}

void MainWindow::loadSettings()
{
    QSettings settings;
    ui->spinBoxPort->setValue(settings.value("settings/port", 21).toInt());
    ui->lineEditUserName->setText(settings.value("settings/username", "admin").toString());
    ui->lineEditPassword->setText(settings.value("settings/password", "qt").toString());
    ui->lineEditRootPath->setText(settings.value("settings/rootpath", QDir::rootPath()).toString());
    ui->checkBoxAnonymous->setChecked(settings.value("settings/anonymous", false).toBool());
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("settings/port", ui->spinBoxPort->value());
    settings.setValue("settings/username", ui->lineEditUserName->text());
    settings.setValue("settings/password", ui->lineEditPassword->text());
    settings.setValue("settings/rootpath", ui->lineEditRootPath->text());
    settings.setValue("settings/anonymous", ui->checkBoxAnonymous->isChecked());
}

void MainWindow::startServer()
{
    QString userName;
    QString password;
    if (!ui->checkBoxAnonymous->isChecked()) {
        userName = ui->lineEditUserName->text();
        password = ui->lineEditPassword->text();
    }
    delete server;
    server = new FtpServer(this, ui->lineEditRootPath->text(), ui->spinBoxPort->value(), userName, password);
    connect(server, SIGNAL(newPeerIp(QString)), SLOT(onPeerIpChanged(QString)));
    if (server->isListening())
        ui->statusBar->showMessage("Listening at " + lanIp());
    else
        ui->statusBar->showMessage("Not listening");
}

QString MainWindow::lanIp()
{
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
            return address.toString();
    }
    return "";
}

void MainWindow::on_pushButtonRestartServer_clicked()
{
    startServer();
}

void MainWindow::on_toolButtonBrowse_clicked()
{
    QString rootPath = QFileDialog::getExistingDirectory(this, QString(), ui->lineEditRootPath->text());
    if (rootPath.isEmpty())
        return;
    ui->lineEditRootPath->setText(rootPath);
}

void MainWindow::onPeerIpChanged(const QString &peerIp)
{
    ui->statusBar->showMessage("Connected to " + peerIp);
}
