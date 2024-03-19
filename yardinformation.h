#ifndef YARDINFORMATION_H
#define YARDINFORMATION_H

#include <QWidget>
#include <QStandardItemModel>
#include <QtSql>
#include <QMessageBox>

namespace Ui {
class YardInformation;
}

class YardInformation : public QWidget
{
    Q_OBJECT

public:
    explicit YardInformation(QWidget *parent = nullptr);
    ~YardInformation();
    void showYard();
    void showDistance();

private slots:
    void on_positionButton_clicked();

    void on_wholeYardButton_clicked();

private:
    Ui::YardInformation *ui;
    QStandardItemModel mmodel;
    void IDQueryYardName(int yardId,QString &yardName);
    void generatePicture();
    QLabel *pictureLabel;
    QString dotPath = "D:/Tools/Graphviz/bin/dot";
    QString distanceDocPath = "D:/QtProject/TranspotationSystem/outputFile/distance.dot";
    QString distancePicPath = "D:/QtProject/TranspotationSystem/outputFile/distance.jpg";
};

#endif // YARDINFORMATION_H
