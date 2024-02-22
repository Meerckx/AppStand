#ifndef OPERATIONSDATA_H
#define OPERATIONSDATA_H

#include <QObject>
#include <QDateTime>
#include <QDebug>
#include "device.h"
#include "channel.h"

/* Код операции */
enum class OpType: quint32
{
    OP_00 = 0,
    OP_01,
    OP_02,
    OP_03,
    OP_04
};

/* Размер данных операции (байты) */
enum class OpDataSize : quint32
{
    RECIEVE_OP_00 = 48,
    SEND_OP_00 = 0,
    RECIEVE_OP_01 = 40,
    SEND_OP_01 = 8,
    RECIEVE_OP_02 = 0,
    SEND_OP_02 = 40,
    RECIEVE_OP_03 = 24,
};

/* Тип кодировки */
enum class EncodingType : quint8
{
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16
};

/* Метки */
struct ReqData_Op02
{
    Device *device = nullptr;
    QString labels = "";
    quint64 label_0_63 = 0;
    quint64 label_64_127 = 0;
    quint64 label_128_191 = 0;
    quint64 label_192_255 = 0;
    QString fullReqText = "";
};

/* Данные слова */
struct WordData
{
    static quint16 count;
    qint32 devIdx = 0;
    qint32 chIdx = 0;
    quint64 time = 0;
    quint64 prevTime = 0;
    quint64 delta = 0;
    quint8 address = 0;
    quint8 matrix = 0;
    quint32 word = 0;
    EncodingType encoding;
    bool isUpdated = false;

    WordData(quint8 devIdx, quint8 chIdx, quint8 label)
    {
        count++;
        this->devIdx = devIdx;
        this->chIdx = chIdx;
        this->address = label;
        qDebug() << "Word: " << devIdx << chIdx << label;
    }

    void setData(quint64 _time, quint32 _word)
    {
        if (time == 0)
        {
            prevTime = _time;
        }
        else
        {
            prevTime = time;
        }
        time = _time;
        delta = time - prevTime;
        matrix = quint8((_word << 29) & 0x3);
        word = _word;
        encoding = EncodingType::BIN;
        isUpdated = true; // Обнулять после каждого вывода на экран
    }
};


typedef QMap<quint8, QMap<quint8, QMap<quint8, WordData*>>> Words_t;

#endif // OPERATIONSDATA_H
