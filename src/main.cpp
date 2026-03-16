#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>

Adafruit_BMP280 bmp;

//WiFi
const char* ssid = "BLLLM";
const char* password = "Schanzenshop100";

//Intervalle
unsigned long letzteMessung = 0;
int messungWartezeit = 5000;

//Pins
const int ldrPin = 32; 

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Verbinden");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (bmp.begin(0x76)) {
    Serial.println("Verbunden");
  } else {
    Serial.println("Fehlgeschlagen");
  }
}

void loop() {
  unsigned long jetzt = millis();

  if (jetzt - letzteMessung > messungWartezeit) {
    letzteMessung = jetzt;

    Serial.print("Temperatur: ");
    Serial.print(bmp.readTemperature());
    Serial.println("°C");
    Serial.print("Luftdruck: ");
    Serial.println(bmp.readPressure() / 100.0F);
    Serial.print("Helligkeit: ");
    Serial.println(analogRead(ldrPin));
  }


}

