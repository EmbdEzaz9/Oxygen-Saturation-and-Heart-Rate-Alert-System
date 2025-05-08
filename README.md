# Oxygen-Saturation-and-Heart-Rate-Alert-System
Developed a Real-time system using NodeMCU to monitor SPO2 and BPM levels for (MedTech) medical applications.

✅ What the Code Does without OLED_Display_Interfacing

#This program:

i)Connects to a Wi-Fi network.

ii)Initializes a MAX30100 Pulse Oximeter sensor to read:

Heart Rate (BPM)
Blood Oxygen Saturation (SpO2)

iii)Sends the sensor data to Ubidots using the MQTT protocol.

✅ It is designed to run on ESP8266 microcontrollers using libraries like:
ESP8266WiFi.h

BlynkSimpleEsp8266.h

MAX30100_PulseOximeter.h

OakOLED.h

✅ Project Summary: SpO2 and Heart Rate Monitoring

What It Does with OLED_Display_Interfacing & It Connects to Wi-Fi and the Blynk IoT platform using your credentials.

Uses the MAX30100 sensor to measure:

Heart rate (BPM)

Blood oxygen saturation (SpO2)

Displays real-time data on an OLED display.


Sends data to the Blynk mobile app on virtual pins V7 (BPM) and V8 (SpO2).
