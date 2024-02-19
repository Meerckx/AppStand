#include "exchangedata.h"

#include <QDebug>
#include <bitset>
#include <iostream>

ExchangeData::ExchangeData(QObject *parent)
    : QObject(parent)
{
    qDebug() << "ExchangeData Constructor" << Qt::endl;
    currentDevice = nullptr;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ExchangeData::onTimerTimeout);
}


ExchangeData::~ExchangeData()
{
    for (int i = 0; i < requests_Op02.size(); i++)
    {
        delete requests_Op02[i].device;
        requests_Op02[i].device = nullptr;
    }
    requests_Op02.clear();
    requests_Op02.squeeze();

//    wordsList.clear();
//    wordsList.squeeze();

    qDeleteAll(devices);
    devices.clear();

    qDeleteAll(wordsByLabel);
    wordsByLabel.clear();

    for (auto devCh : words)
    {
        for (auto chLabel : devCh)
        {
            qDeleteAll(chLabel);
            chLabel.clear();
        }
        devCh.clear();
    }
    words.clear();

    timer->stop();
    delete timer;
}


void ExchangeData::onGetDevices_Op00(QBuffer& buffer)
{
    qDebug() << "onGetDevices_Op00" << Qt::endl;

    if (devices.size() != 0)
    {
        qDeleteAll(devices);
        devices.clear();
    }
    buffer.seek(0);
    quint16 devNumber = buffer.size() / (qint64)OpDataSize::RECIEVE_OP_00;
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
    quint16 chNumber = buffer.size() / (qint64)OpDataSize::RECIEVE_OP_01;
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

void ExchangeData::onGetWords_Op03(QBuffer& buffer)
{
    qDebug() << "onGetWords_Op03" << Qt::endl;

    buffer.seek(0);

    //quint16 startIndex = wordsList.size();     // Индекс, с которого будут начинаться новые записи
    quint16 wordsNumber = buffer.size() / (qint64)OpDataSize::RECIEVE_OP_03;
    qDebug() << "wordsNumber = " << wordsNumber;
    for (quint16 i = 0; i < wordsNumber; i++)
    {
        qint32 devIdx = *(qint32*)(buffer.read(4)).data();
        qint32 chIdx = *(qint32*)(buffer.read(4)).data();
        quint64 time = *(quint64*)(buffer.read(8)).data();
        quint32 word = *(quint32*)(buffer.read(4)).data();

        qDebug() << devIdx << chIdx << time << word;
        quint8 label = quint8(word & 0xFF);
        words[devIdx][chIdx][label]->setData(time, word);
        emit updateTableExchange(words);
//        wordsByLabel.value(label)->setData(devIdx, chIdx, time, word);
        // Копим данные, пока не кончится время

        // Надо завести таймер, по исходу времени обновлять таблицу и обнулять поле isUpdated

//        WordData data(devIdx, chIdx, time, word);
//        if (startIndex > 0)
//        {
//            QDateTime prevTime = wordsList[wordsList.size() - 1].time;
//            data.delta = data.delta.addMSecs(data.time.currentMSecsSinceEpoch() - prevTime.currentMSecsSinceEpoch());
//            qDebug() << "delta = " << data.delta;
//        }
//        wordsList.append(data);
    }

//    emit updateTableExchange(words);
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

    QStringList listLabels = strLabels.split(' ');
    for (qint16 i = 0; i < listLabels.size(); i++)
    {
        if (listLabels.at(i).contains('.'))
        {
            this->setRangeOfLabels(data, listLabels.at(i).split(".."));
        }
        else
        {
            this->setSingleLabel(data, listLabels.at(i).toInt());
        }
    }

    if (data.label_0_63 == 0 && data.label_64_127 == 0 && data.label_128_191 == 0 && data.label_192_255 == 0)
    {
        qDebug() << "No data to add in requests_Op02";
        return;
    }

    QString devName = currentDevice->getName() + "; ";
    QString chName = currentDevice->getCurrentChannel()->getName() + "; ";
    QString reqLabels = "Метки: \"" + strLabels + "\"";
    data.fullReqText.append(devName + chName + reqLabels);
    data.labels.append(strLabels);
    data.device = new Device(currentDevice, this);

    requests_Op02.append(data);

    if (true)   // Debug
    {
        testRequest_Op02(requests_Op02);
    }

    emit addReqToListWidget(data.fullReqText);
}


void ExchangeData::onDeleteRequest_Op02(QString reqText)
{
    qDebug() << "onDeleteRequest_Op02" << Qt::endl;

    if (reqText.size() > 0)
    {
        for (qint16 i = 0; i < requests_Op02.size(); i++)
        {
            if (requests_Op02[i].fullReqText == reqText)
            {
                qDebug() << "DELETE: " << reqText;
                requests_Op02.remove(i);
            }
        }
    }
}


void ExchangeData::onApplyRequest_Op02()
{
    qDebug() << "onApplyRequest_Op02" << Qt::endl;

    if (requests_Op02.size() > 0)
    {
        emit sendRequest_Op02(requests_Op02);

        emit createRowsForWords(words);

        //timer->start(2000); // Каждую секунду срабатывает (поменять)
    }
}


void ExchangeData::onTimerTimeout()
{
    qDebug() << "onTimerTimeout" << Qt::endl;
    emit updateTableExchange(words);
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

    quint8 devIdx = (quint8)currentDevice->getIndex();
    quint8 chIdx = (quint8)currentDevice->getCurrentChannel()->getIndex();
    words[devIdx][chIdx][labelNum] = new WordData(devIdx, chIdx, labelNum);

    //    wordsByLabel.insert((quint8)labelNum, new WordData());
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
        this->setSingleLabel(data, labelNum);
    }
}
