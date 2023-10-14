#ifndef TRUCK_H
#define TRUCK_H

#include <QWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QtSql>
//利用结构体存储固定类型的truck默认参数
struct defaultTruck
{
    QString truckType;
    int fuelConsumptionRatePer100KM;
    QString truckRequireLicense;
    QString containerChangable;
};
namespace Ui {
class Truck;
}

class Truck : public QWidget
{
    Q_OBJECT

public:
    explicit Truck(QWidget *parent = nullptr);
    ~Truck();
    void showTruck();
private slots:
    void on_addTruckButton_clicked();
private:
    Ui::Truck *ui;
    QStandardItemModel mmodel;
    void defaultType(QString & ,int & ,QString &,QString &);
};

#endif // TRUCK_H
