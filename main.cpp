#include "userswindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UsersWindow w;
    w.show();
    return a.exec();
}
