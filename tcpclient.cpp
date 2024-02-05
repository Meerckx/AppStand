#include "tcpclient.h"

#include <QDebug>

typedef struct server_answer
{
    long long time;
    long long deltaTime;
    uint32_t word;
} server_answer_t;


TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
    , socket(new QTcpSocket(this))
{
    qDebug() << "TcpClient Constructor" << Qt::endl;

    hostAddress.setAddress("127.0.0.1");
//    port = 50001;
    port = 6000;
}

TcpClient::~TcpClient()
{
    buffer.close();
}

QTcpSocket* TcpClient::getSocket()
{
    return socket;
}

void TcpClient::onConnected()
{
    qDebug() << "Connected!" << Qt::endl;

}

void TcpClient::onConnectToHost()
{
    qDebug() << "onConnectToHost" << Qt::endl;
    if (socket->state() != QTcpSocket::ConnectedState)
    {
        buffer.open(QIODevice::ReadWrite);
        socket->connectToHost(hostAddress, port);
        connect(socket, &QAbstractSocket::connected, this, &TcpClient::onConnected);
        connect(socket, &QIODevice::readyRead, this, &TcpClient::onReadyRead);
    }

    QByteArray request;
    request.resize(8);
    *(quint32*)request.data() = 0;

    *(quint32*)(request.data() + 4) = 0;

    qDebug() << request << Qt::endl;
    socket->write(request);
}

void TcpClient::onReadyRead()
{
    qDebug() << "onReadyRead" << Qt::endl;
    if (!isOpActive)
    {
        qDebug() << "!isOpActive" << Qt::endl;
        QByteArray socData = socket->read(sizeof(quint32));
        opCode = *(quint32*)socData.data();

        socData = socket->read(sizeof(quint32));
        lenBytes = *(quint32*)socData.data();

        qDebug() << opCode << " " << lenBytes << Qt::endl;
    }

    isOpActive = true;
    while(buffer.size() < lenBytes)
    {
       if (!socket->bytesAvailable())
       {
           socket->waitForReadyRead(10000);
       }
       buffer.write(socket->read(lenBytes - buffer.size()));
       qDebug() << lenBytes - buffer.size() << Qt::endl;
    }

    switch (opCode)
    {
    case (quint32)OpType::OP_00:
        emit getDevices_Op00(buffer);
        break;
    case (quint32)OpType::OP_01:
        emit getChannels_Op01(buffer);
        break;
    }

    QByteArray trash = socket->readAll();   // Потом реализовать очистку принятых данных в случае закрытия диалогового окна принудительно
    buffer.buffer().clear();    // Вроде бы очищает буффер
    isOpActive = false;

//    QByteArray data = socket.readLine();
//    int rows = ui->tableExchange->rowCount();
//    int cols = ui->tableExchange->columnCount();
//    ui->tableExchange->setRowCount(rows + 1);
//    QTableWidgetItem *item = new QTableWidgetItem(QString(data.toHex()));
//    item->setTextAlignment(Qt::AlignCenter);
//    ui->tableExchange->setItem(rows, 6, item);

//    ui->tableExchange->scrollToBottom();

//    qDebug() << data.toHex();
//    socket.write(QByteArray("HEHEHE\n"));
}

void TcpClient::onSendRequest_Op01(qint32 index, bool rx)
{
    qDebug() << "onSendRequest_Op01: " << index << rx << Qt::endl;
    QByteArray request(14, 0);
    *(quint32*)request.data() = 1;
    *(quint32*)(request.data() + 4) = 6;        // Потом вписать в размер enum уже готовый

    *(qint32*)(request.data() + 8) = index;
    *(bool*)(request.data() + 12) = rx;
    *(bool*)(request.data() + 13) = !rx;
    qDebug() << request << Qt::endl;
    socket->write(request);
}

void TcpClient::readData_Op00(quint32 length, quint32 OpCode)
{
    qDebug() << "readOperation_00" << Qt::endl;
    isOpActive = true;

    while(buffer.size() < length)
    {
       if (!socket->bytesAvailable())
       {
           socket->waitForReadyRead(10000);
       }
       buffer.write(socket->read(length - buffer.size()));
       qDebug() << length - buffer.size() << Qt::endl;
    }

    emit getDevices_Op00(buffer);
    QByteArray trash = socket->readAll();   // Потом реализовать очистку принятых данных в случае закрытия диалогового окна принудительно
    isOpActive = false;
}
