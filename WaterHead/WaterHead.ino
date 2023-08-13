#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "credentials.h"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int pumpPin = 16;
int pumpState = LOW;
const int runTime = 16;

unsigned long previousMillis = 0;
const long interval = 3000; // 1000 = 1 second

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  // Setup pumpPin
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, pumpState);
  
  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the pump is off turn it on and vice-versa:
    if (pumpState == LOW) {
      pumpState = HIGH;
      bot.sendMessage(CHAT_ID, "Pump in ON", "");
      digitalWrite(pumpPin, pumpState);
      Serial.println("Pump in ON");
    }
  }
}
