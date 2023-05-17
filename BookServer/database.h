#ifndef DATABASE_H
#define DATABASE_H
#include <QtSql>

class Database
{

public:
    static Database& get_instance();  // 获取单例实例
    bool Open();  // 是否连接
    QSqlDatabase database() const;  // 获取数据库连接对象

private:
    Database();  // 构造函数私有化，防止直接创建对象
    ~Database(); // 禁止拷贝构造和赋值操作

    QSqlDatabase m_db;  // 数据库连接对象
};

#endif // DATABASE_H
