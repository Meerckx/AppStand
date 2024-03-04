#include "exchangedata.h"

#include <QDebug>
#include <bitset>
#include <iostream>

ExchangeData::ExchangeData(QObject *parent)
    : QObject(parent)
{
    currentDevice = nullptr;

    updateRowsTimer = new QTimer(this);
    connect(updateRowsTimer, &QTimer::timeout, this, &ExchangeData::onUpdateRowsTimerTimeout);

    monitorWordsTimer = new QTimer(this);
    connect(monitorWordsTimer, &QTimer::timeout, this, &ExchangeData::onMonitorWordsTimerTimeout);
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

    qDeleteAll(devices);
    devices.clear();

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

    updateRowsTimer->stop();
    delete updateRowsTimer;
}


/* SLOTS */
void ExchangeData::onGetDevices_Op00(QBuffer& buffer)
{
    if (devices.size() != 0)
    {
        qDeleteAll(devices);
        devices.clear();
    }

    /* Записываем данные из буфера */
    buffer.seek(0);
    quint16 devNumber = buffer.size() / sizeof(Receive_Op00);
    for (quint16 i = 0; i < devNumber; i++)
    {
        Receive_Op00 data = *(Receive_Op00*)(buffer.read(sizeof(Receive_Op00)).data());

        Device *dev = new Device(data.name, data.index, data.rxCount, data.txCount, this);
        devices.insert(data.name, dev);
    }

    emit updateCbDevices(devices);
}

void ExchangeData::onGetChannels_Op01(QBuffer& buffer)
{
    currentDevice->clearChannels();

    /* Записываем данные из буфера */
    buffer.seek(0);
    quint16 chNumber = buffer.size() / sizeof(Receive_Op01);
    for (quint16 i = 0; i < chNumber; i++)
    {
        Receive_Op01 data = *(Receive_Op01*)(buffer.read(sizeof(Receive_Op01)).data());
        currentDevice->addChannel(data.name, data.index, data.rx);
    }

    emit updateCbChannels(currentDevice->getChannels());
}

