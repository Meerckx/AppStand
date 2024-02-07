#include "deviceproperties.h"
#include "ui_deviceproperties.h"

#include <QDebug>

DeviceProperties::DeviceProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceProperties)
{
    qDebug() << "DeviceProperties constructor" << Qt::endl;
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->cbDevices->setInsertPolicy(ui->cbDevices->InsertAtBottom);
    ui->cbDevices->setDuplicatesEnabled(false);
}

DeviceProperties::DeviceProperties(TcpClient *client, QWidget *parent)
    : DeviceProperties(parent)
{
    this->client = client;
}

DeviceProperties::~DeviceProperties()
{
    delete ui;
}

void DeviceProperties::onUpdateCbDevices(QMap<QString, Device*>& devices)
{
    qDebug() << "onUpdateCbDevices" << Qt::endl;

    for(auto dev : devices.toStdMap())
    {
        ui->cbDevices->addItem(dev.first);
    }
}

void DeviceProperties::onUpdateCbChannels(QMap<QString, Channel*>& channels)
{
    qDebug() << "onUpdateCbChannels" << Qt::endl;
    ui->cbChannels->clear();
    for(auto channel : channels.toStdMap())
    {
        ui->cbChannels->addItem(channel.first);
    }
}

void DeviceProperties::on_cbDevices_currentIndexChanged(const QString& name)
{
    qDebug() << "on_cbDevices_currentIndexChanged" << Qt::endl;
    if (name.size() != 0)
    {
        emit currentDeviceChanged(name);
    }
}


void DeviceProperties::on_cbChannels_currentIndexChanged(const QString& name)
{
    qDebug() << "on_cbDevices_currentIndexChanged" << Qt::endl;
    if (name.size() != 0)
    {
        emit currentChannelChanged(name);
    }
}

