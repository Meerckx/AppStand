#include "tcpclient.h"

#include <QDebug>
#include <regex>

TcpClient::TcpClient(QObject *parent)
    : QObject(parent)
    , socket(new QTcpSocket(this))
{
    std::string ipAddrStr = "127.0.0.1";
    port = 50101;

    FILE *confFile = fopen("stand_params.conf", "r");
    if (confFile)
    {
        int symb;
        std::string fileStr;
        std::smatch match;
        std::regex regexCheckIpPort("[0-9]{1,3}\\.[0-9]{1,3}\\."
                                    "[0-9]{1,3}\\.[0-9]{1,3}\\:"
                                    "[0-9]+");

        while((symb = fgetc(confFile)) != EOF)
            if (symb != '\n' && symb != '\r')
              fileStr += symb;

        if (std::regex_match(fileStr, match, regexCheckIpPort))
        {
            std::vector<std::string> tokenizedFileStr;
            std::regex delimiter(":");
            tokenizedFileStr = std::vector<std::string>(
                        std::sregex_token_iterator(fileStr.begin(),
                                                   fileStr.end(),
                                                   delimiter, -1), {});
            port = std::stoi(tokenizedFileStr[1]);
            ipAddrStr = tokenizedFileStr[0];
        }
    }

    hostAddress.setAddress(ipAddrStr.c_str());

    qDebug() << "Try TCP conect on " << ipAddrStr.c_str() << ":" << port << Qt::endl;

    connect(this, &TcpClient::connectToHost, this, &TcpClient::onConnectToHost);
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
    do
    {
        QByteArray socData = socket->read(sizeof(OpHeader));
        OpHeader header = *(OpHeader*)socData.data();

        buffer.seek(0);
        while (buffer.size() < header.length)
        {
           if (!socket->bytesAvailable())
           {
               socket->waitForReadyRead(10000);
           }
           buffer.write(socket->read(header.length - buffer.size()));
        }

        switch (header.type)
        {
        case (quint32)OpType::OP_00:
            emit getDevices_Op00(buffer);
            break;
        case (quint32)OpType::OP_01:
            emit getChannels_Op01(buffer);
            break;
        case (quint32)OpType::OP_02:
            qDebug() << "OP_02 is received";
            break;
        case (quint32)OpType::OP_03:
            emit getWords_Op03(buffer);
            break;
        case (quint32)OpType::OP_04:
            qDebug() << "OP_04 is received";
            emit stopReceiving();
            emit connectToHost();
            break;
        }

        buffer.buffer().clear();
    } while (socket->bytesAvailable());
}

void TcpClient::onSendRequest_Op00()
{
    OpHeader header = {(quint32)OpType::OP_00, (quint32)OpDataSize::SEND_OP_00};

    QByteArray request(sizeof(OpHeader), 0);

    *(OpHeader*)request.data() = header;

    qDebug() << request << Qt::endl;
    socket->write(request);
}


void TcpClient::onSendRequest_Op01(qint32 index, bool rx)
{
    qDebug() << "onSendRequest_Op01: " << index << rx << Qt::endl;

    quint16 size = sizeof(OpHeader) + sizeof(Send_Op01);
    QByteArray request(size, 0);

    OpHeader header = {(quint32)OpType::OP_01, sizeof(Send_Op01)};
    Send_Op01 data = {index, rx, !rx, {0}};

    *(OpHeader*)request.data() = header;
    *(Send_Op01*)(request.data() + sizeof(OpHeader)) = data;

    qDebug() << request << Qt::endl;
    socket->write(request);
}


void TcpClient::onSendRequest_Op02(const QVector<ReqData_Op02>& requests)
{
    qDebug() << "onSendRequest_Op02: " << requests.size() << Qt::endl;

    quint32 dataLength = sizeof(Send_Op02) * requests.size();
    qint32 size = sizeof(OpHeader) + dataLength;
    QByteArray reqToSend(size, 0);

    OpHeader header = {(quint32)OpType::OP_02, dataLength};
    *(OpHeader*)reqToSend.data() = header;

    for (qint32 i = 0; i < requests.size(); i++)
    {
        qint32 shift = sizeof(OpHeader) + i * sizeof(Send_Op02);

        Send_Op02 data = { requests[i].device->getIndex(),
                           requests[i].device->getCurrentChannel()->getIndex(),
                           requests[i].label_0_63,
                           requests[i].label_64_127,
                           requests[i].label_128_191,
                           requests[i].label_192_255 };

        *(Send_Op02*)(reqToSend.data() + shift) = data;
    }

    qDebug() << requests.size() << reqToSend << Qt::endl;
    socket->write(reqToSend);
}

void TcpClient::onSendRequest_Op04()
{
    qDebug() << "onSendRequest_Op04" << Qt::endl;
    OpHeader header = {(quint32)OpType::OP_04, (quint32)OpDataSize::SEND_OP_04};

    QByteArray request(sizeof(OpHeader), 0);

    *(OpHeader*)request.data() = header;

    qDebug() << request << Qt::endl;
    socket->write(request);
}
