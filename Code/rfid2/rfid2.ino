#include <Wire.h>
#include <Adafruit_PN532.h>

#define PN532_IRQ   (2)
#define PN532_RESET (0)

// Initialisation pour I2C en utilisant l'interface Wire
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET, &Wire);

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

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.println("Found an NFC tag!");

    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");
    delay(1000);
  }
}
