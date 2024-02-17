#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QScreen>

#include "exchangedata.h"
#include "tcpclient.h"
#include "deviceproperties.h"
#include "operationsdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    quint16 getColsCount();

signals:
    void connectToHost();

public slots:
//    void onUpdateTableExchange(const QVector<WordData>& words, quint16 start);
    void onUpdateTableExchange(QMap<quint8, WordData*>& words);

private slots:
    void on_btnDevProps_clicked();

private:
    Ui::MainWindow *ui;
    ExchangeData *exchangeData;
    TcpClient *client;

    quint16 colsCount;

    void setItemsText(const WordData& word, quint16 rowIndex, QStringList& itemsText);
};
#endif // MAINWINDOW_H
