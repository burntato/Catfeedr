#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_I2CDevice.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "hidden";
const char *password = "hidden";
const char *mqtt_server = "broker.hivemq.com";

// Pub/Sub MQTT
WiFiClient espClient;
PubSubClient client(espClient);
// WiFi

// Timer
long now = millis();
long lastMeasure = 0;
String macAddr = "";
// Timer

// Servo SG90
int servoPin = D0;
Servo sg90;
// Servo SG90

// HC-SR04
int triggerPin = D2;
int echoPin = D1;
// HC-SR04

// WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Mulai menghubungkan ke WiFi
  WiFi.begin(ssid, password);

  // proses koneksi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Konfirmasi koneksi berhasil
  Serial.println("");
  Serial.println("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
  macAddr = WiFi.macAddress();
  Serial.println(macAddr);
}

// Reconnect WiFi
void reconnect() {

  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (client.connect(macAddr.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup()
{

  // WiFi
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  // Servo SG90
  sg90.attach(servoPin);
  // Servo SG90

  Serial.begin(9600);

  // HC-SR04
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // reset servo ke 0 derajat
  sg90.write(0);
}

// HC-SR04
// Mengukur jarak
int rangeCheck() {
  long duration, distance;
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.println(distance);
  delay(2000);
  // Mengembalikan nilai jarak
  return distance;
}

// Servo SG90
void moveServo() {
  
  // Menggerakkan servo 0-180 2x
  sg90.write(180);
  client.publish("esp/servo", "180"); 
  delay(2000);
  sg90.write(0);
  client.publish("esp/servo", "0"); 
  delay(500);
  sg90.write(180);
  client.publish("esp/servo", "180"); 
  delay(2000);
  sg90.write(0);
  client.publish("esp/servo", "0"); 

}

// Loop
void loop() {
  if (!client.connected()) {
    reconnect();
  }

  if (!client.loop()) {
    client.connect(macAddr.c_str());
  }

  now = millis();
  client.publish("esp/range", String(rangeCheck()).c_str());

  static bool triggerFlag = false;
  static unsigned long triggerTime = 0;

  if (now - lastMeasure > 5000) {
    lastMeasure = now;
    const int range = rangeCheck();

    if (range > 13) {
      triggerFlag = true;
      triggerTime = millis();
    }
  }

  if (triggerFlag && millis() - triggerTime < 5000) {
    const int range = rangeCheck();

    if (range > 13) {
      moveServo();
    }
  }

  Serial.println(rangeCheck());
  Serial.println(triggerFlag);
  Serial.println(triggerTime);
}
