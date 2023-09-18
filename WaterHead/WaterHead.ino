#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "credentials.h"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
String chat_id;

const int pumpPin = 16;
int pumpState = LOW;
const int runTime = 16;

unsigned long previousMillis = 0;
unsigned long pumpPreviousMillis = 0;
const long interval = 1000; // 1000 = 1 second
const long pumpTime = 5000;
unsigned long currentMillis;

void runWater()
{
  pumpPreviousMillis = currentMillis;
  pumpState = HIGH;
  digitalWrite(pumpPin, pumpState);
}

void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
    {
    chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    Serial.println(text);

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/send_test_action")
    {
      bot.sendChatAction(chat_id, "typing");
      delay(4000);
      bot.sendMessage(chat_id, "Did you see the action message?");
    }
    if (text == "/start")
    {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Chat Action Bot example.\n\n";
      welcome += "/send_test_action : to send test chat action message\n";
      bot.sendMessage(chat_id, welcome);
    }
    if (text == "/add_water")
    {
      bot.sendMessage(chat_id, "pump is on");
      runWater();
    }
  }
}

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

  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
  }

  if (pumpState == HIGH && currentMillis - pumpPreviousMillis >= pumpTime) {
    pumpState = LOW;
    digitalWrite(pumpPin, pumpState);
    bot.sendMessage(chat_id, "pump is off");
  }
}
