#ifndef BASICINFORMATION_H
#define BASICINFORMATION_H

#include <QWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QPushButton>
#include "driver.h"
#include "truck.h"

namespace Ui {
class BasicInformation;
}

class BasicInformation : public QWidget
{
    Q_OBJECT

public:
    explicit BasicInformation(QWidget *parent = nullptr);
    ~BasicInformation();


private slots:
    void initPage();

    void on_searchLineEdit_textEdited(const QString &arg1);

    void onNavigationButtonClicked();

private:
    Ui::BasicInformation *ui;
    Driver *mDriver;
    Truck *mTruck;
    QPushButton *driverButton;
    QPushButton *TruckButton;
};

#endif // BASICINFORMATION_H
