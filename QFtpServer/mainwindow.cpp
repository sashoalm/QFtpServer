#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ftpserver.h"
#include "debuglogdialog.h"

#if defined(Q_OS_ANDROID)
    #include "AndroidUtils.h"
    #include "AndroidDirectory.h"
#endif

#include <QApplication>
#include <QSettings>
#include <QFileDialog>
#include <QIntValidator>
#include <QMessageBox>
#include <QDebug>
#include <QImage>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      server(nullptr),
      m_TrayIcon(this),
      m_TrayIconMenu(this)
{
    ui->setupUi(this);
    
    m_nCloseType = UNKNOW;
    
    ui->lineEditPort->setValidator(new QIntValidator(1, 65535, this));

#if defined(Q_OS_ANDROID)
    ui->checkBoxDisableLockScreen->checkState() == Qt::Checked
            ? CAndroidUtils::PowerWakeLock() : CAndroidUtils::PowerWakeLock(false);
    CAndroidUtils::InitPermissions();
    connect(qApp, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
            this, SLOT(slotApplicationStateChanged(Qt::ApplicationState)));
    // Fix for the bug android keyboard bug - see
    // http://stackoverflow.com/q/21074012/492336.
    foreach (QLineEdit *lineEdit, findChildren<QLineEdit*>()) {
        connect(lineEdit, SIGNAL(editingFinished()),
                QGuiApplication::inputMethod(), SLOT(hide()));
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)) && (QT_VERSION < QT_VERSION_CHECK(5, 3, 0))
    // Fix for bug in Qt 5.2 - Virtual keyboard keeps switching to
    // uppercase when I type in a QLineEdit, see
    // http://stackoverflow.com/q/25263561/492336.
    // The bug is fixed in Qt 5.4, I don't know about Qt 5.3.
    foreach (QLineEdit *lineEdit, findChildren<QLineEdit*>()) {
        lineEdit->setInputMethodHints(Qt::ImhNoAutoUppercase);
    }
#endif
#else
    // The exit button is needed only for Android. Hide it for other builds.
    ui->pushButtonExit->hide();
    ui->checkBoxDisableLockScreen->hide();
#endif // Q_OS_ANDROID

    // Set window icon.
    setWindowIcon(QIcon(":/icons/appicon"));
    if(QSystemTrayIcon::isSystemTrayAvailable())
    {
        bool check = connect(&m_TrayIcon,
                  SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                  SLOT(slotTrayIconActive(QSystemTrayIcon::ActivationReason)));
        Q_ASSERT(check);
        QAction* pExit = m_TrayIconMenu.addAction(
                    QIcon(":/icons/close"),
                    tr("Exit"),
                    this,
                    SLOT(slotActionExit(bool)));
        Q_ASSERT(pExit);
        m_TrayIcon.setContextMenu(&m_TrayIconMenu);
        m_TrayIcon.setToolTip(tr("QFtpServer"));
        m_TrayIcon.setIcon(this->windowIcon());
        m_TrayIcon.show();
    }
    
    loadSettings();

    startServer();
}

MainWindow::~MainWindow()
{
    if(server)
    {
        server->quit();
    }
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
#if QT_VERSION < 0x040702 || QT_VERSION >= 0x050000
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    // Qt 5 has removed them.
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
#elif defined(Q_WS_MAEMO_5) || defined (Q_OS_ANDROID)
    showMaximized();
#else
    showNormal();
#endif
}

void MainWindow::loadSettings()
{
    // UNIX-derived systems such as Linux and Android don't allow access to
    // port 21 for non-root programs, so we will use port 2121 instead.
    QString defaultPort;
#if defined(Q_OS_UNIX)
    defaultPort = "2121";
#else
    defaultPort = "21";
#endif

    QSettings settings;
    ui->lineEditPort->setText(settings.value("settings/port", defaultPort).toString());
    ui->lineEditUserName->setText(settings.value("settings/username", "admin").toString());
    ui->lineEditPassword->setText(settings.value("settings/password", "qt").toString());
    QString rootPath = QDir::rootPath();
#ifdef Q_OS_ANDROID
    QDir root(CAndroidDirectory::GetExternalStorageDirectory());
    if(root.exists())
        rootPath = CAndroidDirectory::GetExternalStorageDirectory();
#endif 
    ui->lineEditRootPath->setText(settings.value("settings/rootpath", rootPath).toString());
    //ui->lineEditRootPath->setText(settings.value("settings/rootpath", QDir::rootPath()).toString());
    ui->checkBoxAnonymous->setChecked(settings.value("settings/anonymous", false).toBool());
    ui->checkBoxReadOnly->setChecked(settings.value("settings/readonly", false).toBool());
    ui->checkBoxOnlyOneIpAllowed->setChecked(settings.value("settings/oneip", true).toBool());
#ifndef Q_OS_ANDROID
    m_nCloseType = settings.value("settings/closetype", UNKNOW).toInt();
#endif
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("settings/port", ui->lineEditPort->text());
    settings.setValue("settings/username", ui->lineEditUserName->text());
    settings.setValue("settings/password", ui->lineEditPassword->text());
    settings.setValue("settings/rootpath", ui->lineEditRootPath->text());
    settings.setValue("settings/anonymous", ui->checkBoxAnonymous->isChecked());
    settings.setValue("settings/readonly", ui->checkBoxReadOnly->isChecked());
    settings.setValue("settings/oneip", ui->checkBoxOnlyOneIpAllowed->isChecked());
#ifndef Q_OS_ANDROID
    settings.setValue("settings/closetype", m_nCloseType);
#endif
}

