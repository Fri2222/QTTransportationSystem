#ifndef NEWORDER_H
#define NEWORDER_H

#include <QWidget>
#include <QStandardItemModel>
#include <QtSql>
#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QFile>
#include <QRegularExpression>
#include <QDebug>
#include <QMap>
#include <orderandaccount.h>
#include <QTableView.h>
#include <QtGlobal>
#include <QtWebView/qwebview_global.h>
#include <QtWebView/QtWebView>
#include <maphtml.h>
#include <cmath>
namespace Ui {
class NewOrder;
}
class Point;
class NewOrder : public QWidget
{
    Q_OBJECT
public:
    explicit NewOrder(QWidget *parent = nullptr);
    ~NewOrder();
    int orderID;

private slots:
    void on_selectDTButton_clicked();
    void on_selectSButton_clicked();
    void on_commitButton_clicked();
    void onSteelTableViewDoubleClicked(QModelIndex index);
    void on_planButton_clicked();

private:
    Ui::NewOrder *ui;
    //司机 货车 钢材表格
    QStandardItemModel driverModel;
    QStandardItemModel truckModel;
    QStandardItemModel steelModel;
    //设置复选框
    void SetCurrentRowChecked();
    //显示表格中内容
    void showSteel();
    void showDriver();
    void showTruck();
    //设置表格显示
    bool bDriverWidget;
    bool bSteelWidget;
    void shiftTableView(bool bDriverWidget, bool bSteelWidget);
    //选中的司机 货车 钢材的基本信息
    void selectedDriver(int &);
    void selectedTruck(int &);
    bool scanSelectedSteel(QList<int> &selectedSteelIDs,
                       QStringList &selectedSteelTypes,
                       QList<double> &selectedSteelLengths,
                       QList<double> &selectedSteelQuantitys,
                       QList<double> &selectedSteelWeights,
                       QList<int> &selectedSteelPrioritys,
                       QStringList &steelInitialPositions,
                       QStringList &steelGoalPositions);
    //从数据库查询基本资料
    void queryDriver();
    void queryTruck();
    void querySteel();
    void queryDistance();
    //将司机驾照按照字库进行转换（PDDL问价中以数字表示驾照）
    int shiftLicense(QString);
    //定位plan
    void positioningPlan(int &startLine, int &endLine,int &planLength);
    //依照outputFile生成每个司机单独的dot文件生成规划解的图片
    void generatePicture();
    //保存当前所有复选框的状态
    QMap<int, Qt::CheckState> steelCheckboxStates;
    bool dataIsVaild(int column,double data);
    QStringList truckTypeItems;
    QStringList truckSpeedItems;
    QStringList truckLengthItems;
    QStringList truckBreadthItems;
    QStringList truckMaxFuelItems;
    QStringList truckMaxWeightItems;
    QStringList fuelConsumptionRatePer100KMItems;
    QStringList truckRequireLicenseItems;
    QStringList containerChangableItems;
    QStringList yardNameItems;
    QStringList yardAliasItems;
    QStringList gasItems;
    QList<int> disYardIDItems;
    QStringList disYardNameItems;
    QStringList disYardAliasItems;
    QList<int> disOtherYardIDItems;
    QStringList disOtherYardNameItems;
    QStringList disOtherYardAliasItems;
    QList<double> distanceItems;
    QStringList driverNameItems;
    QStringList driverLicenseItems;
    QVector<QComboBox*> initialComboBoxArray;
    QVector<QComboBox*> goalComboBoxArray;
    QLabel *pictureLabel;
    QStringList picDirList;
    int currentPicIndex = 0;
    void getSteelType(QString driveTruck,QString yardName,QStringList & steelTypeList);
    // 用于存储司机与开车移动的关系
    QMap<QString, QStringList> driverActions;
    //用于记录车辆装载钢材的关系
    QMap<QString, QStringList> loadActions;
    //用于记录车辆卸载钢材的关系
    QMap<QString, QStringList> unloadActions;
    //保存PDDL命令行程序生成结果
    QString outputFilePath = "D:/QtProject/TranspotationSystem/outputFile/output.txt";
    qfloat16 cost;
    //司机动作花费的”主函数“
    void driverCost();
    int queryYardID(QString yardName);
    qfloat16 queryYardDistance(int yardID,int otherYardID);
    qfloat16 queryTruckSpeed(QString truck);
    //DRIVE-WAY花费的计算
    void drive_wayCost(double distance , double speed);
    //GET-IN花费的计算
    void get_inCost();
    //装卸载动作花费的”主函数“
    void loadCost();
    void unloadCost();
    qfloat16 querySteelWeightSQL(QString steelType);
    qfloat16 querySteelWeightModel(int steelID);
    //MOVE-STEEL-X 和 UNLOAD-STEEL-X的花费计算
    void load_unloadCost(qfloat16 weight);
    bool outputCompare(const QString &outputFile1, const QString &outputFile2);
    //PDD生成的命令行
    void cmdPDDL(int steelIDItems_0,int count);
    void cmdPDDL(int steelIDItems_0,qfloat16 cost,int count);
    void cmdPDDL(int steelIDItems_0,qfloat16);
    void cmdPDDL(int steelIDItems_0);
    void calculateCost();
    //获取outputFile中的动作列表
    void acquireActionList();
    //获取outputFile加count(即最后一次生成文件)中的动作列表
    void acquireActionList(int count);
    void initActionList();
    QStringList driverNameList;
    QStringList truckTypeList;
    QStringList queryAlias(QStringList &);
    QString queryAlias(QString &);
    void openMap();
    void writeMapMain();
    //保存outputFile中出现的LOCATION
    //以driver姓名为主值的locationAlias列表（LOCATIONX）
    QMap<QString, QStringList> driverLocationAliasList;
    QString mapMainPath = "D:/QtProject/TranspotationSystem/outputFile/main.py";
    void queryPOI(QList<Point> &poiList);
    void sortPoint(const QList<Point> &poiList, Point &startPoint, Point &endPoint, QList<Point> &approachPointList,QList<Point> &allPointList);
    QString mapMainFilePath =  "D:/pythonProject/map/main.py";
};
class Point {
public:
    double longitude;
    double latitude;

    // 构造函数
    Point(double lon, double lat) : longitude(lon), latitude(lat) {}
    Point() : longitude(0), latitude(0) {}
};

#endif // NEWORDER_H
