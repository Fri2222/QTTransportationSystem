#ifndef ENROLL_H
#define ENROLL_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

namespace Ui {
class Enroll;
}

class Enroll : public QDialog
{
    Q_OBJECT

public:
    explicit Enroll(QWidget *parent = nullptr);
    ~Enroll();

private slots:
    void on_CommitButton_clicked();

    void on_CancelButton_clicked();

    void on_NameLineEdit_editingFinished();

    void on_PasswordLineEdit_editingFinished();

    void on_EnsureLineEdit_editingFinished();

private:
    Ui::Enroll *ui;
    QLineEdit *nameLineEdit;
    QLineEdit *passwordLineEdit;
    QLineEdit *ensureLineEdit;
    int comformationPassword;
    class RegisterUser
    {
    public:
        int userId;
        int password;
        QString userName;
        QString authority;
    };
    RegisterUser registerUser;

};

#endif // ENROLL_H
