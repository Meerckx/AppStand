#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <time.h>

quint16 WordData::count = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , exchangeData(new ExchangeData(this))
    , client(new TcpClient(this))
{
    qDebug() << "MainWindow Constructor" << Qt::endl;

    setupUi();

    connect(this, &MainWindow::connectToHost, client, &TcpClient::onConnectToHost);
    connect(this, &MainWindow::sendRequest_Op04, client, &TcpClient::onSendRequest_Op04);
    connect(exchangeData, &ExchangeData::createRowsForWords, this, &MainWindow::onCreateRowsForWords);
    connect(exchangeData, &ExchangeData::updateTableExchange, this, &MainWindow::onUpdateTableExchange);
    connect(exchangeData, &ExchangeData::setRowEmpty, this, &MainWindow::onSetRowEmpty);
    connect(client, &TcpClient::getDevices_Op00, exchangeData, &ExchangeData::onGetDevices_Op00);
    connect(client, &TcpClient::getChannels_Op01, exchangeData, &ExchangeData::onGetChannels_Op01);
    connect(client, &TcpClient::getWords_Op03, exchangeData, &ExchangeData::onGetWords_Op03);
    connect(client, &TcpClient::checkRequestsToRestore, exchangeData, &ExchangeData::onCheckRequestsToRestore);
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

    if (exchangeData->recievingIsActive())
    {
        exchangeData->setRecievingState(false);
        emit sendRequest_Op04();
        ui->tableExchange->clearContents();
        ui->tableExchange->setRowCount(0);
    }
    else
    {
        emit connectToHost();
    }

    connect(exchangeData, &ExchangeData::updateCbDevices, devProps, &DeviceProperties::onUpdateCbDevices);
    connect(exchangeData, &ExchangeData::updateCbChannels, devProps, &DeviceProperties::onUpdateCbChannels);
    connect(exchangeData, &ExchangeData::addReqToListWidget, devProps, &DeviceProperties::onAddReqToListWidget);
    connect(exchangeData, &ExchangeData::restoreReqListWidget, devProps, &DeviceProperties::onRestoreReqListWidget);
    connect(devProps, &DeviceProperties::currentDeviceChanged, exchangeData, &ExchangeData::onCurrentDeviceChanged);
    connect(devProps, &DeviceProperties::currentChannelChanged, exchangeData, &ExchangeData::onCurrentChannelChanged);
    connect(devProps, &DeviceProperties::addRequest_Op02, exchangeData, &ExchangeData::onAddRequest_Op02);
    connect(devProps, &DeviceProperties::deleteRequest_Op02, exchangeData, &ExchangeData::onDeleteRequest_Op02);
    connect(devProps, &DeviceProperties::applyRequest_Op02, exchangeData, &ExchangeData::onApplyRequest_Op02);

}

void MainWindow::onCreateRowsForWords(Words_t& words)
{
    if (words.size() == 0)
    {
        qDebug() << "No data to add in tableExchange";
        return;
    }

    quint16 rowCount = 0;
    for (auto device : words)
    {
        for (auto channel : device)
        {
            for (auto word : channel)
            {
                ui->tableExchange->setRowCount(rowCount + 1);
                QStringList itemsText;
                setItemsText(word, itemsText);

                for (quint16 col = 0; col < colsCount; col++)
                {
                    QTableWidgetItem *item = new QTableWidgetItem();
                    item->setText(itemsText[col]);
                    item->setTextAlignment(Qt::AlignCenter);
                    ui->tableExchange->setItem(rowCount, col, item);
                }
                rowCount++;
            }
        }
    }
}

