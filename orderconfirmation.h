#ifndef ORDERCONFIRMATION_H
#define ORDERCONFIRMATION_H

#include <QDialog>
#include <QStandardItemModel>
#include <QComboBox>
#include <QtSql>
#include <QSqlQuery>
#include <QString>
#include <QStackedWidget>

namespace Ui {
class orderConfirmation;
}

class orderConfirmation : public QDialog
{
    Q_OBJECT

public:
    explicit orderConfirmation(QWidget *parent = nullptr);
    ~orderConfirmation();
    QStandardItemModel orderModel;
    QStandardItemModel conditionModel;

    void sendID(QList<int>);
    void sendType(QStringList );
    void sendWeight(QList<double>);
    void sendPriority(QList<int>);
    void buildSteelMap();
    void sendInitialPosition(QStringList );
    void sendGoalPosition(QStringList );
    void sendYardItem(QStringList );
    void showOrder();
    void showCondition();
    void showCondition(int row);

private slots:
    void on_cdtConformationPushButton_clicked();

    void on_conditionPushButton_clicked();

    void on_addButton_clicked();

private:
    Ui::orderConfirmation *ui;
    QStackedWidget *mStackedWidget;
    QWidget *mConditionWidget;
    QWidget *mOrderWidget;
    QList<int> selectedSteelIDs;
    QStringList selectedSteelTypes;
    QList<double> selectedSteelWeights;
    QList<int> selectedSteelPrioritys;
    QStringList steelInitialPositions;
    QStringList steelGoalPositions;
    QStringList yardItems;
    QString ltlStatement;
    QVector<QComboBox*> specifyIDComboBoxArray;
    QVector<QComboBox*> specifySteelComboBoxArray;
    QVector<QComboBox*> specialRequirementsComboBoxArray;
    QVector<QComboBox*> targetIDComboBoxArray;
    QVector<QComboBox*> targetSteelComboBoxArray;
    void initPage();
    void queryYard();
    void invokeLTL();
    void generateLTLPy();
    void dotGeneratesPDDL();
    class Transtion{
        public:
        QString firstState;
        QString secondState;
        QStringList conditions;
    };
    QMap<int,QString> steelIDToType;
    QList<int> querySteelID(const QString &specifySteel);
    QVector<Transtion *> transitionList;
    QStringList replaceCondition(QString );
    QString changeStr(QString originalString);
    //在当前文件夹的ouputFile中生成.py文件的备份
    QString pyFilePath = "D:/QtProject/TranspotationSystem/outputFile/order.py";
    //在Linux与Windows的共享文件夹下生成.py文件
    QString sharePyFilePath = "D:/sharedFolder/order.py";
    // 设置要执行的 Python 脚本路径 启动linux 让linux将共享文件夹中.py文件生成.dot文件
    QString scriptPath = "D:/QtProject/TranspotationSystem/outputFile/ssh.py";
    //dot文件所在路径
    QString dotFilePath = "D:/sharedFolder/order.dot";
    //解析dot文件为符合PDDL语法的约束 将约束以追加的方式添加在domain文件中 下为domain文件路径
    QString PDDLFilePath = "D:/MetricFF/TransportationSystem.pddl";

};
#endif // ORDERCONFIRMATION_H