void ExchangeData::onGetWords_Op03(QBuffer& buffer)
{

    /* Записываем данные из буфера */
    buffer.seek(0);
    quint16 wordsNumber = buffer.size() / sizeof(Receive_Op03);
    for (quint16 i = 0; i < wordsNumber; i++)
    {
        Receive_Op03 data = *(Receive_Op03*)(buffer.read(sizeof(Receive_Op03)).data());

        quint8 label = quint8(data.word & 0xFF);
        words[data.devIdx][data.chIdx][label]->setData(data.time, data.word);
        //qDebug() << "onGetWords_Op03" << data.devIdx << data.chIdx << label;
    }
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

#ifdef DEBUG
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
#endif

void ExchangeData::onAddRequest_Op02(QString strLabels)
{
    /* Если ничего не ввели */
    if (strLabels.size() == 0)
    {
        qDebug() << "No data to add in requests_Op02";
        return;
    }

    /* Обработка запрашиваемых меток */
    ReqData_Op02 data;
    QStringList listLabels = strLabels.split(' ');
    for (qint16 i = 0; i < listLabels.size(); i++)
    {
        if (listLabels.at(i).contains('.'))
        {
            /* Если ввели диапазон меток, разделяем строку по символам ".." */
            this->setRangeOfLabels(data, listLabels.at(i).split(".."));
        }
        else
        {
            /* В ином случае сразу обрабатываем */
            this->setSingleLabel(data, listLabels.at(i).toInt(nullptr, 8));
        }
    }

    /* Если не добавили никаких меток */
    if (data.label_0_63 == 0 && data.label_64_127 == 0 && data.label_128_191 == 0 && data.label_192_255 == 0)
    {
        qDebug() << "No data to add in requests_Op02";
        return;
    }

    /* Добавляем данные в список запрсов */
    QString devName = currentDevice->getName() + "; ";
    QString chName = QString().setNum(currentDevice->getCurrentChannel()->getIndex()) + "; ";
    QString reqLabels = "Метки: \"" + strLabels + "\"";
    data.fullReqText.append(devName + chName + reqLabels);
    data.labels.append(strLabels);
    data.device = new Device(currentDevice, this);
    requests_Op02.append(data);

    // TODO: Стоит оптимизировать запросы в случае пересечения запрашиваемых меток

#ifdef DEBUG
    if (true)   // Debug
    {
        testRequest_Op02(requests_Op02);
    }
#endif

    /* Добавляем данные в виджет */
    emit addReqToListWidget(data.fullReqText);
}

void ExchangeData::onDeleteRequest_Op02(QString reqText)
{
    qDebug() << "Before delete: " << WordData::count;
    if (reqText.size() > 0)
    {
        /* Проходимся по всем запросам для поиска совпадения на удаление */
        for (qint16 i = 0; i < requests_Op02.size(); i++)
        {
            /* Если нашли, то удаляем запрос из списка */
            if (requests_Op02[i].fullReqText == reqText)
            {
                qDebug() << "DELETE: " << reqText;
                quint8 dev = (quint8)requests_Op02[i].device->getIndex();
                quint8 ch = (quint8)requests_Op02[i].device->getCurrentChannel()->getIndex();
                this->deleteRequestedWords(requests_Op02[i].label_0_63, dev, ch, 0);
                this->deleteRequestedWords(requests_Op02[i].label_64_127, dev, ch, 1);
                this->deleteRequestedWords(requests_Op02[i].label_128_191, dev, ch, 2);
                this->deleteRequestedWords(requests_Op02[i].label_192_255, dev, ch, 3);
                requests_Op02.remove(i);

                break;
            }
        }
    }
    qDebug() << "After delete: " << WordData::count;
}

void ExchangeData::onApplyRequest_Op02()
{
    if (requests_Op02.size() > 0)
    {
        /* Для каждого запроса создаём соответствующие объекты для данных слов */
        for (quint16 i = 0; i < requests_Op02.size(); i++)
        {
            quint8 dev = (quint8)requests_Op02[i].device->getIndex();
            quint8 ch = (quint8)requests_Op02[i].device->getCurrentChannel()->getIndex();
            this->createWordsData(requests_Op02[i].label_0_63, dev, ch, 0);
            this->createWordsData(requests_Op02[i].label_64_127, dev, ch, 1);
            this->createWordsData(requests_Op02[i].label_128_191, dev, ch, 2);
            this->createWordsData(requests_Op02[i].label_192_255, dev, ch, 3);
        }

        /* Генерируем сигналы на отправку запросов и создание строк под слова в таблице tableExchange */
        emit sendRequest_Op02(requests_Op02);
        emit createRowsForWords(words);

        /* Запускаем таймеры */
        updateRowsTimer->start(msecUpdateRowsTimeout);
        monitorWordsTimer->start(msecMonitorWordsTimeout);
    }
}

void ExchangeData::onCheckRequestsToRestore()
{
    if (requests_Op02.size() > 0)
    {
        /* Если при открытии окна "Настроить передачу" уже существуют запросы,
         * то генерируем сигнал на их восстановление в соответствующем списке */
        emit restoreReqListWidget(requests_Op02);
    }
}

void ExchangeData::onUpdateRowsTimerTimeout()
{
     emit updateTableExchange(words);
}

void ExchangeData::onSetMsecUpdateRowsTimer(quint16 time)
{
    /* Устанавливаем новый интервал обновления строк по запросу пользователя */
    msecUpdateRowsTimeout = time;
    if (updateRowsTimer->isActive())
    {
        updateRowsTimer->start(msecUpdateRowsTimeout);
    }
}

void ExchangeData::onMonitorWordsTimerTimeout()
{
    /* Если идёт приём слов, то раз в минуту проверяем, какие слова не обновлялись за этот промежуток,
     * и для них убирваем данные из соответствующих строк таблицы tableExchange */
    const quint16 msecMaxDelta = 60000;
    quint16 rowNumber = 0;
    for (auto device : words)
    {
        for (auto channel : device)
        {
            for (auto word : channel)
            {
                quint64 currentTime = QDateTime::currentMSecsSinceEpoch();
                if (currentTime - word->time > msecMaxDelta)
                {
                    emit setRowEmpty(rowNumber);
                }
                rowNumber++;
            }
        }
    }
}


/* PUBLIC FUNCTIONS */
void ExchangeData::stopTimers()
{
    updateRowsTimer->stop();
    monitorWordsTimer->stop();
}


/* PRIVATE FUNCTIONS */
void ExchangeData::setSingleLabel(ReqData_Op02& data, qint32 labelNum)
{
    if (labelNum < 0 || labelNum > 255)
    {
        qDebug() << "Wrong label number";
        return;
    }

    /* Определяем номер метки и затем устанавливаем соответствующий бит равным `1` */
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
    /* В подстроке могут быть только 2 элемента, т.к. диапазон вводится в формате "0..377" */
    if (listLabels.size() < 2)
    {
        qDebug() << "Wrong label range";
        return;
    }

    qint32 start = listLabels.at(0).toInt(nullptr, 8);
    qint32 end = listLabels.at(1).toInt(nullptr, 8);
    if (end < start)
    {
        qDebug() << "Wrong label range";
        return;
    }

    /* Обрабатываем каждую метку из заданного диапазона */
    for (qint32 labelNum = start; labelNum <= end; labelNum++)
    {
        this->setSingleLabel(data, labelNum);
    }
}

void ExchangeData::deleteRequestedWords(quint64 labelBits, quint8 dev, quint8 ch, quint8 rank)
{
    const quint8 bitNum = 64;
    for (quint8 bit = 0; bit < bitNum; bit++)
    {
        if (labelBits == 0)
        {
            return;
        }

        if ((labelBits & 0x1) == 1)
        {
            if (words[dev][ch].find(bit + bitNum * rank) != words[dev][ch].end())
            {
                delete words[dev][ch][bit + bitNum * rank];
                words[dev][ch].remove(bit + bitNum * rank);
            }
        }
        labelBits >>= 1;
    }
}

void ExchangeData::createWordsData(quint64 labelBits, quint8 dev, quint8 ch, quint8 rank)
{
    const quint8 bitNum = 64;
    for (quint8 bit = 0; bit < bitNum; bit++)
    {
        if (labelBits == 0)
        {
            return;
        }

        if ((labelBits & 0x1) == 1)
        {
            quint8 labelNum = bit + bitNum * rank;

            /* Проверяем, создавалось ли такое слово ранее*/
            bool exist = false;
            if (words.find(dev) != words.end())
            {
                if (words[dev].find(ch) != words[dev].end())
                {
                    if (words[dev][ch].find(labelNum) != words[dev][ch].end())
                    {
                        exist = true;
                    }
                }
            }

            if (!exist)
            {
                words[dev][ch][labelNum] = new WordData(dev, ch, labelNum);
            }
            else
            {
                qDebug() << "Already have this word in map";
            }
        }

        labelBits >>= 1;
    }
}
