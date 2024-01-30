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
    socket->connectToHost(hostAddress, port);

    buffer.open(QIODevice::ReadWrite);

    connect(socket, &QAbstractSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QIODevice::readyRead, this, &TcpClient::onReadyRead);
}

TcpClient::~TcpClient()
{

}

void TcpClient::onReadyRead()
{
    qDebug() << "onReadyRead" << Qt::endl;
    if (!isOpActive)
    {
        qDebug() << "!isOpActive" << Qt::endl;
        QByteArray socData = socket->read(sizeof(quint16));
        opCode = *(quint16*)socData.data();

        socData = socket->read(sizeof(quint32));
        lenBytes = *(quint32*)socData.data();

        qDebug() << opCode << " " << lenBytes << Qt::endl;
    }

    switch (opCode)
    {
    case OP_00:
        readOperation_00(lenBytes);
        break;
    }

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

void TcpClient::onConnected()
{
    qDebug() << "Connected!" << Qt::endl;
    QByteArray request;
    request.resize(6);
    *(uint16_t*)request.data() = 0;

    *(uint32_t*)(request.data() + 2) = 0;

    qDebug() << request << Qt::endl;
    socket->write(request);
}

void TcpClient::readOperation_00(quint32 length)
{
    qDebug() << "readOperation_00" << Qt::endl;
    isOpActive = true;
    //qDebug() << length - buffer.size() << Qt::endl;

    while(buffer.size() < length)
    {
       if (!socket->bytesAvailable())
       {
           socket->waitForReadyRead(10000);
       }
       buffer.write(socket->read(length - buffer.size()));
       qDebug() << length - buffer.size() << Qt::endl;
    }
    //stream.device()->seek(0);
    //QByteArray n = buffer.read(4);
//    quint32 first = *(quint32*)buffer.data();
//    quint32 second = *(quint32*)(buffer.data() + 4);
//    quint32 third = *(quint32*)(buffer.data() + 8);
//    quint32 fourth = *(quint32*)(buffer.data() + 12);

//    stream >> opCode;
//    stream >> lenBytes;
//    stream >> first;
//    stream >> second;
//    stream >> third;
//    stream >> fourth;

    //qDebug() << *(quint32*)n.data();
//    qDebug() << opCode << lenBytes << first << second << third << fourth;

    emit readyToProcess_00(buffer);
    isOpActive = false;
}
