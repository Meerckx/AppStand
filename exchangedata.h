#ifndef EXCHANGEDATA_H
#define EXCHANGEDATA_H

#include <QObject>
#include <QMap>
#include <QBuffer>
#include <QDataStream>

#include "device.h"

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

public slots:
    void onReadyToProcess_00(QBuffer& buffer);

private:
    quint16 opCode;                     // Код операции при общении со стендом
    quint32 lenBytes;                   // Длина передаваемых данных в байтах
    QMap<quint16, Device*> devices;     // Устройства

};

#endif // EXCHANGEDATA_H
