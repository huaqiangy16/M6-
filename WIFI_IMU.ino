#include <Arduino_LSM6DSOX.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "Secret.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
float Ax, Ay, Az;
float Gx, Gy, Gz;
String message = " ";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char broker[] = "test.mosquitto.org";
int        port     = 1883;
const char topic[]  = "wifi_imu_12375";

//set interval for sending messages (milliseconds)
const long interval = 500;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  Serial.begin(9600);


  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alive which
  // avoids being disconnected by the broker
  mqttClient.poll();

  unsigned long currentMillis = millis();

  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(Ax, Ay, Az);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(Gx, Gy, Gz);
  }

  if(Ay > 0.2){
    message = String(1);
  }
  else if(Ay < -0.2){
    message = String(-1);
  }
  else{
    message = String(0);
  }
  Serial.print("Sending message to topic: ");
  Serial.println(topic);
  Serial.println(message);

    // send message, the Print interface can be used to set the message contents
  mqttClient.beginMessage(topic);
  mqttClient.print(message);
  mqttClient.endMessage();

  Serial.println();
  delay(100);
}