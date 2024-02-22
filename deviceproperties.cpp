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

void DeviceProperties::onAddReqToListWidget(const QString& reqText)
{
    qDebug() << "onAddReqToListWidget" << Qt::endl;
    if (reqText.size() > 0)
    {
        ui->lwReqData->addItem(reqText);
    }
}


void DeviceProperties::onRestoreReqListWidget(const QVector<ReqData_Op02>& requests)
{
    qDebug() << "onRestoreReqListWidget" << Qt::endl;

    for (quint16 i = 0; i < requests.size(); i++)
    {
        ui->lwReqData->addItem(requests[i].fullReqText);
    }
}


void DeviceProperties::on_cbDevices_currentIndexChanged(const QString& name)
{
    qDebug() << "on_cbDevices_currentIndexChanged" << Qt::endl;
    if (name.size() > 0)
    {
        emit currentDeviceChanged(name);
    }
}


void DeviceProperties::on_cbChannels_currentIndexChanged(const QString& name)
{
    qDebug() << "on_cbDevices_currentIndexChanged" << Qt::endl;
    if (name.size() > 0)
    {
        emit currentChannelChanged(name);
    }
}


void DeviceProperties::on_btnAddReq_clicked()
{
    qDebug() << "on_btnAddReq_clicked" << Qt::endl;

    if (ui->leLabels->text().size() != 0 && ui->cbDevices->count() != 0 && ui->cbChannels->count() != 0)
    {
        QString labels = ui->leLabels->text();
        ui->leLabels->clear();
        emit addRequest_Op02(labels);
    }
    else
    {
        qDebug() << "on_btnAddReq_clicked: NO REQUESTED DATA";
    }
}


void DeviceProperties::on_btnDeleteReq_clicked()
{
    qDebug() << "on_btnDeleteReq_clicked" << Qt::endl;

    if (ui->lwReqData->currentItem())
    {
        qDebug() << "item selected";
        QString reqText = ui->lwReqData->currentItem()->text();
        delete ui->lwReqData->takeItem(ui->lwReqData->currentRow());
        ui->lwReqData->setCurrentItem(nullptr);
        emit deleteRequest_Op02(reqText);
    }
}


void DeviceProperties::on_btnApply_clicked()
{
    qDebug() << "on_btnApply_clicked" << Qt::endl;

    if (ui->lwReqData->count() > 0)
    {
        emit applyRequest_Op02();
    }
    this->close();
}


// При повторном открытии окна должнен сохраняться QListWidget
void DeviceProperties::on_btnCancel_clicked()
{
    qDebug() << "on_btnCancel_clicked" << Qt::endl;
    this->close();
}



