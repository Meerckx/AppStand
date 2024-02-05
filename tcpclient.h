#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QBuffer>
#include <QDataStream>

enum class OpType: quint32
{
    OP_00 = 0x00,
    OP_01,
    OP_02,
    OP_03,
    OP_04
};

/* Класс для описания функционирования клиентской части */
class TcpClient : public QObject
{
    Q_OBJECT
public:
    explicit TcpClient(QObject *parent = nullptr);
    ~TcpClient();

    QTcpSocket* getSocket();

signals:
    void getDevices_Op00(QBuffer& buffer);
    void getChannels_Op01(QBuffer& buffer);

public slots:
    void onReadyRead();
    void onConnected();
    void onConnectToHost();
    void onSendRequest_Op01(qint32 index, bool rx);

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
