#include "channel.h"

#include <QDebug>

Channel::Channel(QObject *parent)
    : QObject{parent}
{

}

Channel::Channel(QString name, qint32 index, bool rx, QObject *parent)
    : QObject{parent}
{
    qDebug() << "Constructor Channel " << name << index << rx;
    chName = name;
    chIndex = index;
    isRxChannel = rx;
}
