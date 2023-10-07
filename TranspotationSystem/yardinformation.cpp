#include "yardinformation.h"
#include "ui_yardinformation.h"

YardInformation::YardInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YardInformation)
{
    ui->setupUi(this);
    ui->tableView->setModel(&mmodel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    showDistance();
}

YardInformation::~YardInformation()
{
    delete ui;
}

void YardInformation::on_positionButton_clicked()
{
    showDistance();
}
void YardInformation::on_wholeYardButton_clicked()
{
    showYard();
}


void YardInformation::IDQueryYardName(int yardID ,QString &yardName)
{
    QSqlQuery yardNameQuery;
    yardNameQuery.prepare("SELECT yardName FROM FreightYard WHERE yardID = :yardID");
    yardNameQuery.bindValue(":yardID", yardID);
    if (yardNameQuery.exec() && yardNameQuery.next())
    {
        yardName = yardNameQuery.value(0).toString();
    }

}
void YardInformation::showYard()
{
    QSqlQuery yardQuery;
    QString sql = "SELECT *FROM FreightYard";
    if(yardQuery.exec(sql))
    {
        mmodel.clear();
        mmodel.setHorizontalHeaderLabels({"货场ID","货场名称"});
        while(yardQuery.next())
        {
            int yardID = yardQuery.value("yardID").toInt();
            QString yardName = yardQuery.value("yardName").toString();
            QList<QStandardItem *> yardItem;
            yardItem.append(new QStandardItem(QString::number(yardID)));
            yardItem.append(new QStandardItem(yardName));
            mmodel.appendRow(yardItem);
        }
    }
    else
    {
        qDebug() << "Database Error:" << yardQuery.lastError().text();
    }
}

void YardInformation::showDistance()
{
    QSqlQuery distanceQuery;
    QString sql = "SELECT *FROM Distance";
    if(distanceQuery.exec(sql))
    {
        mmodel.clear();
        mmodel.setHorizontalHeaderLabels({"起始货场名称","终止货场名称","距离"});

        while(distanceQuery.next())
        {
            int yardID = distanceQuery.value("yardID").toInt();
            int otherYardId = distanceQuery.value("otherYardId").toInt();
            int distance = distanceQuery.value("distance").toInt();
            QString yardName = "";
            QString otherYardName = "";
            IDQueryYardName(yardID,yardName);
            IDQueryYardName(otherYardId,otherYardName);
            QList<QStandardItem *> distanceItem;
            distanceItem.append(new QStandardItem(yardName));
            distanceItem.append(new QStandardItem(otherYardName));
            distanceItem.append(new QStandardItem(QString::number(distance)));
            mmodel.appendRow(distanceItem);
        }
    }
    else
    {
        qDebug() << "Database Error:" << distanceQuery.lastError().text();
    }
}



