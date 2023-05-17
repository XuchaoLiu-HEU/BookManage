#ifndef USERSWINDOW_H
#define USERSWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QtSql>
#include <QDebug>
#include "bookmanage.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class UsersWindow;
}
QT_END_NAMESPACE

class UsersWindow : public QWidget {
    Q_OBJECT

public:
    UsersWindow(QWidget *parent = nullptr);

    ~UsersWindow();

private slots:
    void registerUser();
    void login();

private:
    Ui::UsersWindow *ui;

    QLineEdit *usernameLineEdit;
    QLineEdit *passwordLineEdit;
    QSqlDatabase db;
    Manage *m;
};

#endif // USERSWINDOW_H
