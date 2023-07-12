#include "bleclient.h"
#include "QDebug"

BLEClient *stClient = nullptr;
uint8_t firstResponseHeader[9] = {0x06, 0x0A, 0x61, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00};
uint8_t remainingResponseHeader[9] = {0x06, 0x0A, 0x61, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00};
uint8_t timeZoneReqPacket[4] = {0x06, 0x0D, 0x00, 0x00};
uint8_t connectedPacket[4] = {0x06, 0x0C, 0x00, 0x00};


BLEClient::BLEClient(QListWidget *devices, QListWidget *ssidList, QTextEdit *logText)
{
    m_devices = devices;
    m_ssidList = ssidList;
    m_logText = logText;
    stClient = this;
}

BLEClient::~BLEClient()
{
    gattlib_disconnect(gatt_connection);
}

int BLEClient::startBleScan()
{
    int ret;

    ret = gattlib_adapter_open(adapter_name, &adapter);
    if (ret) {
        m_logText->append("GATTLIB_ERROR --- Failed to open adapter.");
        return 1;
    }

    ret = gattlib_adapter_scan_enable(adapter, ble_discovered_device, BLE_SCAN_TIMEOUT, this /* user_data */);
    if (ret) {
        m_logText->append("GATTLIB_ERROR --- Failed to scan.");
        goto EXIT;
    }

    gattlib_adapter_scan_disable(adapter);

    m_logText->append("Scan completed");

EXIT:
    gattlib_adapter_close(adapter);
    return ret;
}

void *BLEClient::ble_connect_device(char *addr)
{
//    gatt_connection_t* gatt_connection;
    gattlib_primary_service_t* services;
    gattlib_characteristic_t* characteristics;
    int services_count, characteristics_count;
    char uuid_str[MAX_LEN_UUID_STR + 1];
    int ret, i;

    m_logText->append(QString::asprintf("------------START %s---------------", addr));

    gatt_connection = gattlib_connect(NULL, addr, GATTLIB_CONNECTION_OPTIONS_LEGACY_DEFAULT);
    if (gatt_connection == NULL) {
        m_logText->append("GATTLIB_ERROR --- Fail to connect to the bluetooth device.");
        goto connection_exit;
    } else {
        m_logText->append("Succeeded to connect to the bluetooth device.");
    }

    ret = gattlib_discover_primary(gatt_connection, &services, &services_count);
    if (ret != 0) {
        m_logText->append("GATTLIB_ERROR --- Fail to discover primary services.");
        goto disconnect_exit;
    }

    for (i = 0; i < services_count; i++) {
        gattlib_uuid_to_string(&services[i].uuid, uuid_str, sizeof(uuid_str));

        m_logText->append(QString::asprintf("service[%d] start_handle:%02x end_handle:%02x uuid:%s\n", i,
                                                          services[i].attr_handle_start, services[i].attr_handle_end,
                                                          uuid_str));
    }
    free(services);

    ret = gattlib_discover_char(gatt_connection, &characteristics, &characteristics_count);
    if (ret != 0) {
        m_logText->append("GATTLIB_ERROR --- Fail to discover characteristics.");
        goto disconnect_exit;
    }
    for (i = 0; i < characteristics_count; i++) {
        gattlib_uuid_to_string(&characteristics[i].uuid, uuid_str, sizeof(uuid_str));

        m_logText->append(QString::asprintf("characteristic[%d] properties:%02x value_handle:%04x uuid:%s\n", i,
                                                          characteristics[i].properties, characteristics[i].value_handle,
                                                          uuid_str));
        if(0xAC02 == characteristics[i].uuid.value.uuid16)
        {
            m_logText->append("find read UUID");
            setReadUUID(characteristics[i].uuid);
        }

        if(0xAC01 == characteristics[i].uuid.value.uuid16)
        {
            m_logText->append("find write UUID");
            setWriteUUID(characteristics[i].uuid);
        }
    }
    free(characteristics);

disconnect_exit:
//    gattlib_disconnect(gatt_connection);

connection_exit:
    m_logText->append(QString::asprintf("------------DONE %s---------------", addr));
    return NULL;
}

void BLEClient::disconnect()
{
    gattlib_disconnect(gatt_connection);
}

void BLEClient::setReadUUID(uuid_t uuid)
{
    m_readUUID = uuid;
    isReadUUIDOK = true;
}

void BLEClient::setWriteUUID(uuid_t uuid)
{
    m_writeUUID = uuid;
    isWriteUUIDOK = true;
}

int BLEClient::readMessage()
{
    int ret;
    gattlib_register_notification(gatt_connection, readCallback, this);
    ret = gattlib_notification_start(gatt_connection, &m_readUUID);
    return ret;
}

bool BLEClient::isOnboardingChOk()
{
    return (isWriteUUIDOK && isReadUUIDOK);
}

void BLEClient::readCallback(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data)
{
    BLEClient *obj = (BLEClient*)(user_data);
    obj->m_logText->append(QString::number(data_length));
//    obj->m_logText->append("remaining len:");
//    obj->m_logText->append(QString::number(data[9]));

//    for (int i = 0; i < data_length; i++) {
//        obj->m_payloads.append(data[i]);
//    }

}

void BLEClient::ble_discovered_device(void *adapter, const char *addr, const char *name, void *user_data)
{
    BLEClient *obj = (BLEClient*)(user_data);
    int ret;

    if (name) {
        obj->m_devices->addItem(QString::asprintf("%s-%s", addr, name));
    } else {
        obj->m_devices->addItem(QString::asprintf("%s", addr));
    }
}
