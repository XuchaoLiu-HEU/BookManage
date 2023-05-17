#include "bookserver.h"
#include <QMessageBox>
#include "ui_bookserver.h"


BookServer::BookServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BookServer)
{
    ui->setupUi(this);
    server = new QTcpServer(this);

    connect(server, &QTcpServer::newConnection, this, &BookServer::onNewConnection);

    if (!server->listen(QHostAddress::Any, 8888)) {
        qDebug() << "Failed to start server!";
    } else {
        qDebug() << "Server started";
    }
    db = Database::get_instance().database();
    if (!db.open()) {
        QMessageBox::warning(this, "Error", "Failed to connect to MySQL database.");
    }
}
void BookServer::onNewConnection()
{
    QTcpSocket *clientSocket = server->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, [clientSocket, this]() {
        onReadyRead(clientSocket);
    });
    connect(clientSocket, &QTcpSocket::disconnected, this, [clientSocket, this]() {
        onDisconnected(clientSocket);
    });
    qDebug() << "New client connected";
}
void BookServer::onReadyRead(QTcpSocket *clientSocket)
{
    QByteArray requestData = clientSocket->readAll();

    // 处理客户端的请求并返回响应数据
    QString requestString = QString::fromUtf8(requestData);
    if (requestString.startsWith("SEARCH:")) {
        QString searchKeyword = requestString.mid(7); // 提取关键词
        // 根据关键词执行图书检索操作并获取结果
        QList<Book> searchResults = performBookSearch(searchKeyword);

        // 序列化图书信息
        QByteArray responseData;
        QDataStream stream(&responseData, QIODevice::WriteOnly);
        stream << searchResults;

        // 发送响应数据给客户端
        clientSocket->write(responseData);
        clientSocket->flush();
    }
}

void BookServer::onDisconnected(QTcpSocket *clientSocket)
{
    clientSocket->deleteLater();
    qDebug() << "Client disconnected";
}

BookServer::~BookServer()
{
    delete ui;
    server->close();
}

// 在服务端的槽函数中接收客户端的请求并处理
void BookServer::handleRequest()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

    // 读取客户端发送的请求数据
    QByteArray requestData = clientSocket->readAll();
    QString request = QString::fromUtf8(requestData).trimmed();

    if (request.startsWith("SEARCH:")) {
        // 提取关键词
        QString searchKeyword = request.mid(7);

        // 根据关键词执行图书检索，并获取图书信息
        QList<Book> searchResults = performBookSearch(searchKeyword);

        // 序列化图书信息
        QByteArray responseData;
        QDataStream stream(&responseData, QIODevice::WriteOnly);
        stream << searchResults;

        // 将图书信息发送给客户端
        clientSocket->write(responseData);
        clientSocket->waitForBytesWritten();
    }

    // 关闭连接
    clientSocket->disconnectFromHost();
}
QList<Book> BookServer::performBookSearch(const QString &searchKeyword){
    QList<Book> books;
    QSqlQuery query(db);
    QString searchQuery = "SELECT * FROM book_table WHERE title LIKE :keyword OR author LIKE :keyword OR date LIKE :keyword";
    query.prepare(searchQuery);
    query.bindValue(":keyword", "%" + searchKeyword + "%");
    if (!query.exec()) {
        qDebug() << "Search error: " << query.lastError().text();
        return books;
    }

    while (query.next()) {
        Book book;
        book.title = query.value("title").toString();
        book.author = query.value("author").toString();
        book.date = query.value("date").toString();
        book.nums = query.value("nums").toString();
        books.append(book);
    }
    return books;
}
