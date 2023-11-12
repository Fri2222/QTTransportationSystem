#include "neworder.h"
#include "ui_neworder.h"

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

    bDriverWidget = false;
    bSteelWidget = true;

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
    //int lastColumn = steelModel.columnCount();
    for(int row = 0;row < lastRow ;row ++)
    {
        for(int conlumn = 6;conlumn < 7 ;conlumn++)
        {
            QComboBox* steelInitialPositionCmb = new QComboBox();
            steelInitialPositionCmb->addItems({yardItems});
            steelInitialPositionCmb->setStyleSheet("QComboBox { border-radius: 3px; border: 1px; selection-color: black; selection-background-color: darkgray; } QFrame { border: 0px; } QComboBox::drop-down{background-color: white;}");
            ui->steelTableView->setIndexWidget(steelModel.index (row,conlumn), steelInitialPositionCmb);
            // 将 QComboBox 指针添加到 QVector 中
            initialComboBoxArray.append(steelInitialPositionCmb);
        }
        for(int conlumn = 7;conlumn < 8 ;conlumn++)
        {
            QComboBox* steelGoalPositionCmb = new QComboBox();
            steelGoalPositionCmb->addItems({yardItems});
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
    bSelectedSteel = selectedSteel(steelIDItems,
                                  selectedSteelType,
                                  selectedSteelLengths,
                                  selectedSteelQuantitys,
                                  selectedSteelWeights,
                                  selectedSteelPrioritys,
                                  selectedSteelInitialPositions,
                                  selectedSteelGoalPositions);
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
            define.append("(define (problem order_"+QString::number(steelIDItems[0])+"_p ) (:domain bug)\n");
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
            define.append(yardItems.join(" ") + " - location\n)");
            define.append("\n");
            stream << define ;
    //init定义
            QString init = "";
        //time-cost money-cost函数定义
            init.append("\n(:init\n(= (time-cost) 0)\n(= (money-cost) 0)\n");
        //driver的姓名、驾照、位置（未写）定义
            for (int i = 0; i < driverNameItems.size(); ++i)
            {
                const QString& driverName = driverNameItems[i];
                init.append("(= (license " + driverName + ") " + driverLicenseItems[i] + ")\n");
                init.append("( driver-in " + driverName + " location1 )\n");
            }
            init.append("\n");
        //truck基本信息定义
            for (int i = 0; i < truckTypeItems.size(); ++i)
            {
                const QString& truckType = truckTypeItems[i];
                //const QString& truckPosition = truckPositionItems[i];
                const QString& truckSpeed = truckSpeedItems[i];

                init.append("( truck-in " + truckType + " location1 )\n");

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
                init.append("( connected-by-way "+disYardNameItems[i]+" "+disOtherYardNameItems[i]+" )\n");
                init.append("(= (distance "+disYardNameItems[i]+" "+disOtherYardNameItems[i]+" )"+QString::number(distanceItems[i])+")\n");
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
                const QString& steelType = selectedSteelType[i];
                const QString& steelPosition = selectedSteelInitialPositions[i];
                init.append("( steel-in "+ steelType + "_"+QString::number(steelIDItems[i]) + " " + steelPosition + ")\n");

            }
            init.append(")\n");
            stream << init;
    //goal
            QString goal = "";
            goal.append("(:goal (and\n");
            for(int i = 0;i < selectedSteelType.size();i++)
            {
                const QString& steelType = selectedSteelType[i];
                const QString& steelPosition = selectedSteelGoalPositions[i];
                goal.append("( steel-in "+ steelType + "_"+QString::number(steelIDItems[i]) + " " + steelPosition + ")\n");
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
            QMessageBox::information(this,"提示","您已经成功创建订单");
            file.close();
        }
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
        arguments << "-o" << "bug.pddl" << "-f" << "order_"+QString::number(steelIDItems[0])+"_pro.pddl" << "-s" << "0";

        // 启动进程
        process.start(command, arguments);

        // 等待进程完成
        if (process.waitForFinished())
        {
            // 读取命令输出
            QString output = process.readAllStandardOutput();
            // 将输出保存到文件
            QString outputFilePath = "D:/QtProject/TranspotationSystem/outputFile/output.txt";
            QFile outputFile(outputFilePath);
            if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream stream(&outputFile);
                stream << output;
                outputFile.close();
                qDebug() << "Output saved to " + outputFilePath;
            }
            else
            {
                qDebug() << "Failed to save output to file!";
            }
        }
        else
        {
            qDebug() << "Command Failed to Execute!";
        }

    }
    else
    {
        QMessageBox::warning(this,"警告","请检查订单中的钢材");
    }
    generatePicture();
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


