#ifndef OPERATIONSDATA_H
#define OPERATIONSDATA_H

#include <QObject>
#include "device.h"
#include "channel.h"

enum class OpDataSize : quint32    // В байтах
{
    RECIEVE_OP_00 = 44,
    SEND_OP_00 = 0,
    RECIEVE_OP_01 = 37,
    SEND_OP_01 = 6,
    RECIEVE_OP_02 = 20,
    SEND_OP_02 = 40,
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

#endif // OPERATIONSDATA_H
