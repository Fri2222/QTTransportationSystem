#ifndef CARGOINFORMATION_H
#define CARGOINFORMATION_H

#include <QWidget>
#include <QtSql>
#include <QStandardItemModel>
#include <QInputDialog>
#include <QMessageBox>
struct defaultSteel
{
    QString steelType;
    double steelPriority;
};
namespace Ui {
class CargoInformation;
}

class CargoInformation : public QWidget
{
    Q_OBJECT

public:
    explicit CargoInformation(QWidget *parent = nullptr);
    ~CargoInformation();
    void showSteel();
private slots:
    void on_wholeButton_clicked();
    void on_addButton_clicked();

private:
    Ui::CargoInformation *ui;
    QStandardItemModel mmodel;
};

#endif // CARGOINFORMATION_H
