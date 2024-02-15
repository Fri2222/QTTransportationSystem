#include "enroll.h"
#include "ui_enroll.h"

Enroll::Enroll(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Enroll),
    nameLineEdit(nullptr),
    passwordLineEdit(nullptr),
    ensureLineEdit(nullptr)
{
    ui->setupUi(this);
    setFixedSize(330, 330);

    //初始化组件
    nameLineEdit = ui->NameLineEdit;
    passwordLineEdit = ui->PasswordLineEdit;
    ensureLineEdit = ui->EnsureLineEdit;

    passwordLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    ensureLineEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);

    registerUser.authority = "用户";
    //初始化用户密码和姓名
    registerUser.password = 0;
    registerUser.userName = "";
}

Enroll::~Enroll()
{
    delete ui;
}
void Enroll::on_NameLineEdit_editingFinished()
{
    registerUser.userName= nameLineEdit->text();
}


void Enroll::on_PasswordLineEdit_editingFinished()
{
    comformationPassword = passwordLineEdit->text().toInt();
}


void Enroll::on_EnsureLineEdit_editingFinished()
{
    if(comformationPassword == ensureLineEdit->text().toInt())
    {
        registerUser.password = ensureLineEdit->text().toInt();
    }
    else
    {
        //两次输入密码不相同弹出warning并且清空输入
        QMessageBox::warning(this,"密码不匹配","两次输入密码不匹配，请重新输入密码");
        passwordLineEdit->clear();
        ensureLineEdit->clear();
    }
}


void Enroll::on_CommitButton_clicked()
{
    //获取班级和部门信息
    QSqlQuery query;
    query.prepare("INSERT INTO User (userId, name, password, authority) "
                  "VALUES (:userId, :userName, :password, :authority)");
    query.bindValue(":userName", registerUser.userName);
    query.bindValue(":password", registerUser.password);
    query.bindValue(":authority", registerUser.authority);

    if(registerUser.password == 0)
    {
        QMessageBox::warning(this,"警告","密码不能为空或全为0");
    }
    //检验字符串是否为空不能使用==NULL
    else if (registerUser.userName.isEmpty())
    {
         QMessageBox::warning(this,"警告","姓名不能为空");
    }
    else
    {
        if (query.exec())
        {
             // 获取自增 ID
            registerUser.userId = query.lastInsertId().toInt();
            QMessageBox::information(this,"用户ID","您已成功注册\n你的用户ID为:"+QString::number(registerUser.userId));
            accept();
        }
        else
        {
            qDebug() << "Failed to register new user: " << query.lastError().text();
        }
    }
}

void Enroll::on_CancelButton_clicked()
{
    reject();
}
