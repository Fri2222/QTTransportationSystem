#include "cargoinformation.h"
#include "ui_cargoinformation.h"

CargoInformation::CargoInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CargoInformation)
{
    ui->setupUi(this);
    ui->tableView->setModel(&mmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mmodel.setHorizontalHeaderLabels({"钢材ID","钢材类型","钢材长度","钢材数量","钢材重量"});
    showSteel();
}

CargoInformation::~CargoInformation()
{
    delete ui;
}

void CargoInformation::on_wholeButton_clicked()
{
    showSteel();
}
void CargoInformation::showSteel()
{
    QSqlQuery steelQuery;
    QString sql = "SELECT *FROM steel";
    steelQuery.exec(sql);
    if(steelQuery.lastError().isValid())
    {
        qDebug() << "Database Error:" << steelQuery.lastError().text();
    }
    else
    {
        mmodel.removeRows(0,mmodel.rowCount());
        while(steelQuery.next())
        {
            int tablesteelID = steelQuery.value("steelID").toInt();
            QString tablesteelType = steelQuery.value("steelType").toString();
            double tablesteelLength = steelQuery.value("steelLength").toDouble();
            double tablesteelQuantity = steelQuery.value("steelQuantity").toDouble();
            double tablesteelWeight = steelQuery.value("steelWeight").toDouble();            QList<QStandardItem *> steelItem;
            steelItem.append(new QStandardItem(QString::number(tablesteelID)));
            steelItem.append(new QStandardItem(tablesteelType));
            steelItem.append(new QStandardItem(QString::number(tablesteelLength)));
            steelItem.append(new QStandardItem(QString::number(tablesteelQuantity)));
            steelItem.append(new QStandardItem(QString::number(tablesteelWeight)));
            mmodel.appendRow(steelItem);
        }
    }
}


void CargoInformation::on_addButton_clicked()
{
    QStringList steelIterms;
    steelIterms <<"管材"<<"型材"<<"板材"<<"棒材";
    QString steelType =QInputDialog::getItem(this,
                                              "钢材信息",
                                              "请选择钢材种类",
                                              steelIterms,
                                              0,
                                              false) ;
    bool bLength = false;
    double steelLength = QInputDialog::getInt(this,
                                           "钢材长度",
                                           "请输入钢材长度，单位为米",
                                           8, // 默认值
                                           0, // 最小值
                                           20, // 最大值
                                           1, // 步长
                                           &bLength
                                           );
    bool bQuantity = false;
    int steelQuantity = QInputDialog::getInt(this,
                                            "钢材数量",
                                            "请输入钢材数量，单位为根",
                                            200, // 默认值
                                            0, // 最小值
                                            10000, // 最大值
                                            100, // 步长
                                            &bQuantity
                                            );
    bool bWeight = false;
    double steelWeight = QInputDialog::getInt(this,
                                            "钢材质量",
                                            "请输入钢材质量",
                                            10, // 默认值
                                            0, // 最小值
                                            2000, // 最大值
                                            50, // 步长
                                            &bWeight
                                            );
    QSqlQuery addQuery;
    addQuery.prepare("INSERT INTO Steel (steelType, steelLength, steelQuantity, steelWeight) "
                     "VALUES (:steelType, :steelLength, :steelQuantity, :steelWeight)");
    addQuery.bindValue(":steelType", steelType);
    addQuery.bindValue(":steelLength", steelLength);
    addQuery.bindValue(":steelQuantity", steelQuantity);
    addQuery.bindValue(":steelWeight", steelWeight);
    int steelID = -1 ;
    if (addQuery.exec())
    {
        steelID = addQuery.lastInsertId().toInt();
    }
    else
    {
        qDebug() << addQuery.lastError();
        QMessageBox::warning(this, "警告", "添加货物信息时出现错误，请联系管理员");
    }

    double steelPriority = 0;
    std::map<QString, defaultSteel> defaultValues;
    defaultValues["管材"] = {"pipe",3};
    defaultValues["型材"] = {"section", 2};
    defaultValues["板材"] = {"plate", 4};
    defaultValues["棒材"] = {"bar", 1};
                                                                                                               std::map<QString, defaultSteel>::iterator it = defaultValues.find(steelType);
    if (it != defaultValues.end())
    {
        defaultSteel defaultValuesForSteel = it->second;
        steelType = defaultValuesForSteel.steelType;
        steelPriority = defaultValuesForSteel.steelPriority;
    }
    //钢材长度每增加5米，质量每增加3吨priority增加0.1
    steelPriority = steelPriority + 0.1*(steelLength/5) +0.1*(steelWeight/3);
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE Steel SET steelType = :steelType, steelPriority = :steelPriority WHERE steelID = :steelID");
    updateQuery.bindValue(":steelType", steelType);
    updateQuery.bindValue(":steelPriority", steelPriority);
    updateQuery.bindValue(":steelID", steelID);
    if(!updateQuery.exec())
    {
        qDebug()<<updateQuery.lastError();
    }
    showSteel();
}
