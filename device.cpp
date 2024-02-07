#include "device.h"

#include <QDebug>

Device::Device(QObject *parent)
    : QObject{parent}
{
    currentChannel = nullptr;
}

Device::Device(QString name, qint32 index, qint32 rxCount, qint32 txCount, QObject *parent)
    : Device(parent)
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

QMap<QString, Channel*>& Device::getChannels()
{
    return channels;
}

void Device::clearChannels()
{
    if (channels.size() != 0)
    {
        channels.clear();
    }
}

void Device::addChannel(QString name, qint32 index, bool rx)
{
    Channel *channel = new Channel(name, index, rx, this);
    channels.insert(name, channel);
}

void Device::setCurrentChannel(const QString& name)
{
    currentChannel = channels.value(name);
}
