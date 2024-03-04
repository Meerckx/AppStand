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
    /* Начальная настройка графики */
    this->setupUi();

    /* Подключение слотов и сигналов */
    connect(this, &MainWindow::connectToHost, client, &TcpClient::onConnectToHost);
    connect(this, &MainWindow::sendRequest_Op04, client, &TcpClient::onSendRequest_Op04);
    connect(client, &TcpClient::stopReceiving, this, &MainWindow::onStopReceiving);
    connect(exchangeData, &ExchangeData::createRowsForWords, this, &MainWindow::onCreateRowsForWords);
    connect(exchangeData, &ExchangeData::updateTableExchange, this, &MainWindow::onUpdateTableExchange);
    connect(exchangeData, &ExchangeData::setRowEmpty, this, &MainWindow::onSetRowEmpty);
    connect(this, &MainWindow::setMsecUpdateRowsTimer, exchangeData, &ExchangeData::onSetMsecUpdateRowsTimer);
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


/* SLOTS */
void MainWindow::on_btnDevProps_clicked()
{
    /* Каждый раз при нажатии на кнопку создаётся новое окно настроек
     * При закрытии окна, объект автоматически удаляется */
    DeviceProperties *devProps = new DeviceProperties();
    devProps->show();

    if (client->getSocket()->state() == QTcpSocket::ConnectedState)
    {
        emit sendRequest_Op04();
    }
    else
    {
        emit connectToHost();
    }

    /* Подключение слотов и сигналов */
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

    /* Для каждого слова создаём строчку в таблице, где изначально
     * заполнены только поля "Устройство/Канал/Метка" */
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
    if (words.size() == 0)
    {
        qDebug() << "No data to add in tableExchange";
        return;
    }

    /* Для каждого слова, данные которого были изменены, обновляем соответствующую строчку */
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
    /* Т.к. функция вызывается, когда слово долго не приходило, а не в случае его полного удаления,
     * оставляем информацию только про поля "Устройство/Канал/Метка" */
    const quint16 colIndexFromUpdate = 3;

    for (quint16 col = colIndexFromUpdate; col < colsCount; col++)
    {
        QTableWidgetItem *item = ui->tableExchange->takeItem(rowNumber, col);
        item->setText("");
        ui->tableExchange->setItem(rowNumber, col, item);
    }
}

void MainWindow::onStopReceiving()
{
    exchangeData->stopTimers();
    ui->tableExchange->clearContents();
    ui->tableExchange->setRowCount(0);
}

void MainWindow::on_leUpdateRowsTimeout_returnPressed()
{
    /* Вызывается в случае нажатия клавиши "Enter" при вводе частоты обновления слов */
    if (ui->leUpdateRowsTimeout->text() > 0)
    {
        emit setMsecUpdateRowsTimer(ui->leUpdateRowsTimeout->text().toUShort());
    }
}


/* PUBLIC FUNCTIONS */
quint16 MainWindow::getColsCount()
{
    return colsCount;
}


/* PRIVATE FUNCTIONS */
void MainWindow::setupUi()
{
    ui->setupUi(this);
    move(screen()->geometry().center() - frameGeometry().center());                                     // Центрирование окна
    ui->verticalLayout_3->setAlignment(Qt::AlignCenter);                                                // Центрирование таблицы
    ui->tableExchange->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);                    // Запрет растягивать колонки таблицы

    /* Настройка ширины столбцов в зависимости от заданных процентных соотношений */
    exchangeColumnPercents percents[] = {DEVICE, CHANNEL, LABEL, TIME, DELTA, SSM, WORD_BIN, WORD_HEX};
    colsCount = ui->tableExchange->columnCount();
    quint32 tableWidth = ui->tableExchange->maximumWidth();
    for (quint32 col = 0; col < colsCount; col++)
    {
        quint32 size = tableWidth / 100. * percents[col];
        ui->tableExchange->setColumnWidth(col, size);
    }

    /* Ограничение ввода частоты обновления слов (только числа 0-1000) */
    ui->leUpdateRowsTimeout->setValidator(new QIntValidator(0, 1000, this));
}

void MainWindow::setItemsText(WordData* word, QStringList& itemsText)
{
    itemsText.append(QString().setNum(word->devIdx));
    itemsText.append(QString().setNum(word->chIdx));
    itemsText.append(QString().setNum(word->address, 8));

    /* При первом создании строки непустыми остаются только поля "Устройство/Канал/Метка" */
    if (!word->isUpdated)
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
        /* Задание определённого формата времени */
        time_t wordAbsTimeS = word->time / 1000;
        struct tm* timeStruct = localtime(&wordAbsTimeS);
        char time[30] = "";
        char fulltime[60] = "";
        strftime(time, sizeof(time)-1, "%H:%M:%S", timeStruct);
        sprintf(fulltime, "%s.%03d", time, (int)(word->time % 1000));
        itemsText.append(QString(fulltime));

        itemsText.append(QString().setNum(word->delta) + "ms");
        itemsText.append(QString().setNum(word->matrix, qint32(EncodingType::BIN)));

        /* Если количество разрядов в двоичном и шестнадцатиричном представлении меньше
         * определённого числа, то дополняем его незначащими нулями */
        const qint32 binDigitsCount = 32;
        QString binStr;
        binStr.setNum(word->word, qint32(EncodingType::BIN));
        while (binStr.size() < binDigitsCount)
        {
            binStr.push_front('0');
        }
        quint16 step = 4;
        for (quint16 i = 4; i < binStr.size(); i += step + 1)
        {
            binStr.insert(i, ' ');
        }
        itemsText.append(binStr);

        const qint32 hexDigitsCount = 8;
        QString hexStr;
        hexStr.setNum(word->word, qint32(EncodingType::HEX));
        while (hexStr.size() < hexDigitsCount)
        {
            hexStr.push_front('0');
        }
        itemsText.append(hexStr);
    }
}
