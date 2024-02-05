#ifndef DEVICEPROPERTIES_H
#define DEVICEPROPERTIES_H

#include <QDialog>
#include <QBuffer>

#include "device.h"
#include "tcpclient.h"

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

    QString getCurrentDeviceName();

signals:
    void devChannelsRequested(const QString& name);

public slots:
    void onUpdateCbDevices(QMap<QString, Device*>& devices);

private slots:
    void on_cbDevices_currentIndexChanged(const QString &arg1);

private:
    Ui::DeviceProperties *ui;
    TcpClient *client;  // Возможно, не нужен
};

#endif // DEVICEPROPERTIES_H
