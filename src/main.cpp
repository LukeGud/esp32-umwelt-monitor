#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

//Intervalle
unsigned long letzteMessung = 0;
int messungWartezeit = 5000;

void setup() {
  Serial.begin(115200);

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
    Serial.println(bmp.readTemperature());
    Serial.print("Luftdruck: ");
    Serial.println(bmp.readPressure() / 100.0F);
  }


}

