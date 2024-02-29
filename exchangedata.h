#ifndef EXCHANGEDATA_H
#define EXCHANGEDATA_H

#include <QObject>
#include <QMap>
#include <QBuffer>
#include <QDataStream>
#include <QTimer>

#include "device.h"
#include "channel.h"
#include "deviceproperties.h"
#include "operationsdata.h"

/* Класс для обмена данными между графикой и стендом */
class ExchangeData : public QObject
{
    Q_OBJECT
public:
    explicit ExchangeData(QObject *parent = nullptr);
    ~ExchangeData();

    bool recievingIsActive();
    void setRecievingState(bool state);

signals:
    void sendRequest_Op01(qint32 index, bool rx);
    void sendRequest_Op02(const QVector<ReqData_Op02>& requests);
    void updateCbDevices(QMap<QString, Device*>& devices);
    void updateCbChannels(QMap<QString, Channel*>& channels);
    void addReqToListWidget(const QString& reqText);
    void createRowsForWords(Words_t& words);
//    void updateTableExchange(const QVector<WordData>& words, quint16 start);
    void updateTableExchange(Words_t& words);
    void setRowEmpty(quint16 rowNumber);
    void restoreReqListWidget(const QVector<ReqData_Op02>& requests);

public slots:
    void onGetDevices_Op00(QBuffer& buffer);
    void onGetChannels_Op01(QBuffer& buffer);
    void onGetWords_Op03(QBuffer& buffer);
    void onCurrentDeviceChanged(const QString& name);
    void onCurrentChannelChanged(const QString& name);
    void onAddRequest_Op02(QString strLabels);
    void onDeleteRequest_Op02(QString reqText);
    void onApplyRequest_Op02();
    void onCheckRequestsToRestore();
//    void onStartTimer();

private slots:
    void onUpdateRowsTimerTimeout();
    void onMonitorWordsTimerTimeout();

private:
    QMap<QString, Device*> devices;     // Устройства
    Device* currentDevice;

    QVector<ReqData_Op02> requests_Op02;    // Запросы на получение меток
//    QVector<WordData> wordsList;            // Список всех приходящих слов
    QMap<quint8, WordData*> wordsByLabel;   // Только слова по запрашиваемым меткам
    Words_t words;

    const quint16 msecUpdateRowsTimeout = 150;
    const quint16 msecMonitorWordsTimeout = 5000;

    QTimer* updateRowsTimer;
    QTimer* monitorWordsTimer;
    bool isRecievingActive = false;

    void setSingleLabel(ReqData_Op02& data, qint32 labelNum);
    void setRangeOfLabels(ReqData_Op02& data, QStringList labels);
    void createWordsData(quint64 labelBits, quint8 dev, quint8 ch, quint8 rank);
    void deleteRequestedWords(quint64 labelBits, quint8 dev, quint8 ch, quint8 rank);
};

#endif // EXCHANGEDATA_H
