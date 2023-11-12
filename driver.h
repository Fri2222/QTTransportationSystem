#ifndef DRIVER_H
#define DRIVER_H

#include <QWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItem>
#include <QtSql>
#include <QDebug>
#include <QDialog>


namespace Ui {
class Driver;
}

class Driver : public QWidget
{
    Q_OBJECT
private slots:
    void on_addDriverButton_clicked();
    void on_deleteButton_clicked();

public:
    explicit Driver(QWidget *parent = nullptr);
    ~Driver();
    void showDriver();
private:
    Ui::Driver *ui;
    QStandardItemModel mmodel;

};

#endif // DRIVER_H
