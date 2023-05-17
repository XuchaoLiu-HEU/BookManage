#ifndef BOOKDIALOG_H
#define BOOKDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
namespace Ui {
class BookDialog;
}

class BookDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookDialog(QWidget *parent = nullptr);
    ~BookDialog();
public slots:
    void onConfirmClicked();
    void onCancelClicked();
public:
    QString getTitle() const;
    QString getAuthor() const;
    QString getDate() const;
    QString getNum() const;

    void setTitle(const QString &title);
    void setAuthor(const QString &author);
    void setDate(const QString &date);
    void setNum(const QString &num);

private:
    Ui::BookDialog *ui;
public:
    QLineEdit *titleLineEdit; // 用于输入书名的 QLineEdit 控件
    QLineEdit *authorLineEdit; // 用于输入作者的 QLineEdit 控件
    QLineEdit *dateLineEdit; // 用于输入出版日期的 QLineEdit 控件
    QLineEdit *numLineEdit;
    QPushButton *confirmButton;
    QPushButton *cancelButton;
};

#endif // BOOKDIALOG_H
