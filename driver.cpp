#include "driver.h"
#include "ui_driver.h"

Driver::Driver(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Driver)
{
    ui->setupUi(this);
    ui->tableView->setModel(&mmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mmodel.setHorizontalHeaderLabels({"司机ID","司机姓名","驾照类型"});
}

Driver::~Driver()
{
    delete ui;
}
void Driver::on_addDriverButton_clicked()
{
    bool bName = false;
    QString driverName = QInputDialog::getText(this,
                                               "司机姓名",
                                               "请输入司机姓名",
                                               QLineEdit::Normal,
                                               "",
                                               &bName
                                               );
    QStringList licenseIterms;
    licenseIterms <<"A1"<<"A2"<<"A3"<<"B1"<<"B2"<<"C1"<<"C2"<<"C3"<<"C4";
    //1表示默认选择第一个选项，false表示下拉菜单中的选项不可编辑
    QString driverLicense =QInputDialog::getItem(this,
                                                  "司机信息",
                                                  "请选择司机驾照等级",
                                                  licenseIterms,
                                                  0,
                                                  false) ;

    QSqlQuery addQuery;
    addQuery.prepare("INSERT INTO Driver (driverID, driverName, driverLicense,) "
                      "VALUES (:driverID, :driverName, :driverLicense)");
    int driverID = addQuery.lastInsertId().toInt();
    addQuery.bindValue(":driverID",driverID);
    addQuery.bindValue(":driverName",driverName);
    addQuery.bindValue(":driverLicense",driverLicense);
    addQuery.exec();
    showDriver();
    if(!addQuery.exec())
    {
        QMessageBox::warning(this,"警告","添加司机信息时出现错误，请联系管理员");
    }
}
void Driver::showDriver()
{
    QSqlQuery driverQuery;
    QString sql = "SELECT *FROM Driver";
    driverQuery.exec(sql);
    if(driverQuery.lastError().isValid())
    {
        qDebug() << "Database Error:" << driverQuery.lastError().text();
    }
    else
    {
        mmodel.removeRows(0,mmodel.rowCount());
        while(driverQuery.next())
        {
            int tableDriverID = driverQuery.value("driverID").toInt();
            QString tableDriverName = driverQuery.value("driverName").toString();
            QString tableDriverLicense = driverQuery.value("driverLicense").toString();         
            QList<QStandardItem *> driverItem;
            driverItem.append(new QStandardItem(QString::number(tableDriverID)));
            driverItem.append(new QStandardItem(tableDriverName));
            driverItem.append(new QStandardItem(tableDriverLicense));
            mmodel.appendRow(driverItem);
        }
    }
}

void Driver::on_deleteButton_clicked()
{
    //实例化选中行
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();

    // 检查是否有选中的行
    if (selectedRows.isEmpty())
    {
        QMessageBox::warning(this, "警告", "请先选择要删除的司机信息");
        return;
    }

    // 提示用户确认删除操作
    int result = QMessageBox::question(this, "确认删除", "确定要删除所选司机信息吗？",
                    QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        QSqlDatabase db = QSqlDatabase::database(); // 获取数据库连接
        QSqlQuery query(db);

        // 遍历选中的行并删除
        foreach (const QModelIndex &index, selectedRows)
        {
            int row = index.row();
            QVariant driverId = ui->tableView->model()
                                    ->data(ui->tableView->model()->index(row, 0));
            // 构建 SQL 查询来删除司机信息
            QString deleteQuery = "DELETE FROM Driver WHERE driverId = :driverId";
            query.prepare(deleteQuery);
            query.bindValue(":driverId", driverId);
            showDriver();
            if (!query.exec())
            {
                qDebug() << "删除司机信息失败:" << query.lastError().text();
            }
        }

    }
}