void NewOrder::on_recreateButton_clicked()
{

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
bool NewOrder::selectedSteel(QList<int> &selectedSteelIDs,
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


void NewOrder::writeFile()
{

}

void NewOrder::writeDefine()
{

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
            disYardNameQuery.prepare("SELECT yardName FROM FreightYard WHERE yardID = :yardID");
            disYardNameQuery.bindValue(":yardID", queryDisYardID);
            if (disYardNameQuery.exec())
            {
                if (disYardNameQuery.next())
                {
                    QString disYardName = disYardNameQuery.value("yardName").toString();
                    disYardNameItems << disYardName;
                }
            }
        }
        foreach (int queryDisYardID, disOtherYardIDItems)
        {
            QSqlQuery disOtherYardNameQuery;
            disOtherYardNameQuery.prepare("SELECT yardName FROM FreightYard WHERE yardID = :yardID");
            disOtherYardNameQuery.bindValue(":yardID", queryDisYardID);
            if (disOtherYardNameQuery.exec())
            {
                if (disOtherYardNameQuery.next())
                {
                    QString disOtherYardName = disOtherYardNameQuery.value("yardName").toString();
                    disOtherYardNameItems << disOtherYardName;
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
        yardItems.clear();

        while (yardQuery.next())
        {
            QString yardName = yardQuery.value("yardName").toString();
            QString gasStation = yardQuery.value("gasStation").toString();
            yardItems << yardName;
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
void NewOrder::generatePicture()
{
    //(\\w+)一个捕获组，用于匹配一个或多个字母、数字或下划线字符
    //\\s+一个空白字符的匹配，用于匹配一个或多个空格字符
    QRegularExpression driverRegex("(\\d+): GET-IN (\\w+)\\s+.*");

    //d 动作序号 DRIVE-WAY w 车辆   w 起始点   w 终止点   w 司机
    // 2: DRIVE-WAY SIXALXLETRUCK LOCATION1 LOCATION4 YU
    QRegularExpression driveRegex("(\\d+): DRIVE-WAY (\\w+) (\\w+) (\\w+) (\\w+)");

    //d动作序号 MOVE-STEEL-CONTAINER  w 钢材名称  w 货箱  w 车辆  w 地点
    QRegularExpression loadSCRegex("(\\d+): MOVE-STEEL-CONTAINER (\\w+) (\\w+) (\\w+) (\\w+)");
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);
    QRegularExpression loadSSRegex("(\\d+): MOVE-STEEL-STEEL (\\w+) (\\w+) (\\w+) (\\w+)");
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    QRegularExpression unloadSCRegex("(\\d+): UNLOAD-STEEL-CONTAINER (\\w+) (\\w+) (\\w+) (\\w+)");
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);
    QRegularExpression unloadSSRegex("(\\d+): UNLOAD-STEEL-STEEL (\\w+) (\\w+) (\\w+) (\\w+)");
    loadSCRegex.setPatternOptions(QRegularExpression::MultilineOption);

    QFile file("D:/QtProject/TranspotationSystem/outputFile/output.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "无法打开文件";
        return;
    }

    QTextStream in(&file);
    QString line;
    QStringList driverNameList;
    QStringList truckTypeList;

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
            QString driveTruck = driveMatch.captured(2);
            QString startYard = driveMatch.captured(3);
            QString endYard = driveMatch.captured(4);
            QString driverName = driveMatch.captured(5);

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
            QString steelType = loadSCMatch.captured(2);
            QString truckType = loadSCMatch.captured(4);
            QString yardName = loadSCMatch.captured(5);


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
            QString steelType = loadSSMatch.captured(2);
            QString truckType = loadSSMatch.captured(4);
            QString yardName = loadSSMatch.captured(5);

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
            QString steelType = unloadSCMatch.captured(2);
            QString truckType = unloadSCMatch.captured(3);
            QString yardName = unloadSCMatch.captured(5);

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
            QString steelType = unloadSSMatch.captured(2);
            QString truckType = unloadSSMatch.captured(3);
            QString yardName = unloadSSMatch.captured(5);

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
    qDebug() << "loadActions" << loadActions;
    qDebug() << "unloadActions" << unloadActions;

    file.close();

    qDebug()<<driverActions;

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
            qDebug() << "steelList:" <<steelTypeList;
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

        qDebug() << "生成 dot 文件成功: " << dotFileNamePath;

        QProcess process;
        // 设置要执行的命令
        QString command = "D:/software/graphviz/bin/dot";
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
            //QPixmap processPng("D:/QtProject/TranspotationSystem/outputFile/"+driverName+".png");
            QPixmap processPng(outputPath);
            pictureLabel->setPixmap(processPng);
            ui->label->setVisible(false);
            ui->steelLabel->setVisible(false);
            ui->steelTableView->setVisible(false);
            ui->driverTableView->setVisible(false);
            ui->truckTableView->setVisible(false);
            ui->steelLabel->setVisible(false);
            pictureLabel->show();
            if (processPng.isNull())
            {
                qDebug() << "Failed to load image from: " << processPng;
            }
        }
        else
        {
            qDebug() << "Command Failed to Execute with error: " << process.errorString();
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
