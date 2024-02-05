#include "exchangedata.h"

#include <QDebug>

ExchangeData::ExchangeData(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExchangeData Constructor" << Qt::endl;
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

void ExchangeData::onGetChannels_Op01(QBuffer& buffer)  // перенести в deviceproperties
{

}

void ExchangeData::onDevChannelsRequested(const QString& name)
{
    qDebug() << "onDevChannelsRequested: " << name << Qt::endl;
    Device *device = devices.value(name);
    emit sendRequest_Op01(device->getIndex(), true);
}
