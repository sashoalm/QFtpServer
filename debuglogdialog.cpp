#include "debuglogdialog.h"
#include "ui_debuglogdialog.h"

static DebugLogDialog *theDialog = 0;

// The message handler's signature has changed in Qt5.
#if QT_VERSION >= 0x050000
static void myMessageOutput(QtMsgType /*type*/, const QMessageLogContext &/*context*/, const QString &msg)
{
    theDialog->appendText(msg);
}
#else
static void myMessageOutput(QtMsgType /*type*/, const char *msg)
{
    theDialog->appendText(msg);
}
#endif

DebugLogDialog::DebugLogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebugLogDialog)
{
    ui->setupUi(this);
    theDialog = this;

    // The message handler's signature has changed in Qt5.
#if QT_VERSION >= 0x050000
    qInstallMessageHandler(myMessageOutput);
#else
    qInstallMsgHandler(myMessageOutput);
#endif
}

DebugLogDialog::~DebugLogDialog()
{
    // The message handler's signature has changed in Qt5.
#if QT_VERSION >= 0x050000
    qInstallMessageHandler(0);
#else
    qInstallMsgHandler(0);
#endif
    delete ui;
}

void DebugLogDialog::setOrientation(ScreenOrientation orientation)
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

void DebugLogDialog::showExpanded()
{
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5) || defined (Q_OS_ANDROID)
    showMaximized();
#else
    show();
#endif
}

void DebugLogDialog::appendText(const QString &text)
{
    ui->plainTextEdit->appendPlainText(text);
}

void DebugLogDialog::on_pushButton_clicked()
{
    close();
}
