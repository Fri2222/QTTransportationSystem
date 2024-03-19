#include "neworder.h"
#include "ui_neworder.h"
#include "orderconfirmation.h"
NewOrder::NewOrder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NewOrder),
    pictureLabel(nullptr)
{
    ui->setupUi(this);
    driverActions.clear();
    loadActions.clear();
    pictureLabel = ui->pictureLable;
    pictureLabel->hide();
    orderID = -1;
    setFixedSize(660,330);
    ui->planButton->hide();
    ui->recreateButton->hide();

    bDriverWidget = false;
    bSteelWidget = true;
    cost = 0;

    ui->driverTableView->setModel(&driverModel);
    ui->driverTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->driverTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->driverTableView->setSelectionMode(QAbstractItemView::MultiSelection);
    driverModel.setHorizontalHeaderLabels({"司机ID","司机姓名"});

    ui->truckTableView->setModel(&truckModel);
    ui->truckTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->truckTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->truckTableView->setSelectionMode(QAbstractItemView::MultiSelection);
    truckModel.setHorizontalHeaderLabels({"货车ID","货车类型","货箱长度","货箱宽度","货箱最大载重量","百公里油耗率"});

    ui->steelTableView->setModel(&steelModel);
    ui->steelTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->steelTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->steelTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    steelModel.setHorizontalHeaderLabels({"选择","钢材ID","钢材名称","钢材长度","钢材数量","钢材质量","钢材初始位置","钢材目标位置"});
    //ui->steelTableView->setEditTriggers(QAbstractItemView::DoubleClicked);

    queryDistance();
    showDriver();
    showTruck();
    showSteel();
    int lastRow = steelModel.rowCount();
    for(int row = 0;row < lastRow ;row ++)
    {
        for(int conlumn = 6;conlumn < 7 ;conlumn++)
        {
            QComboBox* steelInitialPositionCmb = new QComboBox();
            steelInitialPositionCmb->addItems({yardNameItems});
            steelInitialPositionCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->steelTableView->setIndexWidget(steelModel.index (row,conlumn), steelInitialPositionCmb);
            // 将 QComboBox 指针添加到 QVector 中
            initialComboBoxArray.append(steelInitialPositionCmb);
        }
        for(int conlumn = 7;conlumn < 8 ;conlumn++)
        {
            QComboBox* steelGoalPositionCmb = new QComboBox();
            steelGoalPositionCmb->addItems({yardNameItems});
            steelGoalPositionCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->steelTableView->setIndexWidget(steelModel.index (row,conlumn), steelGoalPositionCmb);
            // 将 QComboBox 指针添加到 QVector 中
            goalComboBoxArray.append(steelGoalPositionCmb);
        }
    }
    shiftTableView(bDriverWidget,bSteelWidget);

    connect(ui->steelTableView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(onSteelTableViewDoubleClicked(QModelIndex)));


}

NewOrder::~NewOrder()
{
    delete ui;
}
void NewOrder::on_commitButton_clicked()
{
    QList<int> steelIDItems;
    QStringList selectedSteelType;
    QList<double> selectedSteelLengths;
    QList<double> selectedSteelQuantitys;
    QList<double> selectedSteelWeights;
    QList<int> selectedSteelPrioritys;
    QStringList selectedSteelInitialPositions;
    QStringList selectedSteelGoalPositions;
    bool bSelectedSteel = false;
    bSelectedSteel = scanSelectedSteel(steelIDItems,
                                  selectedSteelType,
                                  selectedSteelLengths,
                                  selectedSteelQuantitys,
                                  selectedSteelWeights,
                                  selectedSteelPrioritys,
                                  selectedSteelInitialPositions,
                                  selectedSteelGoalPositions);
    if(bSelectedSteel)
    {
        orderConfirmation* orderWindow = new orderConfirmation();
        orderWindow->sendID(steelIDItems);
        orderWindow->sendType(selectedSteelType);
        orderWindow->sendWeight(selectedSteelWeights);
        orderWindow->sendInitialPosition(selectedSteelInitialPositions);
        orderWindow->sendGoalPosition(selectedSteelGoalPositions);
        orderWindow->sendYardItem(yardNameItems);
        orderWindow->showOrder();
        orderWindow->exec();
    }
    if(bSelectedSteel)
    {
        QFile file("D://MetricFF//order_"+QString::number(steelIDItems[0])+"_pro.pddl");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            // 清空文件内容
            file.resize(0);
            QTextStream stream(&file);

            QString define = "";
//domain定义
            define.append("(define (problem order_"+QString::number(steelIDItems[0])+"_p ) (:domain transportartionSystem)\n");
    //object定义
        //driver定义
            define.append("(:objects\n" + driverNameItems.join(" ") +" - driver\n");
        //truck和container定义
            define.append(truckTypeItems.join(" ") + " - truck\n");
            for (const QString& truckType : truckTypeItems)
            {
                define.append("container_" + truckType + " ");
            }
            define.append("- container\n");
        //steel定义
            for (int i = 0;i <selectedSteelType.size();i++)
            {
                define.append(selectedSteelType[i] + "_"+QString::number(steelIDItems[i])+ " ");
            }
            define.append("- steel\n");
        //location定义
            define.append(yardAliasItems.join(" ") + " - location\n)");
            define.append("\n");
            stream << define ;
    //init定义
            QString init = "";
        //time-cost money-cost函数定义
            init.append("\n(:init\n(world)\n(= (time-cost) 0)\n(= (money-cost) 0)\n");
        //driver的姓名、驾照、位置（未写）定义
            for (int i = 0; i < driverNameItems.size(); ++i)
            {
                const QString& driverName = driverNameItems[i];
                init.append("(= (license " + driverName + ") " + driverLicenseItems[i] + ")\n");
                init.append("( driver-in " + driverName + " LOCATION1 )\n");
            }
            init.append("\n");
        //truck基本信息定义
            for (int i = 0; i < truckTypeItems.size(); ++i)
            {
                const QString& truckType = truckTypeItems[i];
                //const QString& truckPosition = truckPositionItems[i];
                const QString& truckSpeed = truckSpeedItems[i];

                init.append("( truck-in " + truckType + " LOCATION1 )\n");

                //truck信息
                init.append("(= (speed " + truckType +") " + truckSpeed + " )\n");
                init.append("(= (fuel-truck " + truckType+ " )"+truckMaxFuelItems[i]+")\n");
                init.append("(= (max-fuel-truck " + truckType+ " )"+truckMaxFuelItems[i]+")\n");
                init.append("(= (fuel-consumption-coefficient " + truckType+ " )"+fuelConsumptionRatePer100KMItems[i]+")\n");
                init.append("(= (license-require " + truckType+ " )"+truckRequireLicenseItems[i]+")\n");
                if(containerChangableItems[i] == "yes")
                {
                    init.append("( can-exchange " + truckType+" )\n");
                }
                //container信息
                init.append("(with container_"+truckType+" "+truckType+")\n");
                init.append("(empty container_"+truckType+")\n");
                init.append("(= (length-container container_"+truckType+") "+truckLengthItems[i]+")\n");
                init.append("(= (breadth-container container_"+truckType+") "+truckBreadthItems[i]+")\n");
                init.append("(= (current-weight container_"+truckType+") 0)\n");
                init.append("(= (max-weight container_"+truckType+")"+truckMaxWeightItems[i]+")\n\n");
            }
            init.append("\n");
        //道路的定义
            for(int i = 0;i < disYardIDItems.size();i++)
            {
                init.append("( connected-by-way "+disYardAliasItems[i]+" "+disOtherYardAliasItems[i]+" )\n");
                init.append("(= (distance "+disYardAliasItems[i]+" "+disOtherYardAliasItems[i]+" )"+QString::number(distanceItems[i])+")\n");
            }
            init.append("\n");
        //选中的钢材信息
            for(int i = 0;i < selectedSteelType.size();i++)
            {
                init.append("( "+selectedSteelType[i]+" "+selectedSteelType[i] +"_"+ QString::number(steelIDItems[i])+" )\n");
                init.append("(= (priority "+selectedSteelType[i] +"_"+ QString::number(steelIDItems[i])+") " +QString::number(selectedSteelPrioritys[i])+" )\n");
                init.append("(= (length-steel "+ selectedSteelType[i] +"_"+ QString::number(steelIDItems[i])+") " +QString::number(selectedSteelLengths[i])+" )\n");
                init.append("(= (quantity "+ selectedSteelType[i] +"_"+ QString::number(steelIDItems[i])+") " +QString::number(selectedSteelQuantitys[i])+" )\n");
                init.append("(= (weight-steel "+ selectedSteelType[i] +"_"+ QString::number(steelIDItems[i])+") " +QString::number(selectedSteelWeights[i])+" )\n");
                init.append("(clear "+selectedSteelType[i] +"_"+ QString::number(steelIDItems[i])+")\n");
                QString steelType = selectedSteelType[i];
                QString steelInitialPosition;
                QString steelGoalPosition ;
                steelInitialPosition = queryAlias(selectedSteelInitialPositions[i]);
                steelGoalPosition = queryAlias(selectedSteelGoalPositions[i]);
                init.append("(q1 " +steelType + "_"+QString::number(steelIDItems[i]) + ")\n");
                init.append("( steel-in "+ steelType + "_"+QString::number(steelIDItems[i]) + " " + steelInitialPosition + ")\n");
                init.append("( set-goal-location "+ steelType + "_"+QString::number(steelIDItems[i]) + " " + steelGoalPosition + ")\n");
            }
            init.append(")\n");
            stream << init;
    //goal
            QString goal = "";
            goal.append("(:goal (and\n");
            goal.append("(world)\n");
            for(int i = 0;i < selectedSteelType.size();i++)
            {
                const QString& steelType = selectedSteelType[i];
                goal.append("( order-completed "+ steelType + "_"+QString::number(steelIDItems[i]) + ")\n");
            }
            goal.append(")\n");
            goal.append(")\n");
            stream << goal;
    //metric
            QString metric = "";
            metric.append("(:metric minimize (+ (time-cost) (money-cost)))");
            metric.append("\n");
            stream << metric;
            stream << "\n)";
            file.close();
        }
    }
    else
    {
        QMessageBox::warning(this,"警告","请检查订单中的钢材");
    }
    acquireActionList();
    cmdPDDL(steelIDItems[0]);
    calculateCost();
    cmdPDDL(steelIDItems[0],1);
    calculateCost();
    cmdPDDL(steelIDItems[0],2);
    int count = 3;
    QString filePath = "D:/QtProject/TranspotationSystem/outputFile/output"+QString::number(count)+".txt";
    while (true)
    {
        if (outputCompare("D:/QtProject/TranspotationSystem/outputFile/output" + QString::number(count - 2) + ".txt",
                          "D:/QtProject/TranspotationSystem/outputFile/output" + QString::number(count - 1) + ".txt") == true || count == 10)
        {
            break; // 当两个文件内容相等或count达到10时，退出循环。
        }
        calculateCost();
        cmdPDDL(steelIDItems[0],count);
        count++;
    }
    acquireActionList(count-1);
    generatePicture();
    writeMapMain();
    bDriverWidget = false;
    bSteelWidget = true;
    shiftTableView(bDriverWidget,bSteelWidget);
}
void NewOrder::shiftTableView(bool bDriverWidget,bool bSteelWidget)
{
    ui->driverLabel->setVisible(bDriverWidget);
    ui->truckLabel->setVisible(bDriverWidget);
    ui->steelLabel->setVisible(bSteelWidget);
    ui->driverTableView->setVisible(bDriverWidget);
    ui->truckTableView->setVisible(bDriverWidget);
    ui->steelTableView->setVisible(bSteelWidget);
}
void NewOrder::showDriver()
{
    QSqlQuery driverQuery;
    QString sql = "SELECT * FROM Driver";
    if (driverQuery.exec(sql))
    {
        driverModel.removeRows(0, driverModel.rowCount());

        while (driverQuery.next())
        {
            int tableDriverID = driverQuery.value("driverID").toInt();
            QString tableDriverName = driverQuery.value("driverName").toString();
            QList<QStandardItem *> driverItem;
            driverItem.append(new QStandardItem(QString::number(tableDriverID)));
            driverItem.append(new QStandardItem(tableDriverName));
            driverModel.appendRow(driverItem);

            QString driverName = driverQuery.value("driverName").toString();
            QString driverLicense = driverQuery.value("driverLicense").toString();
            int intDriverLicense = shiftLicense(driverLicense);
            driverLicense = QString::number(intDriverLicense);
            driverNameItems << driverName;
            driverLicenseItems << driverLicense;
        }
    }
    else
    {
        qDebug() << "Driver query execution failed: " << driverQuery.lastError().text();
    }
}

