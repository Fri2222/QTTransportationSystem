#include "orderandaccount.h"
#include "ui_orderandaccount.h"

OrderAndAccount::OrderAndAccount(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderAndAccount)
{
    ui->setupUi(this);
}

OrderAndAccount::~OrderAndAccount()
{
    delete ui;
}
