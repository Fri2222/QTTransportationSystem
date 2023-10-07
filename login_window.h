#ifndef LOGIN_WINDOW_H
#define LOGIN_WINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QSqlDatabase *db;

protected:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);

        // Load the background image
        QPixmap backgroundPixmap("D:/QtProject/LibraryManagementSystem/backgroundPic.jpg");

        // Create a painter
        QPainter painter(this);

        // Draw the background image
        painter.drawPixmap(0, 0, width(), height(), backgroundPixmap);

        // Call the base class paintEvent to ensure proper rendering
        QMainWindow::paintEvent(event);
    }
private slots:
    void on_loginButton_clicked();
    void on_exitButton_clicked();
};
#endif // LOGIN_WINDOW_H
