#ifndef DEBUGLOGDIALOG_H
#define DEBUGLOGDIALOG_H

#include <QDialog>

namespace Ui {
class DebugLogDialog;
}

class DebugLogDialog : public QDialog
{
    Q_OBJECT

public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };

    explicit DebugLogDialog(QWidget *parent = 0);
    ~DebugLogDialog();

    // Note that this will only have an effect on Symbian and Fremantle.
    void setOrientation(ScreenOrientation orientation);

    void showExpanded();

private slots:
    void on_pushButton_clicked();

private:

    // The message handler's signature has changed in Qt5.
#if QT_VERSION >= 0x050000
    static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
#else
    static void myMessageOutput(QtMsgType type, const char *msg);
#endif

    Ui::DebugLogDialog *ui;
};

#endif // DEBUGLOGDIALOG_H
