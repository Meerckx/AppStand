#ifndef OPERATIONSDATA_H
#define OPERATIONSDATA_H

#include <QObject>
#include "device.h"
#include "channel.h"

enum class OpRecieveSize : quint16    // В байтах
{
    OP_00 = 44,
    OP_01 = 37,
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
};

#endif // OPERATIONSDATA_H
