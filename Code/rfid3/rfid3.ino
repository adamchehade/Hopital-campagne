#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (2)
#define PN532_RESET (0)

// Initialisation pour I2C en utilisant l'interface Wire
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET, &Wire);

// UID autorisé (à remplacer par ton propre UID)
uint8_t authorizedUID[] = { 0x13, 0x56, 0xBB, 0x29 };
uint8_t authorizedUIDLength = 4;  // Longueur de l'UID autorisé

void setup(void) {
  Serial.begin(115200);
  Serial.println("Hello!");

  nfc.begin();  // Initialisation du module
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);  // Boucle infinie en cas d'erreur
  }

  // Configurer le module pour pouvoir lire les tags NFC
  nfc.SAMConfig();

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
      Serial.println("Accès autorisé");
    } else {
      Serial.println("Accès non autorisé");
    }

    delay(1000);
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
