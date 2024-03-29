#include "channel.h"

#include <QDebug>

Channel::Channel(QObject *parent)
    : QObject{parent}
{

}

Channel::Channel(QString name, qint32 index, bool rx, QObject *parent)
    : QObject{parent}
{
    chName = name;
    chIndex = index;
    isRxChannel = rx;
}

Channel::Channel(const Channel* channel, QObject *parent)
    : QObject{parent}
{
    chName = channel->chName;
    chIndex = channel->chIndex;
    isRxChannel = channel->isRxChannel;
}

QString Channel::getName()
{
    return chName;
}

qint32 Channel::getIndex()
{
    return chIndex;
}
