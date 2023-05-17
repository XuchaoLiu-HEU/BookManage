#include "userswindow.h"
#include "ui_userswindow.h"
#include "database.h"


UsersWindow::UsersWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::UsersWindow)
{
    ui->setupUi(this);

    // 创建标题标签
    QLabel *titleLabel = new QLabel("欢迎登录");
    QFont titleFont("Arial", 20, QFont::Bold);
    titleLabel->setFont(titleFont);

    // 创建用户名标签和输入框
    QLabel *usernameLabel = new QLabel("用户名:");
    usernameLineEdit = new QLineEdit();

    // 创建密码标签和输入框
    QLabel *passwordLabel = new QLabel("密码:");
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);

    // 创建注册链接
    QLabel *registerLabel = new QLabel("<a href=\"#\">注册新用户</a>");
    registerLabel->setTextFormat(Qt::RichText);
    registerLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    registerLabel->setOpenExternalLinks(true);

    // 创建登录按钮和注册按钮
    QPushButton *loginButton = new QPushButton("登录");
    QPushButton *registerButton = new QPushButton("注册");

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(titleLabel, 0, Qt::AlignCenter);
    layout->addSpacing(20);
    layout->addWidget(usernameLabel);
    layout->addWidget(usernameLineEdit);
    layout->addWidget(passwordLabel);
    layout->addWidget(passwordLineEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);
    layout->addWidget(registerLabel);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    // 设置布局和样式
    setLayout(layout);
//    setStyleSheet("QLabel { font-size: 14px; }"
//                  "QPushButton { padding: 8px 16px; }"
//                  "QPushButton:hover { background-color: #aaffaa; }"
//                  "QLineEdit { padding: 4px; }");
    setStyleSheet(R"(
           QLabel {
               font-size: 18px;
           }
           QLineEdit {
               padding: 6px;
               border: 1px solid #ccc;
               border-radius: 3px;
               font-size: 14px;
           }
           QPushButton {
               padding: 8px 16px;
               border-radius: 3px;
               font-size: 14px;
               background-color: #4CAF50;
               color: white;
           }
           QPushButton:hover {
               background-color: #45a049;
           }
           QLabel a {
               color: #0645AD;
           }
           )");

    setWindowTitle("登录");
    setFixedSize(600, 400);

    // 创建数据库
    db = Database::get_instance().database();
    // 打开数据库连接
    if (!db.open()) {
        QMessageBox::warning(nullptr, "警告", "数据库打开失败");
    }
    else {
        // 创建表
        QString createTable = R"(
            CREATE TABLE IF NOT EXISTS user_table (
                id SERIAL PRIMARY KEY,
                name VARCHAR(255),
                password INT(10) )
        )";
        QSqlQuery query(db);
        if (!query.exec(createTable)) {
            qDebug() << "Create table error: " << query.lastError().text();
        }
        else {
            qDebug() << "Table created.";
        }

        // 注册按钮点击事件
        connect(registerButton, &QPushButton::clicked, this, &UsersWindow::registerUser);
        // 登录按钮点击事件
        connect(loginButton, &QPushButton::clicked, this, &UsersWindow::login);
    }
}

UsersWindow::~UsersWindow()
{
    delete ui;
}

void UsersWindow::registerUser()
{
    QString name = usernameLineEdit->text();
    QString pwd = passwordLineEdit->text();

    if (name.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(nullptr, "警告", "输入用户名或密码！");
    }
    else {
        QSqlQuery query(db);
        query.prepare("SELECT name FROM user_table WHERE name = :name");
        query.bindValue(":name", name);
        query.exec();

        if (query.next()) {
            QMessageBox::warning(nullptr, "警告", "用户已注册！");
        }
        else {
            query.prepare("INSERT INTO user_table (name, password) VALUES (:name, :password)");
            query.bindValue(":name", name);
            query.bindValue(":password", pwd);
            if (query.exec()) {
                QMessageBox::information(nullptr, "提示", "注册成功");
                usernameLineEdit->clear();
                passwordLineEdit->clear();
            }
            else {
                qDebug() << "Insert into table error: " << query.lastError().text();
            }
        }
    }
}

void UsersWindow::login()
{
    QString name = usernameLineEdit->text();
    QString pwd = passwordLineEdit->text();
    if (name.isEmpty() || pwd.isEmpty()) {
        QMessageBox::warning(nullptr, "警告", "输入用户名或密码！");
    }
    else {
        QSqlQuery query(db);
        query.prepare("SELECT name, password FROM user_table WHERE name = :name");
        query.bindValue(":name", name);
        query.exec();

        if (query.next()) {
            QString name_sql = query.value(0).toString();
            QString pwd_sql = query.value(1).toString();

            if (name_sql == name && pwd_sql == pwd) {
                QMessageBox::information(nullptr, "提示", "登录成功");
                this->close();
                db.close();
                m = new Manage();
                m->show();
                return;
            }
        }

        QMessageBox::warning(nullptr, "警告", "用户名不存在或密码不正确");
    }
}
