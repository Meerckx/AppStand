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
    devName = name;
    devIndex = index;
    devRxCount = rxCount;
    devTxCount = txCount;
}

Device::Device(const Device* device, QObject *parent)
    : QObject{parent}
{
    devName = device->devName;
    devIndex = device->devIndex;
    devRxCount = device->devRxCount;
    devTxCount = device->devTxCount;

    currentChannel = new Channel(device->currentChannel, this);
}

Device::Device(const Device& device, QObject *parent)   // как-будто бы конструктор копирования, но нет
    : QObject{parent}
{
    devName = device.devName;
    devIndex = device.devIndex;
    devRxCount = device.devRxCount;
    devTxCount = device.devTxCount;

    currentChannel = device.currentChannel;
}

Device::~Device()
{
    delete currentChannel;
    currentChannel = nullptr;
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

Channel* Device::getCurrentChannel()
{
    return currentChannel;
}

void Device::clearChannels()
{
    if (channels.size() != 0)
    {
        qDeleteAll(channels);
        channels.clear();
    }
}

void Device::addChannel(QString name, qint32 index, bool rx)
{
    Channel *channel = new Channel(name, index, rx, this);
    channels.insert(QString().setNum(index), channel);
}

void Device::setCurrentChannel(const QString& name)
{
    currentChannel = channels.value(name);
}
