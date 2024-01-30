#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHostAddress>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , exchangeData(new ExchangeData(this))
    , client(new TcpClient(this))
{
    ui->setupUi(this);
    ui->tableExchange->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qDebug() << "MainWindow Constructor" << Qt::endl;

    connect(client, &TcpClient::readyToProcess_00, exchangeData, &ExchangeData::onReadyToProcess_00);
//    socket.connectToHost(QHostAddress("127.0.0.1"), 50001);
//    connect(&socket, SIGNAL(connected()), this, SLOT(onConnected()));
//    connect(&socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
//    qDebug() << "Start\n";
}

MainWindow::~MainWindow()
{
    delete ui;
}