void NewOrder::showTruck()
{
    QSqlQuery truckQuery;
    QString sql = "SELECT *FROM Truck";
    truckQuery.exec(sql);
    truckModel.removeRows(0,truckModel.rowCount());
    while(truckQuery.next())
    {
        int tableTruckID = truckQuery.value("truckID").toInt();
        QString tableTruckType = truckQuery.value("truckType").toString();
        int tableTruckMaxWeight = truckQuery.value("truckMaxWeight").toInt();
        QList<QStandardItem *> truckItem;
        truckItem.append(new QStandardItem(QString::number(tableTruckID)));
        truckItem.append(new QStandardItem(tableTruckType));
        truckItem.append(new QStandardItem(QString::number(tableTruckMaxWeight)));
        truckModel.appendRow(truckItem);
        QString truckType = truckQuery.value("truckType").toString();
        truckTypeItems << truckType;
        QString truckSpeed = truckQuery.value("truckSpeed").toString();
        truckSpeedItems << truckSpeed;
        QString truckLength = truckQuery.value("truckLength").toString();
        truckLengthItems <<truckLength ;
        QString truckBreadth = truckQuery.value("truckBreadth").toString();
        truckBreadthItems<<truckBreadth;
        QString truckMaxFuel = truckQuery.value("truckMaxFuel").toString();
        truckMaxFuelItems<<truckMaxFuel;
        QString truckMaxWeight = truckQuery.value("truckMaxWeight").toString();
        truckMaxWeightItems<<truckMaxWeight;
        QString fuelConsumptionRatePer100KM = truckQuery.value("fuelConsumptionRatePer100KM").toString();
        fuelConsumptionRatePer100KMItems<< fuelConsumptionRatePer100KM;
        QString truckRequireLicense = truckQuery.value("truckRequireLicense").toString();
        int intTruckRequireLicense = shiftLicense(truckRequireLicense);
        truckRequireLicense = QString::number(intTruckRequireLicense);
        truckRequireLicenseItems<< truckRequireLicense;
        QString containerChangable =truckQuery.value("containerChangable").toString();
        containerChangableItems << containerChangable;
    }
}
void NewOrder::showSteel()
{
    QSqlQuery steelQuery;
    QString sql = "SELECT * FROM steel";
    steelQuery.exec(sql);
    if (!steelQuery.lastError().isValid())
    {        steelModel.removeRows(0, steelModel.rowCount());
        while (steelQuery.next())
        {
            int tablesteelID = steelQuery.value("steelID").toInt();
            QString tablesteelType = steelQuery.value("steelType").toString();
            double tablesteelLength = steelQuery.value("steelLength").toDouble();
            double tablesteelQuantity = steelQuery.value("steelQuantity").toDouble();
            double tablesteelWeight = steelQuery.value("steelWeight").toDouble();

            // 创建一个新的 checkable item 并设置为未选中状态
            QStandardItem *checkItem = new QStandardItem;
            checkItem->setCheckable(true);
            checkItem->setCheckState(Qt::Unchecked);
            //为每行第一列都设置复选框
            QList<QStandardItem *> steelItem;
            steelItem.append(checkItem);
            steelItem.append(new QStandardItem(QString::number(tablesteelID)));
            steelItem.append(new QStandardItem(tablesteelType));
            steelItem.append(new QStandardItem(QString::number(tablesteelLength)));
            steelItem.append(new QStandardItem(QString::number(tablesteelQuantity)));
            steelItem.append(new QStandardItem(QString::number(tablesteelWeight)));
            // 在最后一列添加自定义的单元格编辑器
            steelModel.appendRow(steelItem);
            // 保存复选框状态到映射中
            steelCheckboxStates[tablesteelID] = checkItem->checkState();
        }

    }
    else
    {
        qDebug() << "Database Error:" << steelQuery.lastError().text();
    }
}
void NewOrder::onSteelTableViewDoubleClicked(QModelIndex index)
{
    QItemSelectionModel *selectionModel = ui->steelTableView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedRows();

    if (index.isValid() && !selectedIndexes.isEmpty())
    {
        // 遍历选中的行
        foreach (QModelIndex selectedIndex, selectedIndexes)
        {
            int row = selectedIndex.row();
            int steelID = steelModel.index(row, 0).data().toInt();
            int column = index.column();
            // 限制只有选中的行且列索引在 3、4、5 时才能编辑
            if (column >= 3 && column <= 7)
            {
                // 获取当前行的复选框状态
                QModelIndex checkBoxIndex = steelModel.index(row, 0);
                Qt::CheckState checkBoxState =
                    steelModel.data(checkBoxIndex, Qt::CheckStateRole).value<Qt::CheckState>();

                // 检查复选框是否被选中
                if (checkBoxState == Qt::Checked)
                {
                    // 保存复选框状态到映射中
                    steelCheckboxStates[steelID] = checkBoxState;
                    // 进入编辑模式
                    ui->steelTableView->edit(index);

                }
            }
        }
    }
}
void NewOrder::on_selectDTButton_clicked()
{
    bDriverWidget = true;
    bSteelWidget = false;
    shiftTableView(bDriverWidget,bSteelWidget);
}
void NewOrder::on_selectSButton_clicked()
{
    bDriverWidget = false;
    bSteelWidget = true;
    shiftTableView(bDriverWidget,bSteelWidget);
}


