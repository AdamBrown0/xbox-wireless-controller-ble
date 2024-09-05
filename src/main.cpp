#include <Arduino.h>

#include "BLEDevice.h"
#include "ble.hh"
#include "ble_debug.hh"

const bool debug = true;

void setup() {
    Serial.begin(9600);
    Serial.println("Starting Arduino BLE Client application...");
    if (debug) {
        ble_debug_setup();
    } else {
        ble_setup();
    }
    
}

void loop() {
    if (debug) {
        ble_debug_loop();
    } else {
        ble_loop();
    }
}
