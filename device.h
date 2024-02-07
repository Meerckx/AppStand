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
    explicit Device(QString name, qint32 index, qint32 rxCount, qint32 txCount, QObject *parent = nullptr);

    QString getName();
    qint32 getIndex();
    QMap<QString, Channel*>& getChannels();

    void clearChannels();
    void addChannel(QString name, qint32 index, bool rx);
    void setCurrentChannel(const QString& name);

signals:

public slots:

private:
    QString devName;                    // Имя устройства
    qint32 devIndex;                    // Индекс устройства
    qint32 devRxCount;                  // Количество каналов для получения данных
    qint32 devTxCount;                  // Количество каналов для передачи данных
    QMap<QString, Channel*> channels;   // Каналы

    Channel* currentChannel;
};

#endif // DEVICE_H
