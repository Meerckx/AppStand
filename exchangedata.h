#ifndef EXCHANGEDATA_H
#define EXCHANGEDATA_H

#include <QObject>
#include <QMap>
#include <QBuffer>
#include <QDataStream>

#include "device.h"
#include "channel.h"
#include "deviceproperties.h"
#include "operationsdata.h"


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
    ~ExchangeData();

signals:
    void sendRequest_Op01(qint32 index, bool rx);
    void sendRequest_Op02(const QVector<ReqData_Op02>& requests);
    void updateCbDevices(QMap<QString, Device*>& devices);
    void updateCbChannels(QMap<QString, Channel*>& channels);
    void addReqToListWidget(const QString& reqText);

public slots:
    void onGetDevices_Op00(QBuffer& buffer);
    void onGetChannels_Op01(QBuffer& buffer);
    void onCurrentDeviceChanged(const QString& name);
    void onCurrentChannelChanged(const QString& name);
    void onAddRequest_Op02(QString strLabels);
    void onDeleteRequest_Op02(QString reqText);
    void onApplyRequest_Op02();

private:
    QMap<QString, Device*> devices;     // Устройства
    Device* currentDevice;

    QVector<ReqData_Op02> requests_Op02;  // Запросы на получение меток

    static void setSingleLabel(ReqData_Op02& data, qint32 labelNum);
    static void setRangeOfLabels(ReqData_Op02& data, QStringList labels);

};

#endif // EXCHANGEDATA_H
