#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QBuffer>
#include <QDataStream>

enum OpType: quint16
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

signals:
    void readyToProcess_00(QBuffer& buffer);

public slots:
    void onReadyRead();
    void onConnected();

private:
    QTcpSocket *socket;         // Сокет, через который идёт подключение
    QHostAddress hostAddress;   // Адрес хоста для подключения
    quint16 port;               // Порт для подключения

    quint16 opCode = 0;                     // Код операции при общении со стендом
    quint32 lenBytes = 0;                   // Длина передаваемых данных в байтах

    QBuffer buffer;

    bool isOpActive = false;

    void readOperation_00(quint32 len);
};

#endif // TCPCLIENT_H
