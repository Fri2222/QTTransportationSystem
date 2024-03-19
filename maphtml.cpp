#include "maphtml.h"
#include "ui_maphtml.h"

MapHtml::MapHtml(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MapHtml)
{
    ui->setupUi(this);
}

MapHtml::~MapHtml()
{
    delete ui;
}
void MapHtml::openMapHtml()
{
    //命令行执行
    // 设置工作目录
    QString scriptPath = "D:/pythonProject/map/execute.py";
    // 创建 QProcess 对象
    QProcess process;
    // 设置要执行的命令
    QString command = "python";
    QStringList arguments;
    arguments << scriptPath;

    // 启动命令
    process.start(command, arguments);
    // 等待命令执行完成
    process.waitForFinished();
    // 读取输出结果
    QByteArray output = process.readAllStandardOutput();
    QByteArray error = process.readAllStandardError();
    // 打印输出和错误信息
    qDebug() << "Output:" << output;
    qDebug() << "Error:" << error;
}
