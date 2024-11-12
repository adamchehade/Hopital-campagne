#include <Wire.h>
#include <Adafruit_PN532.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h> // Utilisation de la bibliothèque ESP8266 pour la connexion Wi-Fi

#define PN532_IRQ   (2)
#define PN532_RESET (0)

// Initialisation pour I2C en utilisant l'interface Wire
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET, &Wire);

// UID autorisé et le nom correspondant (à remplacer par ton propre UID et nom)
uint8_t authorizedUID[] = { 0x13, 0x56, 0xBB, 0x29 };
const char* authorizedName = "Medical";  // Nom associé à l'UID autorisé
uint8_t authorizedUIDLength = 4;  // Longueur de l'UID autorisé

// Informations pour la connexion Wi-Fi
const char* ssid = "S10+"; // Ton SSID
const char* password = "adam123456"; // Ton mot de passe Wi-Fi

// Configuration du client MQTT
const char* mqttServer = "192.168.223.170"; // Adresse IP de ton serveur MQTT
const int mqttPort = 1883; // Port du serveur MQTT
const char* mqttUser = "grp10";  // Nom d'utilisateur MQTT
const char* mqttPassword = "admin";  // Mot de passe MQTT
const char* topic = "rfid"; // Le topic pour publier les données

WiFiClient espClient; 
PubSubClient client(espClient);

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  // Connexion au Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  nfc.begin();  // Initialisation du module
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);  // Boucle infinie en cas d'erreur
  }

  // Configurer le module pour pouvoir lire les tags NFC
  nfc.SAMConfig();

  // Initialiser le client MQTT
  client.setServer(mqttServer, mqttPort);

  Serial.println("Waiting for an NFC tag...");
}

void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer pour stocker l'UID
  uint8_t uidLength;                        // Longueur de l'UID

  // Lire une carte NFC
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");

    Serial.print("UID Length: "); Serial.print(uidLength, DEC); Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println("");

    // Vérification si l'UID correspond à l'UID autorisé
    if (uidLength == authorizedUIDLength && compareUID(uid, authorizedUID, uidLength)) {
      Serial.println("Acces autorise");
      publishData(authorizedName, true);  // Publier le nom "Adam" et accès autorisé
    } else {
      Serial.println("Acces non autorise");
      publishData("Inconnu", false);  // Publier "Inconnu" pour accès non autorisé
    }

    delay(1000);
  }

  // Maintenir la connexion MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

// Fonction pour publier les données sur MQTT
void publishData(const char* name, bool accessGranted) {
  char message[50];
  
  if (accessGranted) {
    snprintf(message, sizeof(message), "%s accès autorisé", name);  // Message pour accès autorisé
  } else {
    snprintf(message, sizeof(message), "%s accès non autorisé", name);  // Message pour accès non autorisé
  }
  
  client.publish(topic, message);  // Publier le message sur le topic MQTT
}

// Fonction de reconnexion au serveur MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Essayer de se connecter avec nom d'utilisateur et mot de passe
    if (client.connect("NFC_Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Fonction pour comparer deux UID
bool compareUID(uint8_t *uid, uint8_t *authorizedUID, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    if (uid[i] != authorizedUID[i]) {
      return false;
    }
  }
  return true;
}
