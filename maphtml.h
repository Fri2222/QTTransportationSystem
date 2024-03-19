#ifndef MAPHTML_H
#define MAPHTML_H

#include <QWidget>
#include <QStandardItemModel>
#include <QtSql>
#include <QMessageBox>
namespace Ui {
class MapHtml;
}

class MapHtml : public QWidget
{
    Q_OBJECT

public:
    explicit MapHtml(QWidget *parent = nullptr);
    ~MapHtml();
    void openMapHtml();

private:
    Ui::MapHtml *ui;

};

#endif // MAPHTML_H