void NewOrder::selectedDriver(int &driverID)
{
    QItemSelectionModel *selectionModel = ui->driverTableView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedRows();
    foreach (const QModelIndex &index, selectedIndexes)
    {
        //获取当前索引所在的行号
        int row = index.row();
        driverID = driverModel.index(row, 0).data().toInt();
    }
}
void NewOrder::selectedTruck(int &truckID)
{
    QItemSelectionModel *selectionModel = ui->truckTableView->selectionModel();
    QModelIndexList selectedIndexes = selectionModel->selectedRows();
    foreach (const QModelIndex &index, selectedIndexes)
    {
        //获取当前索引所在的行号
        int row = index.row();
        truckID = truckModel.index(row, 0).data().toInt();
    }
}
bool NewOrder::scanSelectedSteel(QList<int> &selectedSteelIDs,
                             QStringList &selectedSteelType,
                             QList<double> &selectedSteelLengths,
                             QList<double> &selectedSteelQuantitys,
                             QList<double> &selectedSteelWeights,
                             QList<int> &selectedSteelPrioritys,
                             QStringList &selectedSteelInitialPositions,
                             QStringList &selectedSteelGoalPositions)
{
    bool selectedIsValid = false;
    int steelRowCount = steelModel.rowCount();
    selectedSteelIDs.clear();
    selectedSteelType.clear();
    selectedSteelLengths.clear();
    selectedSteelQuantitys.clear();
    selectedSteelWeights.clear();
    selectedSteelPrioritys.clear();
    for (int row = 0; row < steelRowCount ; ++row)
    {
        // 获取当前行的复选框状态
        QModelIndex checkBoxIndex = steelModel.index(row, 0);
        Qt::CheckState checkBoxState =
            steelModel.data(checkBoxIndex, Qt::CheckStateRole).value<Qt::CheckState>();

        if (checkBoxState == Qt::Checked)
        {
            int steelID = steelModel.index(row, 1).data().toInt();
            QString steelType = steelModel.index(row, 2).data().toString();
            double steelLength = steelModel.index(row, 3).data().toDouble();
            double steelQuantity = steelModel.index(row, 4).data().toDouble();
            double steelWeight = steelModel.index(row, 5).data().toDouble();
            QString steelInitialPosition = initialComboBoxArray[row]->currentText();
            QString steelGoalPositon = goalComboBoxArray[row]->currentText();
            bool bLength = dataIsVaild(3,steelLength);
            bool bQuantity = dataIsVaild(4,steelQuantity);
            bool bWeight = dataIsVaild(5,steelWeight);
            if(bLength && bQuantity && bWeight)
            {
                selectedSteelIDs << steelID;
                selectedSteelType << steelType;
                selectedSteelLengths << steelLength;
                selectedSteelQuantitys << steelQuantity;
                selectedSteelWeights << steelWeight;
                selectedSteelInitialPositions << steelInitialPosition;
                selectedSteelGoalPositions << steelGoalPositon;
            }
            qDebug() << "选中的钢材有:"<<selectedSteelType;
        }
    }
    if(!selectedSteelIDs.isEmpty())
    {
        selectedIsValid = true;
        foreach (int steelID, selectedSteelIDs)
        {
            QSqlQuery steelQuery;
            steelQuery.prepare("SELECT steelPriority FROM Steel WHERE steelID = :steelID");
            steelQuery.bindValue(":steelID", steelID);

            if (steelQuery.exec())
            {
                if (steelQuery.next())
                {
                    int steelPriority = steelQuery.value("steelPriority").toInt();
                    selectedSteelPrioritys << steelPriority;
                }
            }
        }
    }
    return selectedIsValid;
}


void NewOrder::queryDriver()
{
    //获取司机姓名和驾照信息
    QSqlQuery driverQuery;
    driverQuery.exec("SELECT * FROM Driver");
    if(!driverQuery.lastError().isValid())
    {
        QString driverName = driverQuery.value("driverName").toString();
        QString driverLicense = driverQuery.value("driverLicense").toString();
        int intDriverLicense = shiftLicense(driverLicense);
        driverLicense = QString::number(intDriverLicense);
        driverNameItems << driverName;
        driverLicenseItems << driverLicense;
    }
}
void NewOrder::queryTruck()
{
    //获取车辆类型、长度、宽度、最大油量、最大载重量、百公里油耗、需要驾照类型、货箱是否可以交换
    QSqlQuery truckQuery;
    QString truckSql = "SELECT *FROM Truck";
    truckQuery.exec(truckSql);
    while(truckQuery.next())
    {
        QString truckType = truckQuery.value("truckName").toString();
        truckTypeItems << truckType;
        QString truckSpeed = truckQuery.value("truckSpeed").toString();
        truckSpeedItems << truckSpeed;
        QString truckLength = truckQuery.value("truckLength").toString();
        truckLengthItems <<truckLength ;
        QString truckBreadth = truckQuery.value("truckBreadth").toString();
        truckBreadthItems<<truckBreadth;
        QString truckMaxFuel = truckQuery.value("truckMaxFuel").toString();
        truckMaxFuelItems<<truckMaxFuel;
        QString truckMaxWeight = truckQuery.value("truckMaxWeight").toString();
        truckMaxWeightItems<<truckMaxWeight;
        QString fuelConsumptionRatePer100KM = truckQuery.value("fuelConsumptionRatePer100KM").toString();
        fuelConsumptionRatePer100KMItems<< fuelConsumptionRatePer100KM;
        QString truckRequireLicense = truckQuery.value("truckRequireLicense").toString();
        int intTruckRequireLicense = shiftLicense(truckRequireLicense);
        truckRequireLicense = QString::number(intTruckRequireLicense);
        truckRequireLicenseItems<< truckRequireLicense;
        QString containerChangable =truckQuery.value("containerChangable").toString();
        containerChangableItems << containerChangable;
    }
}
void NewOrder::querySteel()
{

}



