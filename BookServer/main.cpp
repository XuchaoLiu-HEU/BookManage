#include "userswindow.h"
#include <QApplication>
#include "bookserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    UsersWindow w;
    w.show();
//    BookServer s;
//    s.show();
    return a.exec();
}
