#include "basicinformation.h"
#include "ui_basicinformation.h"

BasicInformation::BasicInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BasicInformation),
    mDriver(nullptr),
    mTruck(nullptr)
{
    ui->setupUi(this);

    initPage();

    driverButton = ui->driverButton;
    TruckButton = ui->truckButton;

    ui->searchBox->addItem("检索司机");
    ui->searchBox->addItem("检索车辆");

    // 连接信号和槽
    connect(ui->driverButton,&QPushButton::clicked, this,&BasicInformation::onNavigationButtonClicked);
    connect(ui->truckButton,&QPushButton::clicked, this,&BasicInformation::onNavigationButtonClicked);

}

BasicInformation::~BasicInformation()
{
    delete ui;
}

void BasicInformation::initPage()
{
    mDriver = new Driver;
    mTruck = new Truck;

    ui->stackedWidget->addWidget(mDriver);
    ui->stackedWidget->addWidget(mTruck);
    mDriver->showDriver();
    mTruck->showTruck();
    ui->stackedWidget->setCurrentWidget(mDriver);
}

void BasicInformation::onNavigationButtonClicked()
{
    QPushButton *clickedButton = qobject_cast<QPushButton*>(sender());
    if(clickedButton == driverButton)
    {
        mDriver->showDriver();
        ui->stackedWidget->setCurrentWidget(mDriver);
    }
    else if (clickedButton == TruckButton)
    {
        mTruck->showTruck();
        ui->stackedWidget->setCurrentWidget(mTruck);
    }
}

void BasicInformation::on_searchLineEdit_textEdited(const QString &arg1)
{
    QString searchText = arg1;
}



