#include "exchangedata.h"

#include <QDebug>
#include <bitset>
#include <iostream>

ExchangeData::ExchangeData(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExchangeData Constructor" << Qt::endl;
    currentDevice = nullptr;
}

ExchangeData::~ExchangeData()
{
    for (int i = 0; i < requests_Op02.size(); i++)
    {
        delete requests_Op02[i].device;
    }
}

void ExchangeData::onGetDevices_Op00(QBuffer& buffer)
{
    qDebug() << "onGetDevices_Op00" << Qt::endl;

    if (devices.size() != 0)
    {
        devices.clear();
    }
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

void ExchangeData::setSingleLabel(ReqData_Op02& data, qint32 labelNum)
{
    if (labelNum < 0 || labelNum > 255)
    {
        qDebug() << "Wrong label number";
        return;
    }

    quint64 *num = nullptr;
    qint32 shift = labelNum % 64;
    if (labelNum >= 0 && labelNum <= 63)
    {
        num = &data.label_0_63;
    }
    else if (labelNum >= 64 && labelNum <= 127)
    {
        num = &data.label_64_127;
    }
    else if (labelNum >= 128 && labelNum <= 191)
    {
        num = &data.label_128_191;
    }
    else if (labelNum >= 192 && labelNum <= 255)
    {
        num = &data.label_192_255;
    }

    (*num) |= ((quint64)0x1 << shift);
}

void ExchangeData::setRangeOfLabels(ReqData_Op02& data, QStringList listLabels)
{
    if (listLabels.size() < 2)
    {
        qDebug() << "Wrong label range";
        return;
    }

    qint32 start = listLabels.at(0).toInt();
    qint32 end = listLabels.at(1).toInt();
    if (end < start)
    {
        qDebug() << "Wrong label range";
        return;
    }

    for (qint32 labelNum = start; labelNum <= end; labelNum++)
    {
        setSingleLabel(data, labelNum);
    }
}

void testRequest_Op02(QVector<ReqData_Op02>& req)
{
    for (int i = 0; i < req.size(); i++)
    {
        qDebug() << req[i].device->getName();

        std::bitset<64> num1(req[i].label_0_63);
        std::bitset<64> num2(req[i].label_64_127);
        std::bitset<64> num3(req[i].label_128_191);
        std::bitset<64> num4(req[i].label_192_255);
        std::cout << num1 << std::endl;
        std::cout << num2 << std::endl;
        std::cout << num3 << std::endl;
        std::cout << num4 << std::endl;
    }
}

void ExchangeData::onAddRequest_Op02(QString strLabels)
{
    qDebug() << "onAddRequest_Op02" << Qt::endl;
    ReqData_Op02 data;
    //memset((void*)(&data), 0, sizeof(ReqData_Op02));

    QStringList listLabels = strLabels.split(' ');
    for (qint16 i = 0; i < listLabels.size(); i++)
    {
        if (listLabels.at(i).contains('.'))
        {
            setRangeOfLabels(data, listLabels.at(i).split(".."));
        }
        else
        {
            setSingleLabel(data, listLabels.at(i).toInt());
        }
    }
    qDebug() << "1";
    if (data.label_0_63 == 0 && data.label_64_127 == 0 && data.label_128_191 == 0 && data.label_192_255 == 0)
    {
        qDebug() << "No data to add in requests_Op02";
        return;
    }
    qDebug() << "2";
    data.device = new Device(currentDevice, this);
    qDebug() << "3";
    data.labels.append(strLabels);
    qDebug() << "4";
    requests_Op02.append(data);

    if (true)   // Debug
    {
        testRequest_Op02(requests_Op02);
    }

    emit addReqToListWidget(requests_Op02);
}
