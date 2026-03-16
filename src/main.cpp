#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(80);
Adafruit_BMP280 bmp;

//WiFi
const char* ssid = "BLLLM";
const char* password = "Schanzenshop100";

//Intervalle
unsigned long letzteMessung = 0;
int messungWartezeit = 5000;

//Pins
const int ldrPin = 32; 

void startseiteSenden() {
  float temp = bmp.readTemperature();
  float press = bmp.readPressure() / 100;
  float bright = analogRead(ldrPin);
  String htmlSeite = "<h1>Umwelt-Monitor</h1>";
  htmlSeite = htmlSeite + "<p>Temperatur: " + String(temp) + "°C</p> <p>Luftdruck: " + String(press) + "</p> <p>Helligkeit: " + String(bright) + "</p>";
  server.send(200, "text/html", htmlSeite);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Mit WLAN verbinden");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  MDNS.begin("umweltmonitor");

  if (bmp.begin(0x76)) {
    Serial.println("BMP Verbunden");
  } else {
    Serial.println("BMP Fehlgeschlagen");
  }

  server.on("/", startseiteSenden);
  server.begin();
  Serial.println(WiFi.localIP());
}

void loop() {
  unsigned long jetzt = millis();

  server.handleClient();

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

