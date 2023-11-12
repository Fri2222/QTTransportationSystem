#include "login_window.h"
#include "ui_login_window.h"
#include "main_interface.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->loginButton->setDefault(true);
    //数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setHostName("localhost"); // 主机名
    db.setPort(3306);            // 端口
    db.setDatabaseName("D:/22017/DataBase/Transpotation.db");  // 数据库名称
    db.setUserName("root");      // 用户名
    db.setPassword("220175");  // 密码
    if (!db.open()) {
        qDebug() << "Failed to connect to database!";
    }
    if(db.isOpen())
        qDebug() << "Connected to database!";
}


MainWindow::~MainWindow()
{
    delete ui;
    db->close();
}


void MainWindow::on_loginButton_clicked()
{
    main_interface *mainInterface = new main_interface();
    mainInterface->show();
    this->hide();
}


void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}

