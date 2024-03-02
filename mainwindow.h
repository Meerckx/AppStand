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
    void sendRequest_Op04();
    void setMsecUpdateRowsTimer(quint16 time);

public slots:
    void onCreateRowsForWords(Words_t& words);
    void onUpdateTableExchange(Words_t& words);
    void onSetRowEmpty(quint16 rowNumber);

private slots:
    void on_btnDevProps_clicked();
    void on_leUpdateRowsTimeout_returnPressed();

private:
    Ui::MainWindow *ui;
    ExchangeData *exchangeData;
    TcpClient *client;

    quint16 colsCount;
    enum exchangeColumnPercents
    {
        DEVICE = 10,
        CHANNEL = 10,
        LABEL = 10,
        TIME = 10,
        DELTA = 10,
        SSM = 10,
        WORD_BIN = 25,
        WORD_HEX = 15
    };

    void setupUi();
    void setItemsText(WordData* word, QStringList& itemsText);
};
#endif // MAINWINDOW_H
