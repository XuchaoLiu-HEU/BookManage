#ifndef BOOKSERVER_H
#define BOOKSERVER_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QtSql>
#include "database.h"
//#include "bookmanage.h"

namespace Ui {
class BookServer;
}

// 图书类
class Book {
public:
    QString title;
    QString author;
    QString date;
    QString nums;
    // 将Book对象插入到数据流中
    friend QDataStream& operator<<(QDataStream& stream, const Book& book)
    {
        stream << book.title << book.author << book.date << book.nums;
        return stream;
    }

    // 从数据流中提取Book对象
    friend QDataStream& operator>>(QDataStream& stream, Book& book)
    {
        stream >> book.title >> book.author >> book.date >> book.nums;
        return stream;
    }
};

class BookServer : public QWidget
{
    Q_OBJECT

public:
    explicit BookServer(QWidget *parent = nullptr);
    ~BookServer();
public:
    void onReadyRead(QTcpSocket *clientSocket);//
    void onDisconnected(QTcpSocket *clientSocket);
    void onNewConnection();
    void handleRequest();
    QList<Book> performBookSearch(const QString &searchKeyword);
private:
    QSqlDatabase db;
    Ui::BookServer *ui;
    QTcpServer *server;
};



#endif // BOOKSERVER_H
