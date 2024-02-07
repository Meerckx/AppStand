#ifndef EXCHANGEDATA_H
#define EXCHANGEDATA_H

#include <QObject>
#include <QMap>
#include <QBuffer>
#include <QDataStream>

#include "device.h"
#include "channel.h"
#include "deviceproperties.h"

enum class OpRecieveSize : quint16    // В байтах
{
    OP_00 = 44,
    OP_01 = 37,
};

/* Метки */
typedef struct Labels
{
    quint64 label_0_63;
    quint64 label_64_127;
    quint64 label_128_191;
    quint64 label_192_255;
} Labels_t;

/* Данные слова */
typedef struct WordData
{
    quint32 word;           // Само слово
    quint64 timeRxSec;      // Время получения слова в секундах
    quint64 timeRxMcSec;    // Время получения слова в микросекундах
} WordData_t;


/* Класс для обмена данными между графикой и стендом */
class ExchangeData : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeData(QObject *parent = nullptr);

signals:
    void updateCbDevices(QMap<QString, Device*>& devices);
    void updateCbChannels(QMap<QString, Channel*>& channels);
    void sendRequest_Op01(qint32 index, bool rx);

public slots:
    void onGetDevices_Op00(QBuffer& buffer);
    void onGetChannels_Op01(QBuffer& buffer);
    void onCurrentDeviceChanged(const QString& name);
    void onCurrentChannelChanged(const QString& name);

private:
    quint16 opCode;                     // Код операции при общении со стендом
    quint32 lenBytes;                   // Длина передаваемых данных в байтах
    QMap<QString, Device*> devices;     // Устройства

    Device* currentDevice;;
};

#endif // EXCHANGEDATA_H
