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

    /* Функции контроля состояния приёма слов */
    bool recievingIsActive();
    void setRecievingState(bool state);
    void stopTimers();

signals:
    void sendRequest_Op01(qint32 index, bool rx);
    void sendRequest_Op02(const QVector<ReqData_Op02>& requests);
    void updateCbDevices(QMap<QString, Device*>& devices);
    void updateCbChannels(QMap<QString, Channel*>& channels);
    void addReqToListWidget(const QString& reqText);
    void createRowsForWords(Words_t& words);
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
    void onSetMsecUpdateRowsTimer(quint16 time);

private slots:
    void onUpdateRowsTimerTimeout();
    void onMonitorWordsTimerTimeout();

private:
    QMap<QString, Device*> devices;         // Устройства
    Device* currentDevice;                  // Устройство, которое в данный момент выбрано в выпадающем списке

    QVector<ReqData_Op02> requests_Op02;    // Запросы на получение меток
    Words_t words;                          // Данные слов по устройству/каналу/метке

    quint16 msecUpdateRowsTimeout = 150;            // Интервал обновления строк в таблице tableExchange
    const quint16 msecMonitorWordsTimeout = 5000;   // Интервал контроля прихода слов

    QTimer* updateRowsTimer;                // Таймер обновления строк в таблице tableExchange
    QTimer* monitorWordsTimer;              // Таймер контроля прихода слов

    bool isRecievingActive = false;         // Флаг состояния приёма слов

    /* Вспомогательные функции для создания/удаления данных слов и связанных с ними запросов */
    void setSingleLabel(ReqData_Op02& data, qint32 labelNum);
    void setRangeOfLabels(ReqData_Op02& data, QStringList labels);
    void createWordsData(quint64 labelBits, quint8 dev, quint8 ch, quint8 rank);
    void deleteRequestedWords(quint64 labelBits, quint8 dev, quint8 ch, quint8 rank);
};

#endif // EXCHANGEDATA_H