//查询数据库填充PDDL中距离定义（查询所有地点）
void NewOrder::queryDistance()
{
    // 获取两个地点之间距离
    QSqlQuery distanceQuery;
    QString disSql = "SELECT * FROM Distance";
    if (distanceQuery.exec(disSql))
    {
        disYardIDItems.clear();
        disOtherYardIDItems.clear();
        distanceItems.clear();

        while (distanceQuery.next())
        {
            int disYardID = distanceQuery.value("yardID").toInt();
            disYardIDItems << disYardID;
            int disOtherYardID = distanceQuery.value("otherYardID").toInt();
            disOtherYardIDItems << disOtherYardID;
            double distance = distanceQuery.value("distance").toDouble();
            distanceItems << distance;
        }
        foreach (int queryDisYardID, disYardIDItems)
        {
            QSqlQuery disYardNameQuery;
            disYardNameQuery.prepare("SELECT yardName, yardAlias FROM FreightYard WHERE yardID = :yardID");
            disYardNameQuery.bindValue(":yardID", queryDisYardID);
            if (disYardNameQuery.exec())
            {
                if (disYardNameQuery.next())
                {
                    QString disYardName = disYardNameQuery.value("yardName").toString();
                    QString disYardAlias = disYardNameQuery.value("yardAlias").toString();
                    disYardNameItems << disYardName;
                    disYardAliasItems << disYardAlias;
                 }
                qDebug() << "disYardAliasItems:"<< disYardAliasItems;
            }
        }
        foreach (int queryDisYardID, disOtherYardIDItems)
        {
            QSqlQuery disOtherYardNameQuery;
            disOtherYardNameQuery.prepare("SELECT yardName, yardAlias FROM FreightYard WHERE yardID = :yardID");
            disOtherYardNameQuery.bindValue(":yardID", queryDisYardID);
            if (disOtherYardNameQuery.exec())
            {
                if (disOtherYardNameQuery.next())
                {
                    QString disOtherYardName = disOtherYardNameQuery.value("yardName").toString();
                    QString disOtherAliasName = disOtherYardNameQuery.value("yardAlias").toString();
                    disOtherYardNameItems << disOtherYardName;
                    disOtherYardAliasItems << disOtherAliasName;
                }
            }
        }
    }
    else
    {
        qDebug() << "Distance query execution failed: " << distanceQuery.lastError().text();
    }
    QSqlQuery yardQuery;
    QString yardSql = "SELECT * FROM FreightYard";
    if (yardQuery.exec(yardSql))
    {
        yardNameItems.clear();

        while (yardQuery.next())
        {
            QString yardName = yardQuery.value("yardName").toString();
            QString yardAlias = yardQuery.value("yardAlias").toString();
            QString gasStation = yardQuery.value("gasStation").toString();
            yardNameItems << yardName;
            yardAliasItems << yardAlias;
            gasItems << gasStation;
        }
    }
    else
    {
        qDebug() << "Yard query execution failed: " << yardQuery.lastError().text();
    }

}
int NewOrder::shiftLicense(QString license)
{
    QMap<QString, int> licenseScores;
    licenseScores["A1"] = 16;
    licenseScores["A2"] = 15;
    licenseScores["A3"] = 14;
    licenseScores["B1"] = 13;
    licenseScores["B2"] = 12;
    licenseScores["C1"] = 11;
    licenseScores["C2"] = 10;
    licenseScores["C3"] = 9;
    licenseScores["C4"] = 8;
    return licenseScores.value(license, 0);
}
bool NewOrder::dataIsVaild(int column,double data)
{
    if(column == 3)
    {
        if(data > 0 && data < 20)
        {
            return 1;
        }
        else
        {
            QMessageBox::warning(this,"警告","钢材长度不合法");
            return 0;
        }
    }
    else if (column == 4)
    {
        if(data > 0 && data < 1000)
        {
            return 1;
        }
        else
        {
            QMessageBox::warning(this,"警告","钢材数量不合法");
            return 0;
        }
    }
    else if (column == 5)
    {
        if(data > 0 && data < 100)
        {
            return 1;
        }
        else
        {
            QMessageBox::warning(this,"警告","钢材重量不合法");
            return 0;
        }
    }
    else
        return 0 ;
}
void NewOrder::positioningPlan(int &startLine, int &endLine,int &planLength)
{
    QFile file("D:/QtProject/TranspotationSystem/outputFile/output.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开文件";
        return;
    }

    QTextStream in(&file);
    QString line;
    int lineNumber = 0;

    while (!in.atEnd())
    {
        lineNumber++;
        line = in.readLine();

        // 在这里检查每个搜索字符串是否在当前行中
        if (line.contains("step"))
        {
            startLine = lineNumber;
        }
        if(line.contains("plan cost"))
        {
            endLine = lineNumber - 1;
        }
        planLength = endLine - startLine;
    }

    file.close();
}
void NewOrder::acquireActionList()
{
    initActionList();
    //(\\w+)一个捕获组，用于匹配一个或多个字母、数字或下划线字符
    //d 动作序号 DRIVE-WAY w 司机   w 车辆   w 地点  w 钢材（不捕获）
    QRegularExpression driverRegex("(\\d+): GET-IN (\\w+) (\\w+) (\\w+) \\w+");

    //d 动作序号 DRIVE-WAY w 司机  w 起始点  w 终止点   w 车辆 w 钢材（不捕获）
    // 2: DRIVE-WAY SIXALXLETRUCK LOCATION1 LOCATION4 YU
    QRegularExpression driveRegex("(\\d+): DRIVE-WAY (\\w+) (\\w+) (\\w+) (\\w+) \\w+", QRegularExpression::MultilineOption);

    //d动作序号 MOVE-STEEL-CONTAINER w 地点 w 车辆   w 钢材名称  w 货箱
    QRegularExpression loadSCRegex("(\\d+): MOVE-STEEL-CONTAINER (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    //d动作序号 MOVE-STEEL-STEEL w 地点  w 车辆  w 上层钢材  w 下层钢材   w 货箱
    QRegularExpression loadSSRegex("(\\d+): MOVE-STEEL-STEEL (\\w+) (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    //d动作序号 UNLOAD-STEEL-CONTAINER  w 地点  w 车辆  w 钢材   w 货箱
    QRegularExpression unloadSCRegex("(\\d+): UNLOAD-STEEL-CONTAINER (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    //d动作序号 UNLOAD-STEEL-STEEL  w 地点  w 车辆  w 上层钢材  w 下层钢材   w 货箱
    QRegularExpression unloadSSRegex("(\\d+): UNLOAD-STEEL-STEEL (\\w+) (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    QFile file(outputFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开PDDL输出文件"+outputFilePath;
        return;
    }

    QTextStream in(&file);
    QString line;

    // 使用正则表达式查找匹配项
    while (!in.atEnd())
    {
        line = in.readLine();

        QRegularExpressionMatch driverMatch = driverRegex.match(line);
        QRegularExpressionMatch driveMatch = driveRegex.match(line);
        QRegularExpressionMatch loadSCMatch = loadSCRegex.match(line);
        QRegularExpressionMatch loadSSMatch = loadSSRegex.match(line);
        QRegularExpressionMatch unloadSCMatch = unloadSCRegex.match(line);
        QRegularExpressionMatch unloadSSMatch = unloadSSRegex.match(line);
        if (driverMatch.hasMatch())
        {
            // 获取匹配的操作行号和司机名字
            QString lineNumber = driverMatch.captured(1);
            QString driverName = driverMatch.captured(2);

            // 检查司机是否已经在Map中
            if (!driverActions.contains(driverName))
            {
                driverActions.insert(driverName, QStringList());
                driverNameList.append(driverName);
            }
        }
        if (driveMatch.hasMatch())
        {
            //d 动作序号 w 车辆 w 起始点 w 终止点 w 司机

            // 获取匹配的操作行号和司机名字
            QString lineNumber = driveMatch.captured(1);
            QString driveTruck = driveMatch.captured(5);
            QString startYard = driveMatch.captured(3);
            QString endYard = driveMatch.captured(4);
            QString driverName = driveMatch.captured(2);

            // 检查司机是否已经在Map中
            if (!driverActions.contains(driverName))
            {
                driverActions.insert(driverName, QStringList());
                driverNameList.append(driverName);
            }
            // 将操作行号添加到司机的操作列表中
            driverActions[driverName].append(lineNumber);
            driverActions[driverName].append(driveTruck);
            driverActions[driverName].append(startYard);
            driverActions[driverName].append(endYard);
        }
        if (loadSCMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = loadSCMatch.captured(1);
            QString steelType = loadSCMatch.captured(4);
            QString truckType = loadSCMatch.captured(3);
            QString yardName = loadSCMatch.captured(2);


            // 将操作行号添加到装卸的操作列表中
            loadActions[truckType].append(truckType);
            loadActions[truckType].append(lineNumber);
            loadActions[truckType].append(steelType);
            loadActions[truckType].append(yardName);
        }
        if (loadSSMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = loadSSMatch.captured(1);
            QString steelType = loadSSMatch.captured(4);
            QString truckType = loadSSMatch.captured(3);
            QString yardName = loadSSMatch.captured(2);

            // 检查车辆是否已经在Map中

            // 将操作行号添加到装卸的操作列表中
            loadActions[truckType].append(truckType);
            loadActions[truckType].append(lineNumber);
            loadActions[truckType].append(steelType);
            loadActions[truckType].append(yardName);
        }
        if (unloadSCMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = unloadSCMatch.captured(1);
            QString steelType = unloadSCMatch.captured(4);
            QString truckType = unloadSCMatch.captured(3);
            QString yardName = unloadSCMatch.captured(2);

            // 检查车辆是否已经在Map中
            if (!unloadActions.contains(truckType))
            {
                unloadActions.insert(truckType, QStringList());
                truckTypeList.append(truckType);
            }
            // 将操作行号添加到装卸的操作列表中
            unloadActions[truckType].append(truckType);
            unloadActions[truckType].append(lineNumber);
            unloadActions[truckType].append(steelType);
            unloadActions[truckType].append(yardName);
        }
        if (unloadSSMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = unloadSSMatch.captured(1);
            QString steelType = unloadSSMatch.captured(4);
            QString truckType = unloadSSMatch.captured(3);
            QString yardName = unloadSSMatch.captured(2);

            // 检查车辆是否已经在Map中
            if (!unloadActions.contains(truckType))
            {
                unloadActions.insert(truckType, QStringList());
                truckTypeList.append(truckType);
            }
            // 将操作行号添加到装卸的操作列表中
            unloadActions[truckType].append(truckType);
            unloadActions[truckType].append(lineNumber);
            unloadActions[truckType].append(steelType);
            unloadActions[truckType].append(yardName);
        }
    }
    if(!loadActions.isEmpty() && !unloadActions.isEmpty())
    {
        qDebug() << "loadActions" << loadActions;
        qDebug() << "unloadActions" << unloadActions;
    }
    else
    {
        qDebug() << "未定位到装载动作，请检查output文件";
    }

    file.close();
    if(!driverActions.isEmpty())
    {
        qDebug()<<driverActions;
    }
    else
    {
        qDebug() << "未定位司机到动作，请检查output文件";

    }
}
void NewOrder::acquireActionList(int count)
{
    initActionList();
    //(\\w+)一个捕获组，用于匹配一个或多个字母、数字或下划线字符
    //d 动作序号 DRIVE-WAY w 司机   w 车辆   w 地点  w 钢材（不捕获）
    QRegularExpression driverRegex("(\\d+): GET-IN (\\w+) (\\w+) (\\w+) \\w+");

    //d 动作序号 DRIVE-WAY w 司机  w 起始点  w 终止点   w 车辆 w 钢材（不捕获）
    // 2: DRIVE-WAY SIXALXLETRUCK LOCATION1 LOCATION4 YU
    QRegularExpression driveRegex("(\\d+): DRIVE-WAY (\\w+) (\\w+) (\\w+) (\\w+) \\w+", QRegularExpression::MultilineOption);

    //d动作序号 MOVE-STEEL-CONTAINER w 地点 w 车辆   w 钢材名称  w 货箱
    QRegularExpression loadSCRegex("(\\d+): MOVE-STEEL-CONTAINER (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    //d动作序号 MOVE-STEEL-STEEL w 地点  w 车辆  w 上层钢材  w 下层钢材   w 货箱
    QRegularExpression loadSSRegex("(\\d+): MOVE-STEEL-STEEL (\\w+) (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    //d动作序号 UNLOAD-STEEL-CONTAINER  w 地点  w 车辆  w 钢材   w 货箱
    QRegularExpression unloadSCRegex("(\\d+): UNLOAD-STEEL-CONTAINER (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    //d动作序号 UNLOAD-STEEL-STEEL  w 地点  w 车辆  w 上层钢材  w 下层钢材   w 货箱
    QRegularExpression unloadSSRegex("(\\d+): UNLOAD-STEEL-STEEL (\\w+) (\\w+) (\\w+) (\\w+) (\\w+)", QRegularExpression::MultilineOption);
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    QString outputFilePath = "D:/QtProject/TranspotationSystem/outputFile/output"+QString::number(count)+".txt";
    QFile file(outputFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开PDDL输出文件"+outputFilePath;
        return;
    }

    QTextStream in(&file);
    QString line;

    // 使用正则表达式查找匹配项
    while (!in.atEnd())
    {
        line = in.readLine();

        QRegularExpressionMatch driverMatch = driverRegex.match(line);
        QRegularExpressionMatch driveMatch = driveRegex.match(line);
        QRegularExpressionMatch loadSCMatch = loadSCRegex.match(line);
        QRegularExpressionMatch loadSSMatch = loadSSRegex.match(line);
        QRegularExpressionMatch unloadSCMatch = unloadSCRegex.match(line);
        QRegularExpressionMatch unloadSSMatch = unloadSSRegex.match(line);
        if (driverMatch.hasMatch())
        {
            // 获取匹配的操作行号和司机名字
            QString lineNumber = driverMatch.captured(1);
            QString driverName = driverMatch.captured(2);

            // 检查司机是否已经在Map中
            if (!driverActions.contains(driverName))
            {
                driverActions.insert(driverName, QStringList());
                driverNameList.append(driverName);
            }
        }
        if (driveMatch.hasMatch())
        {
            //d 动作序号 w 车辆 w 起始点 w 终止点 w 司机

            // 获取匹配的操作行号和司机名字
            QString lineNumber = driveMatch.captured(1);
            QString driveTruck = driveMatch.captured(5);
            QString startYard = driveMatch.captured(3);
            QString endYard = driveMatch.captured(4);
            QString driverName = driveMatch.captured(2);

            // 检查司机是否已经在Map中
            if (!driverActions.contains(driverName))
            {
                driverActions.insert(driverName, QStringList());
                driverNameList.append(driverName);
            }
            if (!driverLocationAliasList.contains(driverName))
            {
                driverLocationAliasList.insert(driverName, QStringList());
            }
            // 将操作行号添加到司机的操作列表中
            driverActions[driverName].append(lineNumber);
            driverActions[driverName].append(driveTruck);
            driverActions[driverName].append(startYard);
            driverActions[driverName].append(endYard);

            //以司机姓名为主键值加途径的地点
            driverLocationAliasList[driverName].append(startYard);
            driverLocationAliasList[driverName].append(endYard);
        }
        qDebug() << "driverLocationAliasList"<<driverLocationAliasList;
        if (loadSCMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = loadSCMatch.captured(1);
            QString steelType = loadSCMatch.captured(4);
            QString truckType = loadSCMatch.captured(3);
            QString yardName = loadSCMatch.captured(2);


            // 将操作行号添加到装卸的操作列表中
            loadActions[truckType].append(truckType);
            loadActions[truckType].append(lineNumber);
            loadActions[truckType].append(steelType);
            loadActions[truckType].append(yardName);
        }
        if (loadSSMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = loadSSMatch.captured(1);
            QString steelType = loadSSMatch.captured(4);
            QString truckType = loadSSMatch.captured(3);
            QString yardName = loadSSMatch.captured(2);

            // 检查车辆是否已经在Map中

            // 将操作行号添加到装卸的操作列表中
            loadActions[truckType].append(truckType);
            loadActions[truckType].append(lineNumber);
            loadActions[truckType].append(steelType);
            loadActions[truckType].append(yardName);
        }
        if (unloadSCMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = unloadSCMatch.captured(1);
            QString steelType = unloadSCMatch.captured(4);
            QString truckType = unloadSCMatch.captured(3);
            QString yardName = unloadSCMatch.captured(2);

            // 检查车辆是否已经在Map中
            if (!unloadActions.contains(truckType))
            {
                unloadActions.insert(truckType, QStringList());
                truckTypeList.append(truckType);
            }
            // 将操作行号添加到装卸的操作列表中
            unloadActions[truckType].append(truckType);
            unloadActions[truckType].append(lineNumber);
            unloadActions[truckType].append(steelType);
            unloadActions[truckType].append(yardName);
        }
        if (unloadSSMatch.hasMatch())
        {
            //d动作序号 w 钢材名称  w 货箱  w 车辆  w 地点

            // 获取匹配的操作行号和司机名字
            QString lineNumber = unloadSSMatch.captured(1);
            QString steelType = unloadSSMatch.captured(4);
            QString truckType = unloadSSMatch.captured(3);
            QString yardName = unloadSSMatch.captured(2);

            // 检查车辆是否已经在Map中
            if (!unloadActions.contains(truckType))
            {
                unloadActions.insert(truckType, QStringList());
                truckTypeList.append(truckType);
            }
            // 将操作行号添加到装卸的操作列表中
            unloadActions[truckType].append(truckType);
            unloadActions[truckType].append(lineNumber);
            unloadActions[truckType].append(steelType);
            unloadActions[truckType].append(yardName);
        }
    }
    if(!loadActions.isEmpty() && !unloadActions.isEmpty())
    {
        qDebug() << "loadActions" << loadActions;
        qDebug() << "unloadActions" << unloadActions;
    }
    else
    {
        qDebug() << "未定位到装载动作，请检查output文件";
    }

    file.close();
    if(!driverActions.isEmpty())
    {
        qDebug()<<driverActions;
    }
    else
    {
        qDebug() << "未定位司机到动作，请检查output文件";

    }
}
void NewOrder::initActionList()
{
    driverActions.clear();
    loadActions.clear();
    unloadActions.clear();
    driverNameList.clear();
}
void NewOrder::generatePicture()
{
    foreach (QString driverName, driverNameList)
    {
        QString dotFileNamePath = QString("D:/QtProject/TranspotationSystem/outputFile/%1.dot").arg(driverName);
        QFile dotFile(dotFileNamePath);

        if (!dotFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "无法创建或打开 dot 文件: " << dotFileNamePath;
            continue;
        }

        QTextStream dotStream(&dotFile);

        // 写入 dot 文件头部
        dotStream << "digraph " << driverName << " {" << "\n" ;
        dotStream << "start [shape=ellipse, label=start]" << "\n";


        // 获取该司机的所有操作行号
        QStringList actions = driverActions.value(driverName);
        QString previousEndYard; // 上一个endYard
        QStringList steelTypeList;
        steelTypeList.clear();
        for (int i = 0; i < actions.size(); i += 4)
        {
            QString actionLineNumber = actions[i];
            QString driveTruck = actions[i + 1];
            QString startYard = actions[i + 2];
            QString endYard = actions[i + 3];
            getSteelType(driveTruck,startYard,steelTypeList);
            QString joinedSteel = steelTypeList.join("\n");
            startYard = startYard +"_" + QString::number(i);
            endYard = endYard +"_" + QString::number(i);
            if (i == 0)
            {
                // 将start连接到第一个startName
                dotStream << "start -> " << startYard << "[ label=\"driver:"+ driverName +"\", fontsize=8]" "\n";
                dotStream << "start -> " << startYard << "[label =\"" +joinedSteel+"\"fontsize=8];\n";
                dotStream << startYard<< " [shape=box, label="<< startYard << "];" <<"\n";
                dotStream << endYard<< " [shape=box, label="<< endYard << "];" <<"\n";
                dotStream << startYard << "->" << endYard << "[label =\"" + QString::number((i/4)+1) + ". "+ driveTruck +"\"];\n";
                previousEndYard = endYard;
            }
            else
            {
                dotStream << previousEndYard << "->" << startYard << "[label =\"" +joinedSteel+"\"fontsize=8];\n";
                dotStream << startYard<< " [shape=box, label="<< startYard << "];" <<"\n";
                dotStream << endYard<< " [shape=box, label="<< endYard << "];" <<"\n";
                dotStream << startYard << "->" << endYard << "[label =\"" + QString::number((i/4)+1) + ". "+ driveTruck +"\"];\n";
                previousEndYard = endYard;
            }
            }

        // 写入 dot 文件尾部
        dotStream << "end [shape=ellipse, label=end]" << "\n";
        dotStream << previousEndYard <<" -> end \n";
        dotStream << "}" << "\n";

        dotFile.close();
        if(dotFile.size() != 0)
        {
            qDebug() << "生成 dot 文件成功: " << dotFileNamePath;
        }
        else
        {
            qDebug() << "未生成 dot 文件: " ;

        }

        QProcess process;
        // 设置要执行的命令
        QString command = "D:/Tools/Graphviz/bin/dot";
        QStringList arguments;
        arguments << "-Tpng";
        arguments << dotFileNamePath;
        QString outputPath = "D:/QtProject/TranspotationSystem/outputFile/" + driverName + ".png";
        arguments << "-o" << outputPath;
        picDirList << outputPath;
        // 启动进程
        process.start(command,arguments);

        // 等待进程完成
        if (process.waitForFinished())
        {
            // 将输出保存到文件
            QPixmap processPng(outputPath);
            pictureLabel->setPixmap(processPng);
            ui->label->setVisible(false);
            ui->steelLabel->setVisible(false);
            ui->steelTableView->setVisible(false);
            ui->driverTableView->setVisible(false);
            ui->truckTableView->setVisible(false);
            ui->steelLabel->setVisible(false);
            pictureLabel->show();
            ui->planButton->show();
            if (processPng.isNull())
            {
                qDebug() << "生成规划解的图片失败: " << processPng;
            }
        }
        else
        {
            qDebug() << "生成规划解图片的命令行出现错误: " << process.errorString();
        }
    }
}

void NewOrder::getSteelType(QString driveTruck,QString yardName, QStringList & steelTypeList)
{
    int startLine = 0,endLine = 0,planLength = 0;
    positioningPlan(startLine,endLine,planLength);
    QStringList removeSteelTypeList;
    // 获取该driveTruck对应的actions
    QStringList loadactions = loadActions.value(driveTruck);
    QStringList unloadactions = unloadActions.value(driveTruck);
    // 遍历actions，查找包含特定truckType的项目
    for (int i = 0; i < loadactions.size(); i += 4)
    {
        if (loadactions[i] == driveTruck && loadactions[i+3] == yardName)
        {
            QString steelType = loadactions.at(i+2);
            steelTypeList.append(steelType);
        }
    }
    for (int j = 0; (j + 3) < unloadactions.size(); j += 3)
    {
        if (unloadactions[j] == driveTruck && unloadactions[j+3] == yardName)
        {
            QString steelType = unloadactions.at(j+2);
            removeSteelTypeList.append(steelType);

        }
    }
    for (const QString &steelToRemove : removeSteelTypeList)
    {
        steelTypeList.removeAll(steelToRemove);
    }
}


void NewOrder::on_planButton_clicked()
{
    if (!picDirList.isEmpty())
    {
        QPixmap processPng(picDirList[currentPicIndex]);
        pictureLabel->setPixmap(processPng);
        currentPicIndex = (currentPicIndex + 1) % picDirList.size();
    }
}
//计算driver-way的花费
void NewOrder::driverCost()
{
    for (auto it = driverActions.constBegin(); it != driverActions.constEnd(); ++it)
    {
        // 假设 nameList 是包含所有 name 的 QStringList
        QStringList driverList = driverActions.keys();

        // 遍历 nameList
        for (const QString& driver : driverList)
        {
             get_inCost();
        }
        QString name = it.key();        // 获取driverActions中人名
        qDebug() << "name:" << name;
        QList<QString> values = it.value();     // 获取对应的值列表
        qDebug()<< "values:" << values;
        // 遍历值列表
        for (int i = 0; i < values.size(); ++i) {
            QString value = values.at(i);

            // 匹配到人名，cost + 0.1
            //QList中索引从0开始
            // 匹配到数字，读取数字后第二和第三个地点并查询数据库
            //此处location是别名
            if (i % 4 == 0 && (i + 3) < values.size())
            {
                QString truck = values.at(i + 1);
                double speed = queryTruckSpeed(truck);
                QString location1 = values.at(i + 2);
                QString location2 = values.at(i + 3);
                int location1ID = queryYardID(location1);
                int location2ID = queryYardID(location2);
                double distance = queryYardDistance(location1ID,location2ID);
                drive_wayCost(distance , speed);
            }
        }
    }
}
void NewOrder::drive_wayCost(double distance , double speed)
{
    cost += distance / speed;
}
qfloat16 NewOrder::queryTruckSpeed(QString truck)
{
    qfloat16 truckSpeed = -1;

    // 准备查询语句
    QSqlQuery query;
    query.prepare("SELECT truckSpeed FROM Truck WHERE truckType = :truckType");
    query.bindValue(":truckType", truck);

    // 执行查询
    if (query.exec()) {
        // 如果查询成功，提取结果
        if (query.next()) {
            truckSpeed = query.value("truckSpeed").toDouble();
            qDebug() << truck << "的速度"<< truckSpeed <<'\n';
        }
        else {

            qDebug() << "没有查询到:" << truck << "的速度"<< '\n';
        }
    } else {
        qDebug() << "规划解中速度查询失败:" << query.lastError().text()<< '\n';
    }
    return truckSpeed;
}
int NewOrder::queryYardID(QString yardAlias)
{
    QSqlQuery query;
    int yardID = -1;
    query.prepare("SELECT yardID FROM FreightYard WHERE yardAlias = :yardAlias");
    query.bindValue(":yardAlias", yardAlias);

    if (query.exec())
    {
        if (query.next())
        {
            yardID = query.value(0).toInt();
            qDebug() << "yardID " << yardAlias << "为:" << yardID<< '\n';
        }
        else
        {
            qDebug() << "没找到ID" << yardAlias<< '\n';
        }
    }
    else
    {
        qDebug() << "规划解中yardID查询失败:" << query.lastError().text();
    }
    return yardID;
}
qfloat16 NewOrder::queryYardDistance(int yardID,int otherYardID)
{
    qfloat16 distance = -1;
    QSqlQuery query;
    query.prepare("SELECT distance FROM Distance WHERE yardID = :yardID AND otherYardID = :otherYardID");
    query.bindValue(":yardID", yardID);
    query.bindValue(":otherYardID", otherYardID);

    if (query.exec())
    {
        if (query.next())
        {
            distance = query.value(0).toDouble();
            qDebug() << "地点1" << yardID << "地点2" << otherYardID << "之间距离:" << distance << '\n';
        } else
        {
            qDebug() << "地点1" << yardID << "地点2" << otherYardID << "之间距离未查询到"<< '\n';
        }
    } else
    {
        qDebug() << "规划解中距离查询失败:" << query.lastError().text();
    }
    return distance;
}
void NewOrder::get_inCost()
{
    cost += 0.1;
}
void NewOrder::loadCost()
{
    for (auto it = loadActions.constBegin(); it != loadActions.constEnd(); ++it)
    {
        QString truckType = it.key();        // 获取driverActions中人名
        qDebug() << "truckType:" << truckType;
        QList<QString> values = it.value();     // 获取对应的值列表
        qDebug()<< "values:" << values;
        // 遍历值列表
        for (int i = 0; i < values.size(); ++i)
        {
            QString value = values.at(i);
            //loadActions QMap(("FIVEAXLETRUCK", QList("FIVEAXLETRUCK", "13", "PIPE_1", "LOCATION1",
                                                        //"FIVEAXLETRUCK", "22", "BAR_0", "LOCATION1"))
            //QList中索引从0开始
            // 匹配到车辆，读取其后第一和第三个地点并查询数据库
            if (i % 4 == 0 && (i + 3) < values.size())
            {
                QString steelPDDL;
                steelPDDL = values.at(i + 2);
                QRegularExpression regex("(\\w+)_(\\d+)");
                QRegularExpressionMatch match = regex.match(steelPDDL);
                if (match.hasMatch())
                {
                    QString steelType = match.captured(1); // 获取匹配的钢材名称
                    QString stringSteelID = match.captured(2); // 获取匹配的钢材编号
                    int steelID = stringSteelID.toInt();
                    //从表格中获取钢材质量
                    qfloat16 weight =querySteelWeightModel(steelID);
                    load_unloadCost(weight);
                }
                else
                {
                    qDebug() << "未找到规划解的load";
                }
            }
        }
    }
}
void NewOrder::unloadCost()
{
    for (auto it = unloadActions.constBegin(); it != unloadActions.constEnd(); ++it)
    {
        QString truckType = it.key();        // 获取driverActions中人名
        qDebug() << "truckType:" << truckType;
        QList<QString> values = it.value();     // 获取对应的值列表
        qDebug()<< "values:" << values;
        // 遍历值列表
        for (int i = 0; i < values.size(); ++i)
        {
            QString value = values.at(i);
            //unloadActions QMap(("FIVEAXLETRUCK", QList("FIVEAXLETRUCK", "13", "PIPE_1", "LOCATION1",
            //"FIVEAXLETRUCK", "22", "BAR_0", "LOCATION1"))
            //QList中索引从0开始
            // 匹配到车辆，读取其后第一和第三个地点并查询数据库
            if (i % 4 == 0 && (i + 3) < values.size())
            {
                QString steelPDDL;
                steelPDDL = values.at(i + 2);
                QRegularExpression regex("(\\w+)_(\\d+)");
                QRegularExpressionMatch match = regex.match(steelPDDL);
                if (match.hasMatch())
                {
                    QString steelType = match.captured(1); // 获取匹配的钢材名称
                    QString stringSteelID = match.captured(2); // 获取匹配的钢材编号
                    int steelID = stringSteelID.toInt();
                    //从表格中获取钢材质量
                    qfloat16 weight =querySteelWeightModel(steelID);
                    load_unloadCost(weight);
                }
                else
                {
                    qDebug() << "未找到规划解的unload";
                }
            }
        }
    }
}
qfloat16 NewOrder::querySteelWeightModel(int steelID)
{
    qfloat16 weight = -1;
    QModelIndexList matches = steelModel.match(steelModel.index(0, 1), Qt::DisplayRole, steelID, -1, Qt::MatchExactly);
    foreach (const QModelIndex &index, matches)
    {
        int row = index.row(); // 匹配行的索引
        QStandardItem* singleSteelWeight = steelModel.item(row, 4); // 第5列的数据，索引从0开始
        QStandardItem* quantity = steelModel.item(row, 5); // 第6列的数据

        if (singleSteelWeight && quantity)
        {
            QString stringSingleWeight = singleSteelWeight->text(); // 第5列的数据
            float dataSingleWeight = stringSingleWeight.toFloat(); // 将文本转换为浮点数
            QString stringQuantity = quantity->text(); // 第6列的数据
            float dataQuantity = stringQuantity.toFloat(); // 将文本转换为浮点数
            weight = dataSingleWeight*  dataQuantity;
        }
        else
        {
            qDebug() << "无法获取第5和第6列的数据";
        }
    }

    // 检查 item 是否有效，并获取数据

    return weight;
}
qfloat16 NewOrder::querySteelWeightSQL(QString steelType)
{
    QSqlQuery query;
    qfloat16 weight = -1;
    //注意weight = 数据库中weight*quantity
    query.prepare("SELECT steelWeight FROM Steel WHERE steelType = :steelType");
    query.bindValue(":steelType", steelType);

    if (query.exec())
    {
        if (query.next())
        {
            weight = query.value(0).toInt();
            qDebug() << "weight " << steelType << "为:" << weight<< '\n';
        }
        else
        {
            qDebug() << "没找到重量" << steelType<< '\n';
        }
    }
    else
    {
        qDebug() << "规划解中steelWeight查询失败:" << query.lastError().text();
    }
    return weight;

}
void NewOrder::load_unloadCost(qfloat16 weight)
{
    qDebug() << "weight:" <<weight;
    cost += weight / 10;
}
bool NewOrder::outputCompare(const QString &outputFile1, const QString &outputFile2)
{
    QFile file1(outputFile1);
    QFile file2(outputFile2);

    if (!file1.open(QIODevice::ReadOnly | QIODevice::Text) || !file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开文件";
        return false;
    }

    QTextStream stream1(&file1);
    QTextStream stream2(&file2);

    QString content1 = stream1.readAll();
    QString content2 = stream2.readAll();

    return content1 != content2;
}
void NewOrder::cmdPDDL(int steelIDItems_0)
{
    //命令行执行
    // 设置工作目录
    QString workingDirectory = "D:/MetricFF/";

    // 创建一个 QProcess 对象
    QProcess process;

    // 设置工作目录
    process.setWorkingDirectory(workingDirectory);

    // 设置要执行的命令和参数
    QString command = "ff-v2.1.exe";
    QStringList arguments;
    arguments << "-o" << "TransportationSystem_order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-f" << "order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-s" << "0";

    // 启动进程
    process.start(command, arguments);
    // 等待进程完成
    if (process.waitForFinished(10000))
    {
        // 读取命令输出
        QString output;
        while (process.canReadLine())
        {
            QString line = process.readLine();
            output.append(line + "\n"); // 将每一行输出追加到字符串中
        }
        // 将输出保存到文件
        QFile outputFile(outputFilePath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&outputFile);
            stream << output;
            outputFile.close();
        }
        else
        {
            qDebug() << "无法保存PDDL输出文件!";
        }
    }
    else
    {
        qDebug() << "PDDL生成程序出错!";
    }
}
void NewOrder::cmdPDDL(int steelIDItems_0,qfloat16 cost)
{
    //命令行执行
    // 设置工作目录
    QString workingDirectory = "D:/MetricFF/";

    // 创建一个 QProcess 对象
    QProcess process;

    // 设置工作目录
    process.setWorkingDirectory(workingDirectory);

    // 设置要执行的命令和参数
    QString command = "ff-v2.1.exe";
    QStringList arguments;
    arguments << "-o" << "TransportationSystem_order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-f" << "order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-s" << "0" << "-b" << QString::number(cost);

    // 启动进程
    process.start(command, arguments);
    // 等待进程完成
    if (process.waitForFinished(10000))
    {
        // 读取命令输出
        QString output;
        while (process.canReadLine())
        {
            QString line = process.readLine();
            output.append(line + "\n"); // 将每一行输出追加到字符串中
        }
        // 将输出保存到文件
        QFile outputFile(outputFilePath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&outputFile);
            stream << output;
            outputFile.close();
        }
        else
        {
            qDebug() << "无法保存PDDL输出文件!";
        }
    }
    else
    {
        qDebug() << "PDDL生成程序出错!";
    }
}
void NewOrder::cmdPDDL(int steelIDItems_0,qfloat16 cost,int count)
{
    QString filePath = "D:/QtProject/TranspotationSystem/outputFile/output"+QString::number(count)+".txt";
    //命令行执行
    // 设置工作目录
    QString workingDirectory = "D:/MetricFF/";

    // 创建一个 QProcess 对象
    QProcess process;

    // 设置工作目录
    process.setWorkingDirectory(workingDirectory);

    // 设置要执行的命令和参数
    QString command = "ff-v2.1.exe";
    QStringList arguments;
    arguments << "-o" << "TransportationSystem_order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-f" << "order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-s" << "0" << "-b" << QString::number(cost);
    // 启动进程
    process.start(command, arguments);
    // 等待进程完成
    if (process.waitForFinished(10000))
    {
        // 读取命令输出
        QString output;
        while (process.canReadLine())
        {
            QString line = process.readLine();
            output.append(line + "\n"); // 将每一行输出追加到字符串中
        }
        // 将输出保存到文件
        QFile outputFile(filePath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&outputFile);
            stream << output;
            outputFile.close();
        }
        else
        {
            qDebug() << "无法保存PDDL输出文件!";
        }
    }
    else
    {
        qDebug() << "PDDL生成程序出错!";
    }
}
void NewOrder::cmdPDDL(int steelIDItems_0,int count)
{
    QString filePath = "D:/QtProject/TranspotationSystem/outputFile/output"+QString::number(count)+".txt";
    //命令行执行
    // 设置工作目录
    QString workingDirectory = "D:/MetricFF/";

    // 创建一个 QProcess 对象
    QProcess process;

    // 设置工作目录
    process.setWorkingDirectory(workingDirectory);

    // 设置要执行的命令和参数
    QString command = "ff-v2.1.exe";
    QStringList arguments;

    arguments << "-o" << "TransportationSystem_order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-f" << "order_"+QString::number(steelIDItems_0)+"_pro.pddl" << "-s" << "0" << "-b" << QString::number(cost);
    qDebug() << "arguments" << arguments;
    // 启动进程
    process.start(command, arguments);
    qDebug() << command<<arguments;
    // 等待进程完成
    if (process.waitForFinished(10000))
    {
        // 读取命令输出
        QString output;
        while (process.canReadLine())
        {
            QString line = process.readLine();
            output.append(line + "\n"); // 将每一行输出追加到字符串中
        }

        qDebug() << output;
        // 将输出保存到文件
        QFile outputFile(filePath);
        if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&outputFile);
            stream << output;
            outputFile.close();
            qDebug() << "PDDL输出文件保存在 " + filePath;
        }
        else
        {
            qDebug() << "无法保存PDDL输出文件!";
        }
    }
    else
    {
        qDebug() << "PDDL生成程序出错!";
    }
}
void NewOrder::calculateCost()
{
    cost = 0;
    qDebug() << "cost1" << cost;
    driverCost();
    qDebug() << "cost2" << cost;
    loadCost();
    qDebug() << "cost3" << cost;
    unloadCost();
    qDebug() << "cost3" << cost;
}
QStringList NewOrder::queryAlias(QStringList & yardNameList)
{
    QStringList yardAliasList;
    foreach (QString yardName, yardNameList)
    {
        QSqlQuery disYardAliasQuery;
        disYardAliasQuery.prepare("SELECT yardAlias FROM FreightYard WHERE yardName = :yardName");
        disYardAliasQuery.bindValue(":yardName", yardName);
        if (disYardAliasQuery.exec())
        {
            if (disYardAliasQuery.next())
            {
                QString yardAlias = disYardAliasQuery.value("yardAlias").toString();
                yardAliasList << yardAlias;
            }
            qDebug() << "disYardAliasItems:"<< disYardAliasItems;
        }
    }
    return yardAliasList;
}
QString NewOrder::queryAlias(QString & yardName)
{
    QString yardAlias;
    QSqlQuery disYardAliasQuery;
    disYardAliasQuery.prepare("SELECT yardAlias FROM FreightYard WHERE yardName = :yardName");
    disYardAliasQuery.bindValue(":yardName", yardName);
    if (disYardAliasQuery.exec())
    {
        if (disYardAliasQuery.next())
        {
            yardAlias = disYardAliasQuery.value("yardAlias").toString();
        }
        qDebug() << "disYardAliasItems:"<< disYardAliasItems;
    }
    return yardAlias;
}
void NewOrder::writeMapMain()
{
    QString startLocation;
    QString endLocation;
    Point startPoint;
    Point endPoint;
    QList<Point> approachPointList;
    QList<Point> allPointList;
    QList<Point> poiList; // 保存查询到的 POI 点的列表
    queryPOI(poiList);
    sortPoint(poiList, startPoint,endPoint,approachPointList,allPointList);
    QFile file(mapMainFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        // 清空文件内容
        file.resize(0);
        QTextStream stream(&file);
        stream << "from functions import path, get_location_x_y, path, html_path, openHtml\n";
        stream << "def main(start_location, end_location, start_point, approach_point, end_point):\n";
        stream << "\tpath_data=path(start_location,end_location)\n";
        stream << "\thtml_path(path_data,start_point,approach_point,end_point)\n";
        stream << "\topenHtml()\n";
        stream << "if __name__ == '__main__':\n";
        stream << "\tstart_point = (\""+QString::number(startPoint.longitude, 'f', 6)+"\",\""+QString::number(startPoint.latitude, 'f', 6)+"\")\n";
        stream << "\tend_point = (\""+QString::number(endPoint.longitude, 'f', 6)+"\",\""+QString::number(endPoint.latitude, 'f', 6)+"\")\n";
        stream << "\tstart_location = \""+QString::number(startPoint.longitude, 'f', 6)+","+QString::number(startPoint.latitude, 'f', 6)+"\"\n";
        stream << "\tend_location = \""+QString::number(endPoint.longitude, 'f', 6)+","+QString::number(endPoint.latitude, 'f', 6)+"\"\n";
        stream << "\tapproach_point = (";
        QStringList pointList; // 使用QStringList来存储所有点的字符串
        foreach (Point point, approachPointList)
        {
            QString longitudeStr = QString::number(point.longitude, 'f', 6);
            QString latitudeStr = QString::number(point.latitude, 'f', 6);
            QString pointStr = QString("[%1,%2]").arg(longitudeStr).arg(latitudeStr);
            pointList << pointStr; // 将每个点的字符串添加到列表中
        }
        // 使用join()方法将列表中的所有字符串使用逗号连接起来，避免了在最后一个点后面加逗号
        QString pointsStr = pointList.join(",");
        stream << pointsStr;
        stream << ")\n"; // 在所有点添加完毕后闭合括号\n
        stream << "\tmain(start_location, end_location, start_point, approach_point, end_point)\n";
        file.close();
    }
    openMap();
}
void NewOrder::openMap()
{
    MapHtml *map = new MapHtml(this);
    map->openMapHtml();
}
void NewOrder::queryPOI (QList<Point> &poiList)
{

    // 数据库中的表格为 FreightYard，其中包含 yardAlias、POI_longitude 和 POI_latitude 字段
    foreach (const QString &key, driverLocationAliasList.keys())
    {
        QList<QString> locations = driverLocationAliasList.value(key);
        foreach (const QString &location, locations)
        {
            QSqlQuery query;
            query.prepare("SELECT POI_longitude, POI_latitude FROM FreightYard WHERE yardAlias = :yardAlias");
            query.bindValue(":yardAlias", location);
            if (query.exec() && query.next()) {
                double longitude = query.value(0).toDouble();
                double latitude = query.value(1).toDouble();
                // 创建 Point 对象并存储到 poiList 列表中
                Point poi(longitude, latitude);
                poiList.append(poi);
            } else {
                qDebug() << "Error: Failed to execute query or no data found for location" << location;
            }
        }
    }
}
void NewOrder::sortPoint(const QList<Point> &poiList, Point &startPoint, Point &endPoint, QList<Point> &approachPointList,QList<Point> &allPointList)
{
    // 清空之前的数据
    approachPointList.clear();

    // 将 poiList 中的第一个点设置为 startPoint
    if (!poiList.isEmpty()) {
        startPoint = poiList.first();
    }

    // 将 poiList 中的最后一个点设置为 endPoint
    if (!poiList.isEmpty()) {
        endPoint = poiList.last();
    }

    // 将 poiList 中除了第一个和最后一个点之外的其他点添加到 approachPointList 中
    for (int i = 1; i < poiList.size() - 1; ++i) {
        approachPointList.append(poiList[i]);
    }

}

