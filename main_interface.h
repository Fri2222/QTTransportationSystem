#ifndef MAIN_INTERFACE_H
#define MAIN_INTERFACE_H

#include <QMainWindow>
#include <QApplication>
#include <QToolButton>
#include "accountmangement.h"
#include "basicinformation.h"
#include "cargoinformation.h"
#include "yardinformation.h"
#include <QPushButton>

namespace Ui {
class main_interface;
}

class main_interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit main_interface(QWidget *parent = nullptr);
    ~main_interface();
    void initPage();
private slots:
    void onNavigationButtonClicked();

private:
    Ui::main_interface *ui;
    AccountMangement *mAccountMangement;
    BasicInformation *mBasicInformation;
    CargoInformation *mCargoInformation;
    YardInformation *mYardInformation;
    QToolButton *accountmanageButton;
    QToolButton *basicInformationButton;
    QToolButton *cargoInformationButton;
    QToolButton *yardInformationButton;
    QToolButton *mLastPressedButton;

};

#endif // MAIN_INTERFACE_H
