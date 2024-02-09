#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , exchangeData(new ExchangeData(this))
    , client(new TcpClient(this))
{
    ui->setupUi(this);
    move(screen()->geometry().center() - frameGeometry().center());
    ui->tableExchange->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "MainWindow Constructor" << Qt::endl;

    connect(this, &MainWindow::connectToHost, client, &TcpClient::onConnectToHost);
    connect(client, &TcpClient::getDevices_Op00, exchangeData, &ExchangeData::onGetDevices_Op00);
    connect(client, &TcpClient::getChannels_Op01, exchangeData, &ExchangeData::onGetChannels_Op01);
    connect(exchangeData, &ExchangeData::sendRequest_Op01, client, &TcpClient::onSendRequest_Op01);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnDevProps_clicked()
{
    DeviceProperties *devProps = new DeviceProperties(client);
    devProps->show();   // удаляет объект при закрытии, поэтому connect нужно делать тут

    connect(exchangeData, &ExchangeData::updateCbDevices, devProps, &DeviceProperties::onUpdateCbDevices);
    connect(exchangeData, &ExchangeData::updateCbChannels, devProps, &DeviceProperties::onUpdateCbChannels);
    connect(exchangeData, &ExchangeData::addReqToListWidget, devProps, &DeviceProperties::onAddReqToListWidget);
    connect(devProps, &DeviceProperties::currentDeviceChanged, exchangeData, &ExchangeData::onCurrentDeviceChanged);
    connect(devProps, &DeviceProperties::currentChannelChanged, exchangeData, &ExchangeData::onCurrentChannelChanged);
    connect(devProps, &DeviceProperties::addRequest_Op02, exchangeData, &ExchangeData::onAddRequest_Op02);

    emit connectToHost();
}

