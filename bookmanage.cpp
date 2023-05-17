#include "bookmanage.h"
#include "ui_manage.h"
#include "database.h"

Manage::Manage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Manage)
{
    ui->setupUi(this);
    setWindowTitle("图书管理系统");

    db = Database::get_instance().database();
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Failed to connect to MySQL database.");
    }
    else{
        QString createTable = R"(
            CREATE TABLE IF NOT EXISTS book_table (
                id SERIAL PRIMARY KEY,
                title VARCHAR(255),
                author VARCHAR(255),
                date VARCHAR(255),
                nums INT(255) )
        )";
        QSqlQuery query(db);
        if (!query.exec(createTable)) {
            qDebug() << "Create table error: " << query.lastError().text();
        }
        else {
            qDebug() << "Table created.";
        }
    }
    // 创建图书表格，并将数据库图书展示到表格当中
    createBookTable();
    // 创建搜索框，并根据搜索信息将数据库图书展示到表格当中
    createSearchBox();

    //    // 实例化 bookDialog 对象
    //    bookDialog = new BookDialog(this);

    // 创建增加、修改、删除按钮
    addButton = new QPushButton("增加图书", this);
    editButton = new QPushButton("修改图书", this);
    deleteButton = new QPushButton("删除图书", this);

    // Create other UI elements and connect signals/slots
    // ...
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(bookTable);
    vlayout->addWidget(searchBox);

    QWidget *buttonContainer = new QWidget(this);  // 创建容器部件
    QHBoxLayout *hlayout = new QHBoxLayout(buttonContainer);
    hlayout->addWidget(addButton);
    hlayout->addWidget(editButton);
    hlayout->addWidget(deleteButton);

    vlayout->addWidget(buttonContainer);  // 将容器部件添加到vlayout
    setLayout(vlayout);

    // 添加增加、删除、修改图书界面
    connect(addButton, &QPushButton::clicked, this, &Manage::on_addButton_clicked);
    connect(editButton, &QPushButton::clicked, this, &Manage::on_editButton_clicked);
    connect(deleteButton, &QPushButton::clicked, this, &Manage::on_deleteButton_clicked);

}

Manage::~Manage()
{
    delete ui;
}


void Manage::createBookTable()
{
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

    // 字体
//    QFont font;
//    font.setFamily("Arial");
//    font.setPointSize(12);
//    bookTable->setFont(font);

    // 查询数据库表，并将数据更新到表格中
    QSqlQuery query(db);
    QString searchQuery = "SELECT * FROM book_table";
    query.prepare(searchQuery);
    if (!query.exec()) {
        qDebug() << "Search error: " << query.lastError().text();
        return;
    }
    int row = 0;

    while (query.next()) {

        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        QString date = query.value("date").toString();
        int nums = query.value("nums").toInt();

        QTableWidgetItem *titleItem = new QTableWidgetItem(title);
        QTableWidgetItem *authorItem = new QTableWidgetItem(author);
        QTableWidgetItem *dataItem = new QTableWidgetItem(date);
        QTableWidgetItem *numsItem = new QTableWidgetItem(QString::number(nums));

        bookTable->insertRow(row);
        bookTable->setItem(row, 0, titleItem);
        bookTable->setItem(row, 1, authorItem);
        bookTable->setItem(row, 2, dataItem);
        bookTable->setItem(row, 3, numsItem);

        row++;
    }
}

void Manage::createSearchBox()
{
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText("请输入关键词，搜索图书");
    connect(searchBox, &QLineEdit::textChanged, this, &Manage::searchBooks);
}

void Manage::searchBooks(const QString &keyword)
{
    QSqlQuery query(db);
    QString searchQuery = "SELECT * FROM book_table WHERE title LIKE :keyword OR author LIKE :keyword OR date LIKE :keyword";
    query.prepare(searchQuery);
    query.bindValue(":keyword", "%" + keyword + "%");
    if (!query.exec()) {
        qDebug() << "Search error: " << query.lastError().text();
        return;
    }
    bookTable->clearContents();
    bookTable->setRowCount(0);

    int row = 0;
    while (query.next()) {
        QString title = query.value("title").toString();
        QString author = query.value("author").toString();
        QString date = query.value("date").toString();
        int nums = query.value("nums").toInt();

        QTableWidgetItem *titleItem = new QTableWidgetItem(title);
        QTableWidgetItem *authorItem = new QTableWidgetItem(author);
        QTableWidgetItem *dateItem = new QTableWidgetItem(date);
        QTableWidgetItem *numsItem = new QTableWidgetItem(QString::number(nums));

        bookTable->insertRow(row);
        bookTable->setItem(row, 0, titleItem);
        bookTable->setItem(row, 1, authorItem);
        bookTable->setItem(row, 2, dateItem);
        bookTable->setItem(row, 3, numsItem);

        row++;
    }
}

