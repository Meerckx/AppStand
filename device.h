#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QMap>

#include "channel.h"

/* Класс для описания подключенных устройств */
class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);

signals:

private:
    QString devName;                    // Имя устройства
    qint32 devIndex;                    // Индекс устройства
    qint32 devRxCount;                  // Количество каналов для получения данных
    qint32 devTxCount;                  // Количество каналов для передачи данных
    QMap<quint16, Channel*> channels;   // Каналы
};

#endif // DEVICE_H
