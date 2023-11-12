#ifndef ORDERANDACCOUNT_H
#define ORDERANDACCOUNT_H

#include <QWidget>

namespace Ui {
class OrderAndAccount;
}

class OrderAndAccount : public QWidget
{
    Q_OBJECT

public:
    explicit OrderAndAccount(QWidget *parent = nullptr);
    ~OrderAndAccount();

private:
    Ui::OrderAndAccount *ui;
};

#endif // ORDERANDACCOUNT_H
