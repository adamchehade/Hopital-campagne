#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Définition des broches
const int mq135Pin = A0; // Broche analogique pour le MQ135
#define DHTPIN D2     // Broche de données du capteur DHT11
#define DHTTYPE DHT11 // Type de capteur DHT

// Informations de connexion MQTT
const char* mqttServer = "192.168.223.170"; // Adresse IP de votre broker MQTT
const int mqttPort = 1883;                   // Port MQTT, généralement 1883
const char* mqttUser = "grp10";              // Nom d'utilisateur MQTT
const char* mqttPassword = "admin";           // Mot de passe MQTT

// Initialisation du capteur DHT
DHT dht(DHTPIN, DHTTYPE);

// Initialisation de la connexion Wi-Fi
const char* ssid = "S10+";          // SSID de votre réseau Wi-Fi
const char* password = "adam123456"; // Mot de passe de votre réseau Wi-Fi

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Seuil pour déterminer si la concentration est élevée
const float THRESHOLD = 400.0; // Seuil à 400

void setup() {
  // Initialisation du moniteur série
  Serial.begin(9600);
  Serial.println("Initialisation des capteurs...");
  
  // Initialisation du capteur DHT
  dht.begin();

  // Connexion au Wi-Fi
  connectWiFi();

  // Initialisation de la connexion MQTT
  mqttClient.setServer(mqttServer, mqttPort);
}

void loop() {
  // Assurer la connexion MQTT
  if (!mqttClient.connected()) {
    reconnectMQTT();
  }
  mqttClient.loop();

  // Lecture de la valeur analogique du capteur MQ135
  float sensorValue = analogRead(mq135Pin);

  // Lecture de l'humidité et de la température
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Vérifie si la lecture du capteur DHT a échoué
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Échec de la lecture du capteur DHT !");
    return; // Sortir de la boucle si la lecture échoue
  }

  // Vérifier si la valeur dépasse le seuil
  String gasLevel;
  if (sensorValue > THRESHOLD) {
    gasLevel = "Élevée";
  } else {
    gasLevel = "Normale";
  }

  // Création d'un message formaté au format JSON
  String message = "{\"mq135Value\": " + String(sensorValue) + 
                   ", \"gasLevel\": \"" + gasLevel + 
                   "\", \"humidity\": " + String(humidity) + 
                   ", \"temperature\": " + String(temperature) + "}";

  // Publication des données sous le topic `sensors/data`
  mqttClient.publish("sensors/data", message.c_str());

  // Affichage des valeurs sur le moniteur série
  Serial.println(message);

  // Attendre 5 secondes avant la prochaine lecture
  delay(5000); // Délai de 5 secondes
}

// Fonction pour se connecter au Wi-Fi
void connectWiFi() {
  Serial.print("Connexion au réseau Wi-Fi...");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connecté au Wi-Fi");
}

// Fonction pour reconnecter au broker MQTT
void reconnectMQTT() {
  // Boucle jusqu'à ce que nous soyons reconnectés
  while (!mqttClient.connected()) {
    Serial.print("Tentative de connexion au MQTT...");
    // Tentative de connexion
    if (mqttClient.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("connecté !");
    } else {
      Serial.print("Échec de la connexion, code d'erreur : ");
      Serial.print(mqttClient.state());
      delay(2000); // Attendre 2 secondes avant de réessayer
    }
  }
}
