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

    colsCount = ui->tableExchange->columnCount();

    qDebug() << "MainWindow Constructor" << Qt::endl;

    connect(this, &MainWindow::connectToHost, client, &TcpClient::onConnectToHost);
    connect(exchangeData, &ExchangeData::updateTableExchange, this, &MainWindow::onUpdateTableExchange);
    connect(client, &TcpClient::getDevices_Op00, exchangeData, &ExchangeData::onGetDevices_Op00);
    connect(client, &TcpClient::getChannels_Op01, exchangeData, &ExchangeData::onGetChannels_Op01);
    connect(exchangeData, &ExchangeData::sendRequest_Op01, client, &TcpClient::onSendRequest_Op01);
    connect(exchangeData, &ExchangeData::sendRequest_Op02, client, &TcpClient::onSendRequest_Op02);
}

MainWindow::~MainWindow()
{
    delete ui;
}

quint16 MainWindow::getColsCount()
{
    return colsCount;
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
    connect(devProps, &DeviceProperties::deleteRequest_Op02, exchangeData, &ExchangeData::onDeleteRequest_Op02);
    connect(devProps, &DeviceProperties::applyRequest_Op02, exchangeData, &ExchangeData::onApplyRequest_Op02);

    emit connectToHost();
}

void MainWindow::onUpdateTableExchange(const QVector<WordData>& words, quint16 start)   // connect!
{
    if (words.size() == start)
    {
        qDebug() << "No data to add in tableExchange";
        return;
    }

    QStringList itemsText;
    for (quint16 i = start; i < words.size(); i++)
    {
        ui->tableExchange->setRowCount(i + 1);
        setItemsText(words[i], i, itemsText);

        QTableWidgetItem *colsItems = new QTableWidgetItem[colsCount];
        for (quint16 col = 0; col < colsCount; col++)
        {
            colsItems[col].setText(itemsText[i]);
            colsItems[col].setTextAlignment(Qt::AlignCenter);
            ui->tableExchange->setItem(i, col, &colsItems[col]);
        }
        itemsText.clear();

    }
}


/* STATIC FUNCTIONS */
void MainWindow::setItemsText(const WordData& word, quint16 rowIndex, QStringList& itemsText)
{
    itemsText.append(QString(rowIndex));
    itemsText.append(QString(word.devIdx) + " / " + QString(word.chIdx));
    itemsText.append(word.time.time().toString("HH:mm:ss.zzz"));
    itemsText.append(word.time.date().toString("dd.mm.yyyy"));
    itemsText.append(word.delta.toString("HH:mm:ss.zzz"));
    itemsText.append(QString().setNum(word.address, 8));
    itemsText.append(QString().setNum(word.matrix, 2));
    itemsText.append(QString().setNum(word.word, qint32(word.encoding)));
    switch (word.encoding)
    {
    case EncodingType::BIN:
        itemsText.append(QString("BIN"));
        break;
    case EncodingType::OCT:
        itemsText.append(QString("OCT"));
        break;
    case EncodingType::DEC:
        itemsText.append(QString("DEC"));
        break;
    case EncodingType::HEX:
        itemsText.append(QString("HEX"));
        break;
    }
}