void MainWindow::onUpdateTableExchange(Words_t& words)
{
    //qDebug() << "onUpdateTableExchange" << Qt::endl;
    if (words.size() == 0)
    {
        qDebug() << "No data to add in tableExchange";
        return;
    }

    const quint16 colIndexFromUpdate = 3;
    quint16 rowCount = 0;
    for (auto device : words)
    {
        for (auto channel : device)
        {
            for (auto word : channel)
            {
                if (word->isUpdated)
                {
                    QStringList itemsText;
                    setItemsText(word, itemsText);

                    for (quint16 col = colIndexFromUpdate; col < colsCount; col++)
                    {
                        QTableWidgetItem *item = ui->tableExchange->takeItem(rowCount, col);
                        item->setText(itemsText[col]);
                        ui->tableExchange->setItem(rowCount, col, item);
                    }
                    word->isUpdated = false;
                }
                rowCount++;
            }
        }
    }
}


void MainWindow::onSetRowEmpty(quint16 rowNumber)
{
    const quint16 colIndexFromUpdate = 3;

    for (quint16 col = colIndexFromUpdate; col < colsCount; col++)
    {
        QTableWidgetItem *item = ui->tableExchange->takeItem(rowNumber, col);
        item->setText("");
        ui->tableExchange->setItem(rowNumber, col, item);
    }
}

//void MainWindow::onUpdateTableExchange(const QVector<WordData>& words, quint16 start)   // connect!
//{
//    if (words.size() == start)
//    {
//        qDebug() << "No data to add in tableExchange";
//        return;
//    }

//    QStringList itemsText;
//    for (quint16 i = start; i < words.size(); i++)
//    {
//        ui->tableExchange->setRowCount(i + 1);
//        setItemsText(words[i], i, itemsText);

//        QTableWidgetItem *colsItems = new QTableWidgetItem[colsCount];
//        for (quint16 col = 0; col < colsCount; col++)
//        {
//            colsItems[col].setText(itemsText[col]);
//            colsItems[col].setTextAlignment(Qt::AlignCenter);
//            ui->tableExchange->setItem(i, col, &colsItems[col]);
//        }
//        itemsText.clear();

//    }
//}


/* PRIVATE FUNCTIONS */
void MainWindow::setupUi()
{
    ui->setupUi(this);
    move(screen()->geometry().center() - frameGeometry().center());
    //ui->tableExchange->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    colsCount = ui->tableExchange->columnCount();
    exchangeColumnPercents percents[] = {DEVICE, CHANNEL, LABEL, TIME, DELTA, SSM, WORD_BIN, WORD_HEX};
    ui->tableExchange->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    quint32 tableWidth = ui->tableExchange->width();
    for (quint32 col = 0; col < colsCount; col++)
    {
        quint32 size = tableWidth / 100. * percents[col];
        qDebug() << "size =" << size;
        ui->tableExchange->setColumnWidth(col, size);
    }
}

void MainWindow::setItemsText(WordData* word, QStringList& itemsText)
{
    itemsText.append(QString().setNum(word->devIdx));
    itemsText.append(QString().setNum(word->chIdx));
    itemsText.append(QString().setNum(word->address, 8));

    if (!word->isUpdated)   // При первом создании строк
    {
        itemsText.append("");
        itemsText.append("");
        itemsText.append("");
        itemsText.append("");
        itemsText.append("");
        itemsText.append("");
    }
    else
    {
        time_t wordAbsTimeS = word->time / 1000;
        struct tm* timeStruct = localtime(&wordAbsTimeS);
        char time[30] = "";
        char fulltime[60] = "";
        strftime(time, sizeof(time)-1, "%H:%M:%S", timeStruct);
        sprintf(fulltime, "%s.%03d", time, (int)(word->time % 1000));
        itemsText.append(QString(fulltime));
        itemsText.append(QString().setNum(word->delta) + "ms");
        itemsText.append(QString().setNum(word->matrix, qint32(EncodingType::BIN)));
        QString binStr;
        binStr.setNum(word->word, qint32(EncodingType::BIN));
        quint16 step = 4;
        while (binStr.size() < 32)
        {
            binStr.push_front('0');
        }
        for (quint16 i = 4; i < binStr.size(); i += step + 1)
        {
            binStr.insert(i, ' ');
        }
        itemsText.append(binStr);
        itemsText.append(QString().setNum(word->word, qint32(EncodingType::HEX)));
    }
}
