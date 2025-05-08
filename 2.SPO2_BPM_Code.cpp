/*## What Does This program:
i)Connects to a Wi-Fi network.
ii)Initializes a MAX30100 Pulse Oximeter sensor to read:
Heart Rate (BPM)
Blood Oxygen Saturation (SpO2)
iii)Sends the sensor data to Ubidots using the MQTT protocol.  ##*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

// Wi-Fi and MQTT Settings
#define WIFISSID            "iPhone"
#define PASSWORD            "Ezaz@4321"
#define TOKEN               "BBFF-fN7qYkwBkSlrdyu9Vlr2sRGDIK5Cl9"
#define DEVICE_LABEL        "oxygen-saturation-monitor"
#define VARIABLE_LABEL_1    "HeartRate"
#define VARIABLE_LABEL_2    "SpO2"
#define MQTT_CLIENT_NAME    "EI_OXMO"
#define MQTT_BROKER         "industrial.api.ubidots.com"
#define MQTT_PORT           1883

// Reporting interval (in milliseconds)
#define REPORTING_PERIOD_MS 1000

// Globals
ESP8266WiFiMulti WiFiMulti;
WiFiClient ubidots;
PubSubClient client(ubidots);
PulseOximeter pox;
uint32_t tsLastReport = 0;
char payload[700];
char topic[150];
char str_val_1[6];
char str_val_2[6];

// Beat detection callback (can be extended)
void onBeatDetected() {
  // e.g., blink LED or log pulse detection
}

// MQTT message callback (not used here, but good to have)
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Ensure MQTT is connected
void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("MQTT connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 2 seconds...");
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFiMulti.addAP(WIFISSID, PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // MQTT setup
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);

  // Initialize MAX30100 sensor
  Serial.print("Initializing Pulse Oximeter... ");
  if (!pox.begin()) {
    Serial.println("FAILED");
    while (true); // halt
  }
  Serial.println("SUCCESS");
  pox.setIRLedCurrent(MAX30100_LED_CURR_24MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float heartRate = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    Serial.print("BPM: "); Serial.print(heartRate);
    Serial.print(" | SpO2: "); Serial.print(spo2); Serial.println("%");

    dtostrf(heartRate, 4, 2, str_val_1);
    dtostrf(spo2, 4, 2, str_val_2);

    sprintf(topic, "/v1.6/devices/%s", DEVICE_LABEL);
    sprintf(payload, "{\"%s\": {\"value\": %s}, \"%s\": {\"value\": %s}}",
            VARIABLE_LABEL_1, str_val_1, VARIABLE_LABEL_2, str_val_2);

    client.publish(topic, payload);
    client.loop();
    tsLastReport = millis();
  }
}
