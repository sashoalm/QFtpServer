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
    static void myMessageOutput(QtMsgType type, const char *msg);
    Ui::DebugLogDialog *ui;
};

#endif // DEBUGLOGDIALOG_H
