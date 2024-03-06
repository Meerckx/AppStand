#include "deviceproperties.h"
#include "ui_deviceproperties.h"

#include <QDebug>

DeviceProperties::DeviceProperties(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceProperties)
{
    ui->setupUi(this);

    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    ui->leLabels->setFocus();

    ui->cbDevices->setInsertPolicy(ui->cbDevices->InsertAtBottom);
    ui->cbDevices->setDuplicatesEnabled(false);
    ui->cbChannels->setInsertPolicy(ui->cbDevices->InsertAtBottom);
    ui->cbChannels->setDuplicatesEnabled(false);
}

DeviceProperties::~DeviceProperties()
{
    delete ui;
}


/* SLOTS */
void DeviceProperties::onUpdateCbDevices(QMap<QString, Device*>& devices)
{
    for(auto dev : devices.toStdMap())
    {
        ui->cbDevices->addItem(dev.first);
    }
}

void DeviceProperties::onUpdateCbChannels(QMap<QString, Channel*>& channels)
{
    ui->cbChannels->clear();
    for(auto channel : channels.toStdMap())
    {
        ui->cbChannels->addItem(QString().setNum(channel.second->getIndex()));
    }
}

void DeviceProperties::onAddReqToListWidget(const QString& reqText)
{
    if (reqText.size() > 0)
    {
        ui->lwReqData->addItem(reqText);
    }
}

void DeviceProperties::onRestoreReqListWidget(const QVector<ReqData_Op02>& requests)
{
    for (quint16 i = 0; i < requests.size(); i++)
    {
        ui->lwReqData->addItem(requests[i].fullReqText);
    }
}

void DeviceProperties::on_cbDevices_currentIndexChanged(const QString& name)
{
    if (name.size() > 0)
    {
        emit currentDeviceChanged(name);
    }
}

void DeviceProperties::on_cbChannels_currentIndexChanged(const QString& name)
{
    if (name.size() > 0)
    {
        emit currentChannelChanged(name);
    }
}

void DeviceProperties::on_btnAddReq_clicked()
{
    this->addRequest();
}

void DeviceProperties::on_leLabels_returnPressed()
{
    /* Срабатывает при нажатии ENTER при вводе меток */
    this->addRequest();
}

void DeviceProperties::on_btnDeleteReq_clicked()
{
    if (ui->lwReqData->currentItem())
    {
        QString reqText = ui->lwReqData->currentItem()->text();
        delete ui->lwReqData->takeItem(ui->lwReqData->currentRow());
        ui->lwReqData->setCurrentItem(nullptr);
        emit deleteRequest_Op02(reqText);
    }
}

void DeviceProperties::on_btnApply_clicked()
{
    if (ui->lwReqData->count() > 0)
    {
        emit applyRequest_Op02();
    }
    this->close();
}

void DeviceProperties::on_btnCancel_clicked()
{
    this->close();
}


/* PRIVATE FUNCTIONS */
void DeviceProperties::addRequest()
{
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


