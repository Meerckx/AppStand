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

QString DeviceProperties::getCurrentDeviceName()
{
    return ui->cbDevices->currentText();
}

void DeviceProperties::onUpdateCbDevices(QMap<QString, Device*>& devices)
{
    qDebug() << "onUpdateCbDevices" << Qt::endl;

    for(auto dev : devices.toStdMap())
    {
        ui->cbDevices->addItem(dev.first);
    }
}

void DeviceProperties::on_cbDevices_currentIndexChanged(const QString &arg1)    // Почему-то вручную слот создаваться не хотел, поэтому используется обёртка
{
    qDebug() << "on_cbDevices_currentIndexChanged" << Qt::endl;
    emit devChannelsRequested(arg1);
}

