#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Needed for QSettings.
    app.setOrganizationName("CodeThesis");
    app.setApplicationName("QFtpServer");

    // Show the main window.
    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
    mainWindow.showExpanded();

    return app.exec();
}
