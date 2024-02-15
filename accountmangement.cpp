#include "accountmangement.h"
#include "ui_accountmangement.h"

AccountMangement::AccountMangement(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountMangement),
    mNewOrder(nullptr),
    mOrderAndAccount(nullptr)
{
    ui->setupUi(this);

    newOrderButton = ui->newOrderButton;
    orderHistoryButton = ui->orderHistoryButton;
    basicInformationButton = ui->basicInformationButton;
    changePasswordButton = ui->changePasswordButton;
}

AccountMangement::~AccountMangement()
{
    delete ui;
}

void AccountMangement::initPage()
{
    // 初始化界面
    mNewOrder = new NewOrder(this);
    mOrderAndAccount = new OrderAndAccount(this);

    // 添加界面
    ui->stackedWidget->addWidget(mNewOrder);
    ui->stackedWidget->addWidget(mOrderAndAccount);

    // 设定初始界面
    ui->stackedWidget->setCurrentWidget(mNewOrder);
}

void AccountMangement::on_newOrderButton_clicked()
{

}
void AccountMangement::setUser(User user)
{
    currentUser = user;
}
