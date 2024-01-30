#include "exchangedata.h"

#include <QDebug>

ExchangeData::ExchangeData(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExchangeData Constructor" << Qt::endl;
}

void ExchangeData::onReadyToProcess_00(QBuffer& buffer)
{
    qDebug() << "onReadyToProcess_00" << Qt::endl;

    buffer.seek(0);
    quint32 k = *(quint32*)(buffer.read(4)).data();

//    quint32 first;
//    quint32 second;
//    quint32 third;
//    quint32 fourth;

//    stream >> opCode >> lenBytes >> first >> second >> third >> fourth;

//    qDebug() << opCode << lenBytes << first << second << third << fourth;
}
