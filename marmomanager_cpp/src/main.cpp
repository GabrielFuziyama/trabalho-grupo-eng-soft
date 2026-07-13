#include <QApplication>
#include "database.h"
#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    // Initialize Database (Singleton)
    Database::getInstance().initDb();

    // Create and show Main Window
    MarmoManagerApp mainWindow;
    mainWindow.show();

    return app.exec();
}
