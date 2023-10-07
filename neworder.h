#ifndef NEWORDER_H
#define NEWORDER_H

#include <QWidget>
#include <QStandardItemModel>
#include <QtSql>
#include <QDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>
#include <QFile>

namespace Ui {
class NewOrder;
}

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

    void on_recreateButton_clicked();

    void onSteelTableViewDoubleClicked(QModelIndex index);
private:
    Ui::NewOrder *ui;
    QStandardItemModel driverModel;
    QStandardItemModel truckModel;
    QStandardItemModel steelModel;
    void SetCurrentRowChecked();
    void showSteel();
    void showDriver();
    void showTruck();
    bool bDriverWidget;
    bool bSteelWidget;
    void shiftTableView(bool bDriverWidget, bool bSteelWidget);
    void selectedDriver(int &);
    void selectedTruck(int &);
    bool selectedSteel(QList<int> &selectedSteelIDs,
                       QStringList &selectedSteelTypes,
                       QList<double> &selectedSteelLengths,
                       QList<double> &selectedSteelQuantitys,
                       QList<double> &selectedSteelWeights,
                       QList<int> &selectedSteelPrioritys,
                       QStringList &steelInitialPositions,
                       QStringList &steelGoalPositions);
    void writeFile();
    void writeDefine();
    void writeInit();
    void queryDriver();
    void queryTruck();
    void querySteel();
    void queryDistance();
    int shiftLicense(QString);
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
    QStringList yardItems;
    QStringList gasItems;
    QList<int> disYardIDItems;
    QStringList disYardNameItems;
    QList<int> disOtherYardIDItems;
    QStringList disOtherYardNameItems;
    QList<double> distanceItems;
    QStringList driverNameItems;
    QStringList driverLicenseItems;
    QVector<QComboBox*> initialComboBoxArray;
    QVector<QComboBox*> goalComboBoxArray;

};

#endif // NEWORDER_H
