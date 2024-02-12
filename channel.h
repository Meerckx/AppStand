#ifndef CHANNEL_H
#define CHANNEL_H

#include <QObject>

/* Класс для описания каналов */
class Channel : public QObject
{
    Q_OBJECT
public:
    explicit Channel(QObject *parent = nullptr);
    explicit Channel(QString name, qint32 index, bool rx, QObject *parent = nullptr);
    explicit Channel(const Channel* channel, QObject *parent = nullptr);

    QString getName();
    qint32 getIndex();

signals:

private:
    QString chName;     // Имя канала
    qint32 chIndex;     // Индекс канала
    bool isRxChannel;   // Флаг типа канала
};

#endif // CHANNEL_H
