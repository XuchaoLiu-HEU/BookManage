#ifndef BOOKMANAGE_H
#define BOOKMANAGE_H

#include <QHeaderView>
#include <QFont>
#include <QtSql>
#include <QMessageBox>
#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "bookdialog.h"

namespace Ui {
class Manage;
}

class Manage : public QWidget
{
    Q_OBJECT

public:
    explicit Manage(QWidget *parent = nullptr);
    ~Manage();

public:
    void searchBooks(const QString &keyword);
    bool addBook(const QString &title, const QString &author, const QString &date, const QString &num);
    bool editBook(const QString &title,  const QString &newNum);
    bool deleteBook(const QString &title, const QString &author);

public slots:
    void on_addButton_clicked();
    void on_editButton_clicked();
    void on_deleteButton_clicked();

private:
    void createBookTable();
    void createSearchBox();
    void createAddBookDialog();
    void createEditBookDialog();
    void createBookDetailsDialog();
    void refreshBookTable();

    QTableWidget *bookTable;
    QLineEdit *searchBox;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;

//    BookDialog *bookDialog;

    QSqlDatabase db;

private:
    Ui::Manage *ui;
};

#endif // BOOKMANAGE_H


