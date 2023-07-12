#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bleclient.h"
#include "QScrollArea"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    BLEClient *m_bleClienct = nullptr;

private slots:
    void on_pushButton_clicked();

    void on_deviceListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_connectButton_clicked();

    void on_ssidListWidget_doubleClicked(const QModelIndex &index);

    void on_ssidListWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
