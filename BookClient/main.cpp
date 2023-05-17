#include "bookclient.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BookClient w;
    w.show();
    return a.exec();
}
