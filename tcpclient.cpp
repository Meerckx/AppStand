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

    hostAddress.setAddress("10.10.10.222");
//    port = 50001;
    port = 50101;

    connect(this, &TcpClient::sendRequest_Op00, this, &TcpClient::onSendRequest_Op00);
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

    emit checkRequestsToRestore();
    emit sendRequest_Op00();
}

void TcpClient::onReadyRead()
{
    qDebug() << "onReadyRead" << Qt::endl;
    if (!isOpActive)
    {
        isOpActive = true;
        qDebug() << "!isOpActive" << Qt::endl;
        QByteArray socData = socket->read(sizeof(quint32));
        opCode = *(quint32*)socData.data();
        socData.clear();
        socData = socket->read(sizeof(quint32));
        lenBytes = *(quint32*)socData.data();

        qDebug() << opCode << " " << lenBytes << Qt::endl;

        buffer.seek(0);
        while (buffer.size() < lenBytes)
        {
           if (!socket->bytesAvailable())
           {
               socket->waitForReadyRead(10000);
           }
           buffer.write(socket->read(lenBytes - buffer.size()));
           qDebug() << lenBytes - buffer.size() << Qt::endl;
        }

        // Надо написать защиту от неправилього приёма данных (Сравнивать с последней отправленной операцией)
        switch (opCode)
        {
        case (quint32)OpType::OP_00:
            emit getDevices_Op00(buffer);
            break;
        case (quint32)OpType::OP_01:
            emit getChannels_Op01(buffer);
            break;
        case (quint32)OpType::OP_02:
            qDebug() << "OP_02 is recieved";
            break;
        case (quint32)OpType::OP_03:
            emit getWords_Op03(buffer);
            break;
        }

        QByteArray trash = socket->readAll();   // Потом реализовать очистку принятых данных в случае закрытия диалогового окна принудительно
        buffer.buffer().clear();    // Вроде бы очищает буффер

        isOpActive = false;
    }
}

void TcpClient::onSendRequest_Op00()
{
    QByteArray request(8, 0);
    *(quint32*)request.data() = (quint32)OpType::OP_00;

    *(quint32*)(request.data() + 4) = (quint32)OpDataSize::SEND_OP_00;

    qDebug() << request << Qt::endl;
    socket->write(request);
}


void TcpClient::onSendRequest_Op01(qint32 index, bool rx)
{
    qDebug() << "onSendRequest_Op01: " << index << rx << Qt::endl;
    QByteArray request(16, 0);
    *(quint32*)request.data() = (quint32)OpType::OP_01;
    *(quint32*)(request.data() + 4) = (quint32)OpDataSize::SEND_OP_01;

    *(qint32*)(request.data() + 8) = index;
    *(bool*)(request.data() + 12) = rx;
    *(bool*)(request.data() + 13) = !rx;
    *(quint16*)(request.data() + 14) = 0;   // Выравнивание
    qDebug() << request << Qt::endl;
    socket->write(request);
}


void TcpClient::onSendRequest_Op02(const QVector<ReqData_Op02>& requests)
{
    qDebug() << "onSendRequest_Op02: " << requests.size() << Qt::endl;

    qint32 size = requests.size() * (quint32)OpDataSize::SEND_OP_02 + 8;

    QByteArray reqToSend(size, 0);
    *(quint32*)reqToSend.data() = (quint32)OpType::OP_02;
    *(quint32*)(reqToSend.data() + 4) = quint32(requests.size() * (quint32)OpDataSize::SEND_OP_02);

    for (qint32 i = 0; i < requests.size(); i++)
    {
        qint32 shift = i * (quint32)OpDataSize::SEND_OP_02 + 8;
        *(qint32*)(reqToSend.data() + shift) = requests[i].device->getIndex();
        *(qint32*)(reqToSend.data() + shift + 4) = requests[i].device->getCurrentChannel()->getIndex(); // sizeof(...) надо
        *(quint64*)(reqToSend.data() + shift + 8) = requests[i].label_0_63;
        *(quint64*)(reqToSend.data() + shift + 16) = requests[i].label_64_127;
        *(quint64*)(reqToSend.data() + shift + 24) = requests[i].label_128_191;
        *(quint64*)(reqToSend.data() + shift + 32) = requests[i].label_192_255;
    }

    qDebug() << reqToSend << Qt::endl;
    socket->write(reqToSend);
}
