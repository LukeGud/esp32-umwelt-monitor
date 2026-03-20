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
unsigned long letzteLichtMessung = 0;
int messungWartezeit = 5000;
int lichtMessungIntervall = 500;

//Pins
const int ldrPin = 32; 
const int lichtLEDPin = 27;
const int refreshPin = 17;

//Interrupt flag
volatile bool refreshSignal = false;

void startseiteSenden() {
  float temp = bmp.readTemperature();
  float press = bmp.readPressure() / 100;
  float bright = analogRead(ldrPin);
  String htmlSeite = "<h1>Umwelt-Monitor</h1>";
  htmlSeite = htmlSeite + "<p>Temperatur: " + String(temp) + "°C</p> <p>Luftdruck: " + String(press) + "</p> <p>Helligkeit: " + String(bright) + "</p>";
  server.send(200, "text/html", htmlSeite);
}

void IRAM_ATTR refreshISR() {
  refreshSignal = true;
}

void setup() {
  Serial.begin(115200);

  pinMode(lichtLEDPin, OUTPUT);
  pinMode(refreshPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(refreshPin), refreshISR, FALLING);

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

  // Gibt Sensorwerte auf Monitor aus
  if (jetzt - letzteMessung > messungWartezeit || refreshSignal) {
    letzteMessung = jetzt;

    Serial.print("Temperatur: ");
    Serial.print(bmp.readTemperature());
    Serial.println("°C");
    Serial.print("Luftdruck: ");
    Serial.println(bmp.readPressure() / 100.0F);
    Serial.print("Helligkeit: ");
    Serial.println(analogRead(ldrPin));
    refreshSignal = false;
  }

  // LED reagiert auf Licht
  if (jetzt - letzteLichtMessung > lichtMessungIntervall) {
    letzteLichtMessung = jetzt;
    analogWrite(lichtLEDPin, (4095 - analogRead(ldrPin)) / 16);
  }

}

