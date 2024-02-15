#include "login_window.h"
#include "ui_login_window.h"
#include "main_interface.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    ,db(nullptr)
    ,usernameLine(nullptr)
    ,passwordLine(nullptr)
   ,user(nullptr)
{
    ui->setupUi(this);
    ui->loginButton->setDefault(true);
    user = new User;
    //初始化控件
    usernameLine = ui->usernameLine;
    passwordLine = ui->passwordLine;
    passwordLine->setEchoMode(QLineEdit::Password);
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
void MainWindow::on_usernameLine_editingFinished()
{
    user->userId = usernameLine->text().toInt();
    qDebug()<< user->userId;
}


void MainWindow::on_passwordLine_editingFinished()
{
    user->password = passwordLine->text().toInt();
    qDebug()<<user->password;
}

void MainWindow::on_loginButton_clicked()
{
    QSqlQuery query;
    //:userId占位符
    query.prepare("SELECT * FROM User WHERE userId = :userId ");
    query.bindValue(":userId", user->userId);
    query.exec();

    if (query.next())
    {
        // 登录成功，进入主界面
        int storedpassword = query.value("password").toInt();
        if (storedpassword == user->password)
        {
            user->userName = query.value("name").toString();
            user->authority = query.value("authority").toString();
            //实例化主界面
            main_interface *mainInterface = new main_interface();
            //向主界面传递* user
            mainInterface->setLoginUser(user);
            mainInterface->show();
            //不退出当前登陆页面保持数据库连接
            this->hide();
        }
        else
        {
            QMessageBox::warning(this,"登陆失败","密码错误");
        }
    }
    else
    {
        QMessageBox::warning(this,"登陆失败","用户名不存在");
    }

}


void MainWindow::on_exitButton_clicked()
{
    QApplication::quit();
}

void MainWindow::on_enrollButton_clicked()
{
    //进入模态对话框EnrollDialog进行注册
    enrollDialog = new Enroll();
    enrollDialog->exec();
}

