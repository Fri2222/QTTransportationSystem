#include "main_interface.h"
#include "ui_main_interface.h"
#include "accountmangement.h"

main_interface::main_interface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::main_interface),
    mAccountMangement(nullptr),
    mBasicInformation(nullptr),
    mCargoInformation(nullptr),
    mYardInformation(nullptr),
    mLastPressedButton(nullptr)
{
    ui->setupUi(this);
    initPage();

    accountmanageButton = ui->accountmanageButton;
    basicInformationButton = ui->basicInformationButton;
    cargoInformationButton = ui->cargoInformationButton;
    yardInformationButton = ui->yardInformationButton;

    connect(ui->accountmanageButton, &QPushButton::clicked, this, &main_interface::onNavigationButtonClicked);
    connect(ui->basicInformationButton, &QPushButton::clicked, this, &main_interface::onNavigationButtonClicked);
    connect(ui->cargoInformationButton, &QPushButton::clicked, this, &main_interface::onNavigationButtonClicked);
    connect(ui->yardInformationButton, &QPushButton::clicked, this, &main_interface::onNavigationButtonClicked);

}

main_interface::~main_interface()
{
    delete ui;
}

void main_interface::initPage()
{
    // 初始化界面
    mAccountMangement = new AccountMangement(this);
    mBasicInformation = new BasicInformation(this);
    mCargoInformation = new CargoInformation(this);
    mYardInformation = new YardInformation(this);

    mAccountMangement->initPage();
    // 添加界面
    ui->stackedWidget->addWidget(mAccountMangement);
    ui->stackedWidget->addWidget(mBasicInformation);
    ui->stackedWidget->addWidget(mCargoInformation);
    ui->stackedWidget->addWidget(mYardInformation);

    // 设定初始界面
    ui->stackedWidget->setCurrentWidget(mBasicInformation);
}

void main_interface::onNavigationButtonClicked()
{
    if (mLastPressedButton != nullptr) {
        mLastPressedButton->setStyleSheet(""); // 清空样式表，恢复颜色
    }
    //监测按下的按钮
    QToolButton *clickedButton = qobject_cast<QToolButton*>(sender());

    // 设置当前按钮为黑色背景
    clickedButton->setStyleSheet("background-color: #303133; color: white;");

    // 更新当前被按下的按钮
    mLastPressedButton = clickedButton;

    //页面切换
    if (clickedButton == ui->accountmanageButton)
    {
        ui->stackedWidget->setCurrentWidget(mAccountMangement);
    }
    else if (clickedButton == ui->basicInformationButton)
    {
        ui->stackedWidget->setCurrentWidget(mBasicInformation);
    }
    else if (clickedButton == ui->cargoInformationButton)
    {
        ui->stackedWidget->setCurrentWidget(mCargoInformation);
    }
    else if (clickedButton == ui->yardInformationButton)
    {
        ui->stackedWidget->setCurrentWidget(mYardInformation);
    }
}



