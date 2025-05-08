#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "MAX30100_PulseOximeter.h"
#include "Adafruit_GFX.h"
#include "OakOLED.h"

#define BLYNK_PRINT Serial
#define REPORTING_PERIOD_MS 1000

// OLED and Sensor
OakOLED oled;
PulseOximeter pox;

// WiFi and Blynk Auth Credentials
char auth[] = "b1vu48_RNOjhdfo5eAzcAUb53J2Zl3L9";  // Blynk Auth Token
char ssid[] = "iPhone";                           // Wi-Fi SSID
char pass[] = "Asdf@fghj";                        // Wi-Fi Password

float BPM, SpO2;
uint32_t tsLastReport = 0;

// Heartbeat Bitmap
const unsigned char bitmap [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00,
  0x3f, 0xf9, 0xff, 0xc0, 0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0,
  0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xf7, 0xff, 0xf0,
  0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
  0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0,
  0x3f, 0xfd, 0xbf, 0xc0, 0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00,
  0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00, 0x01, 0xff, 0xf8, 0x00,
  0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
  0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// Callback for heartbeat detection
void onBeatDetected() {
  Serial.println("Beat Detected!");
  oled.drawBitmap(60, 20, bitmap, 28, 28, 1);
  oled.display();
}

void setup() {
  Serial.begin(115200);
  pinMode(16, OUTPUT);  // General purpose pin, optional

  oled.begin();
  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(1);
  oled.setCursor(0, 0);
  oled.println("Initializing...");
  oled.display();

  Blynk.begin(auth, ssid, pass);
  Serial.println("Connecting to WiFi & Blynk...");

  if (!pox.begin()) {
    Serial.println("Pulse Oximeter INIT FAILED");
    oled.clearDisplay();
    oled.setCursor(0, 0);
    oled.println("Sensor Init Failed");
    oled.display();
    while (true); // Stop here
  }

  pox.setOnBeatDetectedCallback(onBeatDetected);

  oled.clearDisplay();
  oled.setCursor(0, 0);
  oled.println("Sensor Init SUCCESS");
  oled.display();
}

void loop() {
  pox.update();
  Blynk.run();

  BPM = pox.getHeartRate();
  SpO2 = pox.getSpO2();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    Serial.print("Heart Rate: "); Serial.print(BPM);
    Serial.print(" bpm | SpO2: "); Serial.print(SpO2); Serial.println(" %");

    Blynk.virtualWrite(V7, BPM);
    Blynk.virtualWrite(V8, SpO2);

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(1);

    oled.setCursor(0, 0);
    oled.println("Heart BPM");
    oled.setCursor(0, 16);
    oled.println(BPM);

    oled.setCursor(0, 30);
    oled.println("SpO2");
    oled.setCursor(0, 45);
    oled.println(SpO2);

    oled.display();
    tsLastReport = millis();
  }
}
