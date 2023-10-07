#include "truck.h"
#include "ui_truck.h"


Truck::Truck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Truck)
{
    ui->setupUi(this);
    ui->tableView->setModel(&mmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mmodel.setHorizontalHeaderLabels({"货车ID","货车类型","货箱长度","货箱宽度","货箱最大载重量","百公里油耗率"});

}

Truck::~Truck()
{
    delete ui;
}
void Truck::on_addTruckButton_clicked()
{
    QStringList truckIterms;
    truckIterms <<"六轴货车"<<"五轴货车"<<"四轴货车"<<"三轴货车"<<"两轴货车";
    QString truckType =QInputDialog::getItem(this,
                                          "货车信息",
                                          "请选择货车",
                                          truckIterms,
                                          0,
                                          false) ;
    bool bLength = false;
    int truckLength = QInputDialog::getInt(this,
                                           "货箱长度",
                                           "请输入货箱长度",
                                           2, // 默认值
                                           0, // 最小值
                                           20, // 最大值
                                           1, // 步长
                                           &bLength
                                           );
    bool bBreadth = false;
    int truckBreadth = QInputDialog::getInt(this,
                                           "货箱长度",
                                           "请输入货箱长度",
                                           2, // 默认值
                                           0, // 最小值
                                           10, // 最大值
                                           1, // 步长
                                           &bBreadth
                                           );
    bool bMaxFuel = false;
    int truckMaxFuel = QInputDialog::getInt(this,
                                            "货箱长度",
                                            "请输入货箱长度",
                                            500, // 默认值
                                            0, // 最小值
                                            2000, // 最大值
                                            50, // 步长
                                            &bMaxFuel
                                            );
    bool bMaxWeight = false;
    int truckMaxWeight = QInputDialog::getInt(this,
                                            "货箱长度",
                                            "请输入货箱长度",
                                            30, // 默认值
                                            0, // 最小值
                                            200, // 最大值
                                            5, // 步长
                                            &bMaxWeight
                                            );

    QSqlQuery addQuery;
    addQuery.prepare("INSERT INTO Truck (truckType, truckLength, truckBreadth, truckMaxFuel, truckMaxWeight, "
                     "fuelConsumptionRatePer100KM, truckRequireLicense, containerChangable) "
                     "VALUES ( :truckType, :truckLength, :truckBreadth, :truckMaxFuel, :truckMaxWeight, "
                     ":fuelConsumptionRatePer100KM, :truckRequireLicense, :containerChangable)");

    int fuelConsumptionRatePer100KM = 0;
    QString truckRequireLicense = "";
    QString containerChangable = "";
    defaultType(truckType, fuelConsumptionRatePer100KM,
                truckRequireLicense, containerChangable);
    int truckID = addQuery.lastInsertId().toInt();
    QString truckTypeWithID = truckType + "_" + QString::number(truckID);
    //addQuery.bindValue(":truckID", truckID);
    addQuery.bindValue(":truckType", truckTypeWithID);
    addQuery.bindValue(":truckLength", truckLength);
    addQuery.bindValue(":truckBreadth", truckBreadth);
    addQuery.bindValue(":truckMaxFuel", truckMaxFuel);
    addQuery.bindValue(":truckMaxWeight", truckMaxWeight);
    addQuery.bindValue(":fuelConsumptionRatePer100KM", fuelConsumptionRatePer100KM);
    addQuery.bindValue(":truckRequireLicense", truckRequireLicense);
    addQuery.bindValue(":containerChangable", containerChangable);

    if (!addQuery.exec())
    {
        qDebug() << addQuery.lastError();
        QMessageBox::warning(this, "警告", "添加货车信息时出现错误，请联系管理员");
    }

}
void Truck::showTruck()
{
    QSqlQuery truckQuery;
    QString sql = "SELECT *FROM Truck";
    truckQuery.exec(sql);
    if(truckQuery.lastError().isValid())
    {
        qDebug() << "Database Error:" << truckQuery.lastError().text();
    }
    else
    {
        mmodel.removeRows(0,mmodel.rowCount());
        while(truckQuery.next())
        {
            int tableTruckID = truckQuery.value("truckID").toInt();
            QString tableTruckType = truckQuery.value("truckType").toString();
            int tableTruckLength = truckQuery.value("truckLength").toInt();
            int tableTruckBreadth = truckQuery.value("truckBreadth").toInt();
            int tableTruckMaxFuel = truckQuery.value("truckMaxFuel").toInt();
            int tableTruckFuelConsumption = truckQuery.value("fuelConsumptionRatePer100KM").toInt();
            QList<QStandardItem *> truckItem;
            truckItem.append(new QStandardItem(QString::number(tableTruckID)));
            truckItem.append(new QStandardItem(tableTruckType));
            truckItem.append(new QStandardItem(QString::number(tableTruckLength)));
            truckItem.append(new QStandardItem(QString::number(tableTruckBreadth)));
            truckItem.append(new QStandardItem(QString::number(tableTruckMaxFuel)));
            truckItem.append(new QStandardItem(QString::number(tableTruckFuelConsumption)));
            mmodel.appendRow(truckItem);
        }
    }
}
void Truck::defaultType(QString &truckType, int &fuelConsumptionRatePer100KM,
                        QString &truckRequireLicense, QString &containerChangable)
{
    //设定不同货车默认参数
    std::map<QString, defaultTruck> defaultValues;
    defaultValues["六轴货车"] = {"sixAlxleTruck", 31, "A2", "yes"};
    defaultValues["五轴货车"] = {"fiveAlxleTruck", 27, "A2", "no"};
    defaultValues["四轴货车"] = {"fourAlxleTruck", 22, "A2", "no"};
    defaultValues["三轴货车"] = {"threeAlxleTruck", 20, "A3", "no"};
    defaultValues["两轴货车"] = {"twoAlxleTruck", 8, "A4", "no"};

    // 查找与所选货车类型关联的默认值
    std::map<QString, defaultTruck>::iterator it = defaultValues.find(truckType);
    if (it != defaultValues.end())
    {
    defaultTruck defaultValuesForTruck = it->second;
    truckType = defaultValuesForTruck.truckType;
    fuelConsumptionRatePer100KM = defaultValuesForTruck.fuelConsumptionRatePer100KM;
    truckRequireLicense = defaultValuesForTruck.truckRequireLicense;
    containerChangable = defaultValuesForTruck.containerChangable;
    }
}
