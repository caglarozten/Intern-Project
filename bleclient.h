#ifndef BLECLIENT_H
#define BLECLIENT_H

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/queue.h>
#include "gattlib.h"
#include "QTextEdit"
#include "QListWidget"
#include "QTableWidget"
#include "QByteArray"


#define GATTLIB_LOG_LEVEL GATTLIB_DEBUG
#define BLE_SCAN_TIMEOUT   4

typedef enum MessageState
{
    INITIAL = 0,
    FIRST_RESPONSE_CAME,
    REMAINING_RESPONSE_START,
    TIMEZONE_REQUEST_CAME,
    CONNECTED_CAME
}messageState_t;

typedef void (*ble_discovered_device_t)(const char* addr, const char* name);

class BLEClient
{
public:
    explicit BLEClient(QListWidget *devices = nullptr, QListWidget *ssidList = nullptr, QTextEdit *logText = nullptr);
    ~BLEClient();

    int startBleScan();
    void *ble_connect_device(char *addr);
    void disconnect();

    void setReadUUID(uuid_t uuid);
    void setWriteUUID(uuid_t uuid);

    int readMessage();
    bool isOnboardingChOk();
    static void readCallback(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data);

public:
    char* adapter_name = nullptr;
    void* adapter;

private:
    static void ble_discovered_device(void *adapter, const char* addr, const char* name, void *user_data);

public:
    QTextEdit *m_logText = nullptr;
    QListWidget *m_devices = nullptr;
    QListWidget *m_ssidList = nullptr;

    uuid_t m_readUUID;
    uuid_t m_writeUUID;
    gatt_connection_t* gatt_connection;

    bool isReadUUIDOK = false;
    bool isWriteUUIDOK = false;
    uint8_t ssidIndex = 0;
    QString ssidPassword;
    QByteArray m_payloads;
    messageState_t state = INITIAL;

};

#endif // BLECLIENT_H
