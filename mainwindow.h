#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QScreen>

#include "exchangedata.h"
#include "tcpclient.h"
#include "deviceproperties.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void connectToHost();

public slots:

private slots:
    void on_btnDevProps_clicked();

private:
    Ui::MainWindow *ui;
    ExchangeData *exchangeData;
    TcpClient *client;
};
#endif // MAINWINDOW_H
