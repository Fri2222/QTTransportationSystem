#ifndef ACCOUNTMANGEMENT_H
#define ACCOUNTMANGEMENT_H

#include <QWidget>
#include "neworder.h"
#include "orderandaccount.h"
#include <QPushButton>
#include "User.h"

namespace Ui {
class AccountMangement;
}

class AccountMangement : public QWidget
{
    Q_OBJECT

public:
    explicit AccountMangement(QWidget *parent = nullptr);
    ~AccountMangement();
    void initPage();
    void setUser(User user);
private slots:
    void on_newOrderButton_clicked();

private:
    Ui::AccountMangement *ui;
    NewOrder *mNewOrder;
    OrderAndAccount *mOrderAndAccount;
    QPushButton *newOrderButton;
    QPushButton *orderHistoryButton;
    QPushButton *basicInformationButton;
    User currentUser;
    User &operator=(const User &user)
    {
        currentUser.userId = user.userId;
        currentUser.userName = user.userName;
        currentUser.password = user.password;
        currentUser.authority = user.authority;
        return currentUser;
    }
    void showAccount();
    QPushButton *changePasswordButton;

};

#endif // ACCOUNTMANGEMENT_H
