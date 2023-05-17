#include "bookclient.h"
#include "ui_bookclient.h"
#include <QVBoxLayout>
#include <QMessageBox>

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

BookClient::BookClient(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::BookClient)
{
    ui->setupUi(this);


    searchLineEdit = new QLineEdit("输入关键词搜索或者借阅图书，输入空位检索所有图书",this);
    searchButton = new QPushButton("搜索图书",this);
    borrowButton = new QPushButton("借阅图书",this);
    createBookTable();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(bookTable);
    layout->addWidget(searchLineEdit);

    QWidget *buttonContainer = new QWidget(this);  // 创建容器部件
    QHBoxLayout *hlayout = new QHBoxLayout(buttonContainer);
    hlayout->addWidget(searchButton);
    hlayout->addWidget(borrowButton);

    layout->addWidget(buttonContainer);
    layout->setContentsMargins(0, 0, 0, 0);  // 设置布局的边距为0
    layout->setSpacing(0);  // 设置布局的间距为0

    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);  // 设置布局所依附的中央部件

    setLayout(layout);

    // 连接搜索按钮的点击信号和槽函数
    connect(searchButton, &QPushButton::clicked, this, &BookClient::onSearchButtonClicked);
    // 连接借阅按钮的点击信号和槽函数
    connect(borrowButton, &QPushButton::clicked, this, &BookClient::onBorrowButtonClicked);

    // 建立与服务器的连接
    connectToServer();
}

BookClient::~BookClient()
{
    delete ui;
}

void BookClient::createBookTable(){
    bookTable = new QTableWidget(this);
    bookTable->setColumnCount(4);
    bookTable->setRowCount(0);
    bookTable->setHorizontalHeaderLabels(QStringList() << "书名" << "作者" << "出版日期" << "数量");
    bookTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    bookTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // 设置样式
    bookTable->setStyleSheet("QTableView { background-color: #f2f2f2; }"
                             "QHeaderView::section { background-color: #d3d3d3; }"
                             "QTableView::item:selected { background-color: #a6a6a6; }");

    // 调整行高和列宽
    bookTable->verticalHeader()->setDefaultSectionSize(24);
    int tableWidth = this->width() / 4;
    for (int i = 0; i < bookTable->columnCount(); ++i) {
        //        bookTable->setColumnWidth(i, tableWidth);
        bookTable->horizontalHeader()->setDefaultSectionSize(tableWidth);
    }
}


// 与服务器建立连接
void BookClient::connectToServer()
{
    // 创建套接字
    socket = new QTcpSocket(this);
    // 连接到服务器
    socket->connectToHost("127.0.0.1", 8888);

    // 连接成功时执行相应的操作
    connect(socket, &QTcpSocket::connected, this, &BookClient::onConnected);
    // 接收到服务器返回数据时执行相应的操作
    connect(socket, &QTcpSocket::readyRead, this, &BookClient::onReadyRead);
    // 连接断开时执行相应的操作
    connect(socket, &QTcpSocket::disconnected, this, &BookClient::onDisconnected);

    // 连接失败时执行相应的操作
    connect(socket, &QTcpSocket::errorOccurred, this, [](){
        qDebug() << "Connected to server erro";
        QMessageBox::warning(nullptr,"警告", "服务器连接失败");
    });
}

// 与服务器连接成功的槽函数
void BookClient::onConnected()
{
    qDebug() << "Connected to server";
    QMessageBox::information(nullptr,"提示", "服务器连接成功");
}

// 接收到服务器返回数据的槽函数
void BookClient::onReadyRead()
{
    // QByteArray responseData = socket->readAll();
    // 解析并处理服务器返回的数据
}

// 连接断开的槽函数
void BookClient::onDisconnected()
{
    qDebug() << "Disconnected from server";
}

// 在图书检索界面的搜索按钮点击事件中发送图书检索请求给服务端
void BookClient::onSearchButtonClicked()
{
    // 获取用户输入的关键词
    QString searchKeyword = searchLineEdit->text();

    // 向服务端发送图书检索请求
    QString requestData = "SEARCH:" + searchKeyword;
    socket->write(requestData.toUtf8());
    if (!socket->waitForBytesWritten()) {
        qDebug() << "Failed to send search request!";
        return;
    }

    // 等待服务端返回的图书信息
    if (socket->waitForReadyRead()) {
        QByteArray responseData = socket->readAll();
        // 解析并显示图书信息在图书列表中
        // ...
        // 反序列化图书信息
        QList<Book> books;
        QDataStream stream(&responseData, QIODevice::ReadOnly);
        stream >> books;

        // 清空客户端的bookTable
        bookTable->clearContents();
        bookTable->setRowCount(books.size());

        // 将图书信息显示在客户端的bookTable中
        for (int i = 0; i < books.size(); ++i) {
            const Book& book = books[i];
            QTableWidgetItem* titleItem = new QTableWidgetItem(book.title);
            QTableWidgetItem* authorItem = new QTableWidgetItem(book.author);
            QTableWidgetItem* dateItem = new QTableWidgetItem(book.date);
            QTableWidgetItem* quantityItem = new QTableWidgetItem(book.nums);

            bookTable->setItem(i, 0, titleItem);
            bookTable->setItem(i, 1, authorItem);
            bookTable->setItem(i, 2, dateItem);
            bookTable->setItem(i, 3, quantityItem);
        }
    }
    else {
        qDebug() << "Failed to receive search response!";
    }
}

// 在图书列表的借阅按钮点击事件中发送图书借阅请求给服务端
void BookClient::onBorrowButtonClicked()
{
    /*
    // 获取选中的图书ID
    int bookId = 0;

    // 向服务端发送图书借阅请求
    QString borrowRequest = "BORROW:" + QString::number(bookId);
    socket->write(borrowRequest.toUtf8());
    if (!socket->waitForBytesWritten()) {
        qDebug() << "Failed to send borrow request!";
        return;
    }

    // 等待服务端返回的借阅结果
    if (socket->waitForReadyRead()) {
        QByteArray borrowResponse = socket->readAll();
        // 解析并处理服务端返回的借阅结果
        // ...
    } else {
        qDebug() << "Failed to receive borrow response!";
    }*/
}

/*
// 定期向服务端发送图书推荐和到期提醒请求
void BookClient::sendRecommendationAndReminderRequest()
{
    // 向服务端发送图书推荐请求
    QString recommendationRequest = "RECOMMENDATION";
    socket->write(recommendationRequest.toUtf8());
    if (!socket->waitForBytesWritten()) {
        qDebug() << "Failed to send recommendation request!";
        return;
    }

    // 等待服务端返回的图书推荐信息
    if (socket->waitForReadyRead()) {
        QByteArray recommendationResponse = socket->readAll();
        // 解析并处理服务端返回的推荐信息
        // ...
    } else {
        qDebug() << "Failed to receive recommendation response!";
    }

    // 向服务端发送到期提醒请求
    QString reminderRequest = "REMINDER";
    socket->write(reminderRequest.toUtf8());
    if (!socket->waitForBytesWritten()) {
        qDebug() << "Failed to send reminder request!";
        return;
    }

    // 等待服务端返回的到期提醒信息
    if (socket->waitForReadyRead()) {
        QByteArray reminderResponse = socket->readAll();
        // 解析并处理服务端返回的到期提醒信息
        // ...
    } else {
        qDebug() << "Failed to receive reminder response!";
    }
}*/
