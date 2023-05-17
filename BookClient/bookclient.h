#ifndef BOOKCLIENT_H
#define BOOKCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
QT_BEGIN_NAMESPACE
namespace Ui { class BookClient; }
QT_END_NAMESPACE

class BookClient : public QMainWindow
{
    Q_OBJECT

public:
    BookClient(QWidget *parent = nullptr);
    ~BookClient();
public slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();
    void onBorrowButtonClicked();
    void onSearchButtonClicked();


public:
    void createBookTable();
    void connectToServer();
//    void sendRecommendationAndReminderRequest();
private:
    QTcpSocket *socket;
    QLineEdit *searchLineEdit;
    QPushButton *searchButton;
    QPushButton *borrowButton;
    QTableWidget *bookTable;
private:
    Ui::BookClient *ui;
};
#endif // BOOKCLIENT_H