bool Manage::addBook(const QString &title, const QString &author, const QString &date, const QString &num)
{
    QSqlQuery query;
    QString insertQuery = "INSERT INTO book_table (title, author, date, nums) VALUES (:title, :author, :date, :num)";
    query.prepare(insertQuery);
    query.bindValue(":title", title);
    query.bindValue(":author", author);
    query.bindValue(":date", date);
    query.bindValue(":num", num);

    if (!query.exec()) {
        qDebug() << "Add book error: " << query.lastError().text();
        return false;
    }

    refreshBookTable();
    return true;
}

bool Manage::editBook(const QString &title, const QString &newNum)
{
    if (newNum.isEmpty()) {
        qDebug() << "Edit book error: Invalid value for 'num'";
        return false;
    }

    QSqlQuery query;
    QString updateQuery = "UPDATE book_table SET nums = :num WHERE title = :title";
    query.prepare(updateQuery);
    query.bindValue(":title", title);
    query.bindValue(":num", QVariant(newNum).toInt());
    if (!query.exec()) {
        qDebug() << "Edit book error: " << query.lastError().text();
        return false;
    }
    refreshBookTable();
    return true;
}

bool Manage::deleteBook(const QString &title, const QString &author)
{

    QSqlQuery query;
    QString updateQuery = "delete from book_table where title = :title and author = :author";
    query.prepare(updateQuery);
    query.bindValue(":title", title);
    query.bindValue(":author", author);

    if (query.exec()) {
        if(query.numRowsAffected() > 0){
            refreshBookTable();
            return true;
        }else{
            return false;
        }
    }else{
        return false;
    }
}

void Manage::refreshBookTable(){
    searchBooks("");
}


void Manage::on_addButton_clicked()
{
    // 创建增加图书的对话框
    std::unique_ptr<BookDialog> addDialog(new BookDialog(this));

    // 显示对话框
    addDialog->setWindowTitle("增加图书");

    // 执行对话框的模态显示
    if (addDialog->exec() == QDialog::Accepted) {
        // 获取用户输入的图书信息
        QString title = addDialog->getTitle();
        QString author = addDialog->getAuthor();
        QString date = addDialog->getDate();
        QString num = addDialog->getNum();
        if(title.isEmpty() || num.isEmpty() || date.isEmpty() || author.isEmpty()){
            QMessageBox::warning(this, "增加图书", "请输入正确的信息");
            return;
        }
        // 执行添加图书的操作
        if(addBook(title, author, date, num)){
            QMessageBox::information(this, "添加图书", "添加成功");
        }else{
            QMessageBox::warning(this, "添加图书", "添加失败,请检查输入是否正确！");
        }
    }
}

void Manage::on_editButton_clicked()
{
    // 创建修改图书的对话框，并传递选中行的图书信息
    std::unique_ptr<BookDialog> editDialog(new BookDialog(this));

    // 显示对话框
    //    BookDialog *editDialog = new BookDialog(this);
    editDialog->setWindowTitle("修改图书");
    editDialog->authorLineEdit->setDisabled(true);
    editDialog->dateLineEdit->setDisabled(true);

    // 执行对话框的模态显示
    if (editDialog->exec() == QDialog::Accepted) {
        // 获取用户修改后的图书信息
        QString title = editDialog->getTitle();
        QString newNum = editDialog->getNum();
        if(title.isEmpty() || newNum.isEmpty()){
            QMessageBox::warning(this, "修改图书", "请输入正确的书名和数量");
            return;
        }
        // 执行修改图书的操作
        if(editBook(title, newNum)){
            QMessageBox::information(this, "修改图书", "修改成功");
        }else{
            QMessageBox::warning(this, "修改图书", "修改失败");
        }
    }
}

void Manage::on_deleteButton_clicked()
{

    // 创建删除图书的对话框
    std::unique_ptr<BookDialog> deleteDialog(new BookDialog(this));

    // 显示对话框
    //    BookDialog *deleteDialog = new BookDialog(this);
    deleteDialog->dateLineEdit->setDisabled(true);
    deleteDialog->numLineEdit->setDisabled(true);

    // 执行对话框的模态显示
    if (deleteDialog->exec() == QDialog::Accepted) {
        // 获取用户输入的图书信息
        QString title = deleteDialog->getTitle();
        QString author = deleteDialog->getAuthor();
        if(title.isEmpty() || author.isEmpty()){
            QMessageBox::warning(this, "删除图书", "请输入正确的书名和作者");
            return;
        }
        // 显示确认对话框
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "删除图书", "确定要删除选中的图书吗？", QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // 执行删除图书的操作
            if(deleteBook(title, author)){
                QMessageBox::information(this, "删除图书", "删除成功");
            }else{
                QMessageBox::warning(this, "删除图书", "删除失败");
            }
        }
        return;
    }
}

