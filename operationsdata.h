#ifndef OPERATIONSDATA_H
#define OPERATIONSDATA_H

#include <QObject>
#include <QDateTime>
#include "device.h"
#include "channel.h"

/* Код операции */
enum class OpType: quint32
{
    OP_00 = 0x00,
    OP_01,
    OP_02,
    OP_03,
    OP_04
};

/* Размер данных операции (байты) */
enum class OpDataSize : quint32
{
    RECIEVE_OP_00 = 44,
    SEND_OP_00 = 0,
    RECIEVE_OP_01 = 37,
    SEND_OP_01 = 6,
    RECIEVE_OP_02 = 0,
    SEND_OP_02 = 40,
    RECIEVE_OP_03 = 20,
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
    qint32 devIdx = 0;
    qint32 chIdx = 0;
    QDateTime time;
    QDateTime prevTime;
    QTime delta;
    quint8 address = 0;
    quint8 matrix = 0;
    quint32 word = 0;
    EncodingType encoding;
    bool isUpdated = false;

    WordData() {  }

    WordData(qint32 devIdx, qint32 chIdx, quint64 time, quint32 word)
    {
        this->devIdx = devIdx;
        this->chIdx = chIdx;
        if (this->time.isNull())
        {
            this->prevTime = QDateTime::fromMSecsSinceEpoch(qint64(time));
        }
        else
        {
            this->prevTime = this->time;
        }
        this->time = QDateTime::fromMSecsSinceEpoch(qint64(time));
        this->delta = this->delta.addMSecs(this->time.currentMSecsSinceEpoch() - this->prevTime.currentMSecsSinceEpoch());
        this->address = quint8(word & 0xFF);
        this->matrix = quint8((word << 29) & 0x3);
        this->word = word;
        this->encoding = EncodingType::BIN;
        this->isUpdated = true;
    }

    void setData(qint32 devIdx, qint32 chIdx, quint64 time, quint32 word)
    {
        this->devIdx = devIdx;
        this->chIdx = chIdx;
        if (this->time.isNull())
        {
            this->prevTime = QDateTime::fromMSecsSinceEpoch(qint64(time));
        }
        else
        {
            this->prevTime = this->time;
        }
        this->time = QDateTime::fromMSecsSinceEpoch(qint64(time));
        this->delta = this->delta.addMSecs(this->time.currentMSecsSinceEpoch() - this->prevTime.currentMSecsSinceEpoch());
        this->address = quint8(word & 0xFF);
        this->matrix = quint8((word << 29) & 0x3);
        this->word = word;
        this->encoding = EncodingType::BIN;
        this->isUpdated = true; // Обнулять после каждого вывода на экран
    }
};


#endif // OPERATIONSDATA_H
