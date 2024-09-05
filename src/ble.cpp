#include <Arduino.h>
#include <Wire.h>

#include "BLEDevice.h"

// possibly make proper selection thingy rather than hard code
#define bleServerName "Xbox Wireless Controller"

/* UUID's of the services that we want to read */
// static BLEUUID serviceUUIDS[] = {
//   BLEUUID("00000001-5f60-4c4f-9c83-a7953298d40d"), // unknown service
//   BLEUUID("00001800-0000-1000-8000-00805f9b34fb"), // generic access
//   BLEUUID("00001801-0000-1000-8000-00805f9b34fb"), // generic attribute
//   BLEUUID("0000180a-0000-1000-8000-00805f9b34fb"), // device information
//   BLEUUID("0000180f-0000-1000-8000-00805f9b34fb"), // battery service 
//   BLEUUID("00001812-0000-1000-8000-00805f9b34fb")  // human interface device
//   };

/* UUID's of the characteristics that we want to read */

std::multimap<std::string, std::string> serviceCharacteristicUUIDMap = {
    {
        "00000001-5f60-4c4f-9c83-a7953298d40d", // unknown service
        "0x209"
    },
    {
        "00001800-0000-1000-8000-00805f9b34fb", // generic access
        "0xreplaceme"
    },
    {
        "00001801-0000-1000-8000-00805f9b34fb", // generic attribute
        "0xreplaceme"
    },
    {
        "0000180a-0000-1000-8000-00805f9b34fb", // device information
        "0xreplaceme"
    },
    {
        "0000180f-0000-1000-8000-00805f9b34fb", // battery service
        "0xreplaceme"
    },
    {
        "00001812-0000-1000-8000-00805f9b34fb", // human interface device
        "0xreplaceme"
    }
};

// Flags stating if should begin connecting and if the connection is up
static bool doConnect = false;
static bool connected = false;

// Address of the peripheral device, will be found during scanning
static BLEAddress *pServerAddress;

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

void ble_setup() {
    Serial.println("BLE Client");
}

void ble_loop() {
    Serial.println("CLIENT");
}