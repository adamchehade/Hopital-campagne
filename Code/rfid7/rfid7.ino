#include <Wire.h>
#include <Adafruit_PN532.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define PN532_IRQ   (2)
#define PN532_RESET (0)

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET, &Wire);

uint8_t authorizedUID[] = { 0x13, 0x56, 0xBB, 0x29 };
const char* authorizedName = "Medical";
uint8_t authorizedUIDLength = 4;

const char* ssid = "S10+";
const char* password = "adam123456";

const char* mqttServer = "192.168.223.170";
const int mqttPort = 1883;
const char* mqttUser = "grp10";
const char* mqttPassword = "admin";
const char* topic = "rfid";

WiFiClient espClient; 
PubSubClient client(espClient);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }

  nfc.SAMConfig();
  client.setServer(mqttServer, mqttPort);
  Serial.println("Waiting for an NFC tag...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");
    if (uidLength == authorizedUIDLength && compareUID(uid, authorizedUID, uidLength)) {
      Serial.println("Acces autorise");
      publishData(authorizedName, true);
    } else {
      Serial.println("Acces non autorise");
      publishData("Inconnu", false);
    }
    delay(1000);
  }

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void publishData(const char* name, bool accessGranted) {
  char message[50];
  snprintf(message, sizeof(message), "%s accès %s", name, accessGranted ? "autorisé" : "non autorisé");
  client.publish(topic, message);
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("NFC_Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
    } else {
      delay(5000);
    }
  }
}

bool compareUID(uint8_t *uid, uint8_t *authorizedUID, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;
    }
  }
  return true;
}
