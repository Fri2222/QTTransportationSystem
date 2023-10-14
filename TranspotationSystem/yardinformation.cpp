#include "yardinformation.h"
#include "ui_yardinformation.h"

YardInformation::YardInformation(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::YardInformation),
    pictureLabel(nullptr)
{
    ui->setupUi(this);
    pictureLabel = ui->pictureLabel;
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
    QString sql = "SELECT * FROM Distance";
    if (distanceQuery.exec(sql))
    {
        mmodel.clear();
        mmodel.setHorizontalHeaderLabels({"货场名称","另一货场名称","距离"});

        //创建文件并且打开后清空文件内容
        QFile file("D:/QtProject/TranspotationSystem/outputFile/distance.dot");
        file.open(QIODevice::Append | QIODevice::Text);
        file.resize(0);
        QTextStream stream(&file);
        stream << "digraph DistanceGraph {" << "\n";
        QSet<QString> processedRoads; // 用于跟踪已处理的道路
        while (distanceQuery.next())
        {
            int yardID = distanceQuery.value("yardID").toInt();
            int otherYardId = distanceQuery.value("otherYardId").toInt();
            int distance = distanceQuery.value("distance").toInt();
            QString yardName = "";
            QString otherYardName = "";
            IDQueryYardName(yardID, yardName);
            IDQueryYardName(otherYardId, otherYardName);

            // 构建道路标识符，确保它在两个方向上都是唯一的
            QString roadIdentifier = yardName + "-" + otherYardName;

            if (!processedRoads.contains(roadIdentifier))
            {
                processedRoads.insert(roadIdentifier); // 添加道路标识符以防止重复处理

                // 将道路连接写入 DOT 文件，包括双向箭头
                 stream << yardName << " -> " << otherYardName <<
                    " [label=\"" << distance << "\"];" << "\n";
            }

            QList<QStandardItem *> distanceItem;
            distanceItem.append(new QStandardItem(yardName));
            distanceItem.append(new QStandardItem(otherYardName));
            distanceItem.append(new QStandardItem(QString::number(distance)));
            mmodel.appendRow(distanceItem);
        }
        stream << "}";
        file.close();
    }
    else
    {
        qDebug() << "fail to query distance";
    }
    generatePicture();
}

void YardInformation::generatePicture()
{
    // 设置工作目录
    QString workingDirectory = "D:/QtProject/TranspotationSystem/outputFile";

    // 创建一个 QProcess 对象
    QProcess process;

    // 设置工作目录
    process.setWorkingDirectory(workingDirectory);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", "D:/software/graphviz/bin/"); // 添加 Graphviz 的路径
    process.setProcessEnvironment(env);

    // 设置要执行的命令和参数
    QString command = "D:/software/graphviz/bin/dot";
    QStringList arguments;
    arguments << "-Tpng D:/QtProject/TranspotationSystem/outputFile/distance.dot";
    arguments << "-o D:/QtProject/TranspotationSystem/outputFile/distance.png";

    // 启动进程
    process.start(command,arguments);

    // 等待进程完成
    if (process.waitForFinished())
    {
        // 将输出保存到文件
        QString outputPath = "D:/QtProject/TranspotationSystem/outputFile/distance.png";
        QPixmap distancePng(outputPath);
        pictureLabel->setPixmap(distancePng);
        pictureLabel->show();
        qDebug() << "Command Success to Execute";
    }
    else
    {
        qDebug() << "Command Failed to Execute with error: " << process.errorString();
    }

}

