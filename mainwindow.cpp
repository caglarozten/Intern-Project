#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gattlib.h"
#include "QThread"

#define GATTLIB_LOG_LEVEL GATTLIB_DEBUG

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_bleClienct = new BLEClient(ui->deviceListWidget, ui->ssidListWidget, ui->logTextEdit);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
//    ble_scan();
    ui->logTextEdit->clear();
    ui->deviceListWidget->clear();
    m_bleClienct->startBleScan();
}

void MainWindow::on_deviceListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->logTextEdit->clear();
    m_bleClienct->ble_connect_device((char *)(((QString)(item->text().split("-").at(0))).toStdString().c_str()));
    if(!m_bleClienct->isOnboardingChOk())
    {
        m_bleClienct->disconnect();
        return;
    }
    m_bleClienct->readMessage();
}

void MainWindow::on_connectButton_clicked()
{
    m_bleClienct->ssidPassword = ui->password->text();
}

void MainWindow::on_ssidListWidget_doubleClicked(const QModelIndex &index)
{
    m_bleClienct->ssidIndex = index.row();
}

void MainWindow::on_ssidListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->selectedSSID->setText(item->text());
}