void MainWindow::startServer()
{
    QString userName;
    QString password;
    if (!ui->checkBoxAnonymous->isChecked()) {
        userName = ui->lineEditUserName->text();
        password = ui->lineEditPassword->text();
    }

    if(server)
    {
        server->quit();

    }
    //delete server;
    server = new CFtpThread(ui->lineEditRootPath->text(),
                                   ui->lineEditPort->text().toInt(),
                                   userName,
                                   password,
                                   ui->checkBoxReadOnly->isChecked(),
                                   ui->checkBoxOnlyOneIpAllowed->isChecked());
    bool check = connect(server, SIGNAL(sigNewPeerIp(const QString&)),
                         this, SLOT(onPeerIpChanged(const QString&)));
    Q_ASSERT(check);
    check = connect(server, SIGNAL(sigMessage(const QString&)),
                    this, SLOT(onMessage(const QString&)));
    Q_ASSERT(check);
    check = connect(server, SIGNAL(finished()),
                    server, SLOT(deleteLater()));
    Q_ASSERT(check);
    server->start();
}

void MainWindow::on_pushButtonRestartServer_clicked()
{
    startServer();
}

void MainWindow::on_toolButtonBrowse_clicked()
{
    QString rootPath;
#ifdef Q_OS_ANDROID
    // In Android, the file dialog is not shown maximized by the static
    // function, which looks weird, since the dialog doesn't have borders or
    // anything. To make sure it's shown maximized, we won't be using
    // QFileDialog::getExistingDirectory().
    QFileDialog dialog;
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);
    dialog.showMaximized();
    dialog.exec();
    if (!dialog.selectedFiles().isEmpty()) {
        rootPath = dialog.selectedFiles().front();
    }
#else
    rootPath = QFileDialog::getExistingDirectory(this, QString(), ui->lineEditRootPath->text());
#endif
    if (rootPath.isEmpty()) {
        return;
    }
    ui->lineEditRootPath->setText(rootPath);
}

void MainWindow::onPeerIpChanged(const QString &peerIp)
{
    ui->statusBar->showMessage("Connected to " + peerIp);
}

void MainWindow::onMessage(const QString& msg)
{
    ui->statusBar->showMessage(msg);
}

void MainWindow::on_pushButtonShowDebugLog_clicked()
{
    DebugLogDialog *dlg = new DebugLogDialog;
    dlg->setAttribute( Qt::WA_DeleteOnClose, true );
    dlg->setModal(true);
    dlg->showExpanded();
}

void MainWindow::slotTrayIconActive(QSystemTrayIcon::ActivationReason e)
{
    if(QSystemTrayIcon::Trigger == e)
    {
        showExpanded();
    }
}

#if !defined(Q_OS_ANDROID)
void MainWindow::closeEvent(QCloseEvent *e)
{   
    if(EXIT & m_nCloseType)
    {
        m_nCloseType &= ~EXIT;
        return;
    }
    switch(m_nCloseType)
    {
        case UNKNOW:
        {    
            QMessageBox msg(QMessageBox::Question,
                        tr("Close"),
                        tr("Is closed the programe or hidden programe windows?"),
                        QMessageBox::Close | QMessageBox::Yes | QMessageBox::Cancel,
                        this);

            msg.setButtonText(QMessageBox::Yes, tr("Hidden"));
            msg.setButtonText(QMessageBox::Cancel, tr("Cancel"));
            msg.setDefaultButton(QMessageBox::Close);
    #if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
            QCheckBox cbSave(tr("Save the setting"), &msg);
            msg.setCheckBox(&cbSave);
    #endif
            int nRet = msg.exec();
            switch(nRet)
            {
            case QMessageBox::Yes:
            {
                this->hide();
                m_nCloseType = HIDE;
                e->ignore();
                break;
            }
            case QMessageBox::Cancel:
            {
                e->ignore();
                break;
            }
            default:
                m_nCloseType = CLOSE;
            }
          
    #if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
            if(!msg.checkBox()->isChecked())
                m_nCloseType = UNKNOW;
    #endif
            
            break;
        }
        case HIDE:
        {
            this->hide();
            e->ignore();
            break;
        }
    }
}
#endif

void MainWindow::slotActionExit(bool checked)
{
    Q_UNUSED(checked);
    on_pushButtonExit_clicked();
}

void MainWindow::on_pushButtonExit_clicked()
{
    m_nCloseType |= EXIT;
    close();
}

#if defined(Q_OS_ANDROID)
void MainWindow::slotApplicationStateChanged(Qt::ApplicationState state)
{
    qDebug() << "stats: " << state << " closetype: " << m_nCloseType;
    if(EXIT & m_nCloseType)
    {
        m_nCloseType &= ~EXIT;
        return;
    }

    if(Qt::ApplicationState::ApplicationInactive == state)
        m_Notification.Show(tr("QFtpServer is running in backgroup"),
                            tr("QFtpServer"),
                            0,
                            QImage(":/icons/appicon"),
                            QImage(":/icons/appicon")
                            );
    if(Qt::ApplicationState::ApplicationActive == state)
        m_Notification.Cancel();
}
#endif

#if defined (Q_OS_ANDROID)
void MainWindow::on_checkBoxDisableLockScreen_stateChanged(int arg1)
{
    if(Qt::Checked == arg1)
        CAndroidUtils::PowerWakeLock();
    if(Qt::Unchecked == arg1) {
        CAndroidUtils::PowerWakeLock(false);
    }
}
#endif
