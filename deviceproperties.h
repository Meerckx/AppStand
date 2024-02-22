#ifndef DEVICEPROPERTIES_H
#define DEVICEPROPERTIES_H

#include <QDialog>
#include <QBuffer>

#include "device.h"
#include "channel.h"
#include "tcpclient.h"
#include "operationsdata.h"

namespace Ui {
class DeviceProperties;
}

class DeviceProperties : public QDialog
{
    Q_OBJECT

public:
    explicit DeviceProperties(QWidget *parent = nullptr);
    explicit DeviceProperties(TcpClient *client, QWidget *parent = nullptr);
    ~DeviceProperties();

signals:
    void currentDeviceChanged(const QString& name);
    void currentChannelChanged(const QString& name);
    void addRequest_Op02(QString strLabels);
    void deleteRequest_Op02(QString reqText);
    void applyRequest_Op02();

public slots:
    void onUpdateCbDevices(QMap<QString, Device*>& devices);
    void onUpdateCbChannels(QMap<QString, Channel*>& channels);
    void onAddReqToListWidget(const QString& reqText);
    void onRestoreReqListWidget(const QVector<ReqData_Op02>& requests);

private slots:
    void on_cbDevices_currentIndexChanged(const QString &name);
    void on_cbChannels_currentIndexChanged(const QString &name);
    void on_btnAddReq_clicked();
    void on_btnDeleteReq_clicked();
    void on_btnCancel_clicked();
    void on_btnApply_clicked();

private:
    Ui::DeviceProperties *ui;
    TcpClient *client;  // Возможно, не нужен
};

#endif // DEVICEPROPERTIES_H
