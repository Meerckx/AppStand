#include "exchangedata.h"

#include <QDebug>

ExchangeData::ExchangeData(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExchangeData Constructor" << Qt::endl;
    currentDevice = nullptr;
}


void ExchangeData::onGetDevices_Op00(QBuffer& buffer)
{
    qDebug() << "onGetDevices_Op00" << Qt::endl;

    buffer.seek(0);
    quint16 devNumber = buffer.size() / (qint64)OpRecieveSize::OP_00;
    qDebug() << "devNumber = " << devNumber;
    for (quint16 i = 0; i < devNumber; i++)
    {
        QString name = (buffer.read(32)).data();
        qint32 index = *(qint32*)(buffer.read(4)).data();
        qint32 rxCount = *(qint32*)(buffer.read(4)).data();
        qint32 txCount = *(qint32*)(buffer.read(4)).data();
        qDebug() << name.size() << name << index << rxCount << txCount;

        Device *dev = new Device(name, index, rxCount, txCount, this);
        devices.insert(name, dev);
    }

    emit updateCbDevices(devices);
}

void ExchangeData::onGetChannels_Op01(QBuffer& buffer)
{
    qDebug() << "onGetChannels_Op01" << Qt::endl;

    currentDevice->clearChannels();
    buffer.seek(0);
    quint16 chNumber = buffer.size() / (qint64)OpRecieveSize::OP_01;
    qDebug() << "chNumber = " << chNumber;
    for (quint16 i = 0; i < chNumber; i++)
    {
        QString name = (buffer.read(32)).data();
        qint32 index = *(qint32*)(buffer.read(4)).data();
        bool rx = *(bool*)(buffer.read(1)).data();
        currentDevice->addChannel(name, index, rx);
    }

    emit updateCbChannels(currentDevice->getChannels());
}

void ExchangeData::onCurrentDeviceChanged(const QString& name)
{
    qDebug() << "onCurrentDeviceChanged: " << name << Qt::endl;
    currentDevice = devices.value(name);
    emit sendRequest_Op01(currentDevice->getIndex(), true);
}

void ExchangeData::onCurrentChannelChanged(const QString& name)
{
    qDebug() << "onCurrentChannelChanged: " << name << Qt::endl;
    currentDevice->setCurrentChannel(name);
}
