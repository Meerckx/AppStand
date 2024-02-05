#include "device.h"

#include <QDebug>

Device::Device(QObject *parent)
    : QObject{parent}
{

}

Device::Device(QString name, qint32 index, qint32 rxCount, qint32 txCount, QObject *parent)
    : QObject{parent}
{
    qDebug() << "Constructor Device " << name;
    devName = name;
    devIndex = index;
    devRxCount = rxCount;
    devTxCount = txCount;
}


QString Device::getName()
{
    return devName;
}

qint32 Device::getIndex()
{
    return devIndex;
}
