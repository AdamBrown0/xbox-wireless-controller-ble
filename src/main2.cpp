
#include "BLEDevice.h"
#include <Wire.h>
#include <Arduino.h>

//BLE Server name (the other ESP32 name running the server sketch)
#define bleServerName "Xbox Wireless Controller"

// NO NEED TO HARD CODE UUIDS AT ALL SINCE CAN SCAN FOR THEM
/* UUID's of the service, characteristic that we want to read*/
// BLE Service
static BLEUUID bmeServiceUUID("00001812-0000-1000-8000-00805f9b34fb");
static BLEUUID serviceUUIDS[] = {
  BLEUUID("00000001-5f60-4c4f-9c83-a7953298d40d"), // unknown service
  BLEUUID("00001800-0000-1000-8000-00805f9b34fb"), // generic access
  BLEUUID("00001801-0000-1000-8000-00805f9b34fb"), // generic attribute
  BLEUUID("0000180a-0000-1000-8000-00805f9b34fb"), // device information
  BLEUUID("0000180f-0000-1000-8000-00805f9b34fb"), // battery service 
  BLEUUID("00001812-0000-1000-8000-00805f9b34fb")  // human interface device
  };

static BLEUUID manufacturerNameStringCharacteristicUUID("00002a4b-0000-1000-8000-00805f9b34fb");

//Flags stating if should begin connecting and if the connection is up
static bool doConnect = false;
static bool connected = false;

//Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;
 
//Characteristicd that we want to read
static BLERemoteCharacteristic* manufacturerNameStringCharacteristic;

//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

//Variables to store temperature and humidity
char* manufacturerNameString;

//Flags to check whether new temperature and humidity readings are available
bool newManufacturerNameString = false;

template<typename T, typename S>
void printUUIDS(S thingy) {
  std::map<std::string, T*>& thingies = *(thingy);
  for (auto const& x : thingies) {
    std::string thing;
    thing += x.first;
    thing += " : ";
    thing += x.second->getUUID().toString();
    thing += "\n";
    Serial.print(thing.c_str()); 
  }
}

//When the BLE Server sends a new temperature reading with the notify property
static void manufacturerNameStringNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                        uint8_t* pData, size_t length, bool isNotify) {
  //store temperature value
  manufacturerNameString = (char*)pData;
  newManufacturerNameString = true;
  Serial.print(newManufacturerNameString);
}

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
  Serial.println(" - Connected to server");

  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(bmeServiceUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(bmeServiceUUID.toString().c_str());
    Serial.print("All detected service UUIDS\n");
    printUUIDS<BLERemoteService>(pClient->getServices());
    return (false);
  }

  // Obtain a reference to the characteristics in the service of the remote BLE server.
  manufacturerNameStringCharacteristic = pRemoteService->getCharacteristic(manufacturerNameStringCharacteristicUUID);

  if (manufacturerNameStringCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID\n");
    Serial.print("All detected characteristic UUIDS\n");
    printUUIDS<BLERemoteCharacteristic>(pRemoteService->getCharacteristics());
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  manufacturerNameStringCharacteristic->registerForNotify(manufacturerNameStringNotifyCallback);
  return true;
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
 
//function that prints the latest sensor readings in the OLED display
void printReadings(){
  Serial.print("Temperature: ");
  Serial.print(manufacturerNameString);
}

void readCharacteristicValue(BLERemoteCharacteristic* characteristic) {
    if (characteristic != nullptr) {
        std::string value = characteristic->readValue();
        Serial.print("Read value: ");
        Serial.println(value.c_str());
    } else {
        Serial.println("Characteristic is null, cannot read value.");
    }
}

void main2_setup() {
  //Start serial communication
  Serial.begin(9600);
  Serial.println("Starting Arduino BLE Client application...");
  Serial.print("BLE Client");

  //Init BLE device
  BLEDevice::init("");
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 30 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(100000000);
}

void main2_loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect || connected) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      if (manufacturerNameStringCharacteristic != nullptr) {
        std::map<std::string, BLERemoteDescriptor*>& descriptors = *(manufacturerNameStringCharacteristic->getDescriptors());
        
        if (descriptors.size() > 0) {
          Serial.println("Found the following descriptors:");
          for (auto const& descriptorPair : descriptors) {
            BLERemoteDescriptor* pDescriptor = descriptorPair.second;
            Serial.print("Descriptor UUID: ");
            Serial.println(pDescriptor->getUUID().toString().c_str());
            descriptorPair.second->writeValue((uint8_t*)notificationOn, 2, true);
            Serial.print(descriptorPair.second->readValue().c_str());
          }
        } else {
          Serial.println("No descriptors found for this characteristic.");
          Serial.println("Reading from characteristic.");
          readCharacteristicValue(manufacturerNameStringCharacteristic);
        }

      } else {
        Serial.println("Characteristic is null, cannot find descriptors.");
      }

      connected = true;
    } else {
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  //if new temperature readings are available, print in the OLED
  if (newManufacturerNameString){
    newManufacturerNameString = false;
    printReadings();
  }
  delay(1000); // Delay a second between loops.
}