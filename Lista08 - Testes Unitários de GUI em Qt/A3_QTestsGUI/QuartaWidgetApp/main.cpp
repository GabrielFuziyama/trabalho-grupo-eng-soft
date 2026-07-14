#include <QApplication>

#include "loginDialogBox.h"
#include "MyAutoMainWindow.h"

#include <QDebug>
int main(int argc, char *argv[]) // Arquitetura melhorou muito! As coisa são feitas para funcionarem assincronas e controlados por signals and slotes
{
    QApplication a(argc, argv);

    MyAutoMainWindow w;
    w.show();


    LoginDialog d;
    d.show();


    return a.exec();
}



