#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QBuffer>
#include <QDataStream>

#include "operationsdata.h"


/* Класс для описания функционирования клиентской части */
class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    QTcpSocket* getSocket();


signals:
    void connectToHost();
    void getDevices_Op00(QBuffer& buffer);
    void getChannels_Op01(QBuffer& buffer);
    void getWords_Op03(QBuffer& buffer);
    void sendRequest_Op00();
    void checkRequestsToRestore();
    void stopReceiving();
//    void startTimer();

public slots:
    void onReadyRead();
    void onConnected();
    void onConnectToHost();
    void onSendRequest_Op01(qint32 index, bool rx);
    void onSendRequest_Op02(const QVector<ReqData_Op02>& requests);
    void onSendRequest_Op04();

private slots:
    void onSendRequest_Op00();

private:
    QTcpSocket *socket;         // Сокет, через который идёт подключение
    QHostAddress hostAddress;   // Адрес хоста для подключения
    quint16 port;               // Порт для подключения

    quint32 opCode = 0;                     // Код операции при общении со стендом
    quint32 lenBytes = 0;                   // Длина передаваемых данных в байтах

    QBuffer buffer;

    bool isOpActive = false;
};

#endif // TCPCLIENT_H
