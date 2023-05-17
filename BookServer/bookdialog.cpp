#include "bookdialog.h"
#include "ui_bookdialog.h"
#include <QFormLayout>
#include<QVBoxLayout>

BookDialog::BookDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BookDialog)
{
    ui->setupUi(this);

    titleLineEdit = new QLineEdit(this);
    authorLineEdit = new QLineEdit(this);
    dateLineEdit = new QLineEdit(this);
    numLineEdit = new QLineEdit(this);

    // 将控件添加到对话框的布局中
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("书名:", titleLineEdit);
    formLayout->addRow("作者:", authorLineEdit);
    formLayout->addRow("出版日期:", dateLineEdit);
    formLayout->addRow("数量:", numLineEdit);

    // 创建确认按钮
    confirmButton = new QPushButton("确认", this);
    // 创建取消按钮
    cancelButton = new QPushButton("取消", this);

    // 将按钮添加到对话框布局中
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(confirmButton);
    buttonLayout->addWidget(cancelButton);

    // 连接按钮的点击信号到对应的槽函数
    connect(confirmButton, &QPushButton::clicked, this, &BookDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &BookDialog::onCancelClicked);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

QString BookDialog::getTitle() const
{
    return titleLineEdit->text();
}

QString BookDialog::getAuthor() const
{
    return authorLineEdit->text();
}

QString BookDialog::getDate() const
{
    return dateLineEdit->text();
}

QString BookDialog::getNum() const
{
    return numLineEdit->text();
}

void BookDialog::setTitle(const QString &title)
{
    titleLineEdit->setText(title);
}

void BookDialog::setAuthor(const QString &author)
{
    authorLineEdit->setText(author);
}

void BookDialog::setDate(const QString &date)
{
    dateLineEdit->setText(date);
}

void BookDialog::setNum(const QString &num){
    numLineEdit->setText(num);
}

// 确认按钮的槽函数
void BookDialog::onConfirmClicked()
{
    accept();
}

// 取消按钮的槽函数
void BookDialog::onCancelClicked()
{
    reject();
}

BookDialog::~BookDialog()
{
    delete ui;
}
