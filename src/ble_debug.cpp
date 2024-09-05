#include <Arduino.h>
#include <Wire.h>
#include <regex>

#include "BLEDevice.h"

// possibly make proper selection thingy rather than hard code
#define bleServerName "Xbox Wireless Controller"

// Flags stating if should begin connecting and if the connection is up
static bool doConnect = false;
static bool connected = false;

// Address of the peripheral device, will be found during scanning
static BLEAddress *pServerAddress;

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

template<typename T, typename S>
void printUUIDS(S thingy, uint8_t indentAmount) {
	std::map<std::string, T*>& thingies = *(thingy);
	for (auto const& x : thingies) {
		Serial.println((std::string(indentAmount, '-') + " " + x.first).c_str());
	}
}


//Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
      advertisedDevice.getScan()->stop(); //Scan can be stopped, we found what we are looking for
      pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser is the one we need
      doConnect = true; //Set indicator, stating that we are ready to connect
      Serial.println("Device found. Connecting!");
    }
  }
};

bool ble_debug_connectToServer(BLEAddress pAddress) {
	BLEClient *pClient = BLEDevice::createClient();
	pClient->connect(pAddress);
	Serial.println(" - Connected to server (Debug)");
	
	// get all services
	std::map<std::__cxx11::string, BLERemoteService *>& pRemoteServices = *(pClient->getServices());
	
	// print all service uuids
	Serial.println("All detected service UUIDs: ");
	printUUIDS<BLERemoteService>(pClient->getServices(), 0);
	
	if (pRemoteServices.empty()) {
		Serial.println("No services found, restarting");
		return false;
	}

	//print all characteristics for each service
	Serial.println("All detected characteristics for the services");
	for (auto const& service : pRemoteServices) {
		Serial.println(("Service - " + service.first).c_str());
		printUUIDS<BLERemoteCharacteristic>(service.second->getCharacteristics(), 4);
	}
	return true;
}

void ble_debug_setup() {
	Serial.println("BLE Debug");
	BLEDevice::init("");
	BLEScan *pBLEScan = BLEDevice::getScan();
	pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(600);
}

void ble_debug_loop() {
	if (doConnect || connected) {
		if (ble_debug_connectToServer(*pServerAddress)) {
			Serial.println("We are now connected to the BLE Server. :)");
			Serial.println("No reading debug yet :(");
			connected = true;
		} else {
			Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE servers again.");
		}
		doConnect = false;
	}

	delay(connected ? 60000 : 1000);
}