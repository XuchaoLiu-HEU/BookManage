#include "database.h"

Database::Database() {
    // 在构造函数中进行数据库连接的初始化
    // ...
    // 创建数据库
    m_db = QSqlDatabase::addDatabase("QODBC");
    m_db.setHostName("127.0.0.1");
    m_db.setPort(3306);
    m_db.setDatabaseName("users");
    m_db.setUserName("root");
    m_db.setPassword("1234");
    if (!m_db.open()) {
        qDebug() << "单例模式数据库连接失败：" << m_db.lastError().text();
    } else {
        qDebug() << "数据库连接成功";
    }

}

Database::~Database() {
    // 在析构函数中关闭数据库连接等资源的释放
    // ...
    m_db.close();
}

Database& Database::get_instance() {
    static Database manager;
    return manager;
}

bool Database::Open() {
    if (!m_db.open()) {
        qDebug() << "数据库连接失败：" << m_db.lastError().text();
        return false;
    } else {
        qDebug() << "数据库连接成功";
        return true;
    }
}

QSqlDatabase Database::database() const {
    return m_db;
}
