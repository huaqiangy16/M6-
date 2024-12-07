//What: arduino bluetooth connection
//Where: https://docs.arduino.cc/tutorials/nano-33-iot/bluetooth/
//Why: Bluethooth connection for transimtting IMU data
//     Reusing the bluetooth connection codes saves time for developing the transmition IMU data
#include <ArduinoBLE.h>
#include <Arduino_LSM6DSOX.h>
long previousMillis = 0;
int interval = 0;
int ledState = LOW;
float Ax, Ay, Az;
float Gx, Gy, Gz;
BLEService myService("180A"); // BLE LED Service

// BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEByteCharacteristic message("2A57", BLERead | BLEWrite);
void setup() {
  Serial.begin(9600);

  // set built in LED pin to output mode
  pinMode(LED_BUILTIN, OUTPUT);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Nano RP2040 Connect");
  BLE.setAdvertisedService(myService);

  // add the characteristic to the service
  myService.addCharacteristic(message);
  // add service
  BLE.addService(myService);
  // set the initial value for the characteristic:
  message.writeValue(0);

  // start advertising
  BLE.advertise();

  Serial.println("BLE Peripheral");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(Ax, Ay, Az);
      }

      if (IMU.gyroscopeAvailable()) {
        IMU.readGyroscope(Gx, Gy, Gz);
      }
      if(Ay > 0.2){
        message.writeValue(1);
      }
      else if(Ay < -0.2){
        message.writeValue(2);
      }
      else{
        message.writeValue(3);
      }
      delay(100);
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, LOW);         // will turn the LED off
  }
}