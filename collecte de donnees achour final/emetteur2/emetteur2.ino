#include <SPI.h>
#include <LoRa.h>

#define sizeofdata 247 // Maximum d'octets de données
#define sizeoftram 255
#define Demand_ID 0     // Tram de demande d'ID
#define Out_of_Net 2    // Tram informant la sortie du nœud
#define Send_data 3     // Tram d'envoi de données

volatile unsigned char data[sizeofdata];
volatile unsigned char STram[sizeoftram]; // Trame à envoyer - pour faciliter la reconstitution
String RTram = "";                         // Trame reçue - sous forme de string pour faciliter le traitement de la Trame
volatile unsigned char local_ID[3] = {0, 0, 1}; // Adresse locale du nœud / Adresse MAC
unsigned char dest_ID[3] = {192, 168, 1};       // Adresse du destinataire - Collecteur
String ID[3] = {"", "", ""};                    // Adresse IP / Adresse affectée
int nodeID;

// Fonction pour envoyer une Tram
void envoyerTram(unsigned char *Tram) {
  unsigned int i = 0;
  LoRa.beginPacket();
  while (Tram[i] != 0xFF) {
    LoRa.print(Tram[i]);
    i++;
  }
  LoRa.endPacket();
}

void setup() {
  // Initialisation du port série & LoRa
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Test de début :");
  if (!LoRa.begin(866.7E6)) {
    Serial.println("Démarrage de LoRa échoué !");
    while (1);
  }
  Serial.println("Demande d'identification.");
  // Construction de la Tram de demande d'ID
  // Tram = data + Adresse_destination (192.168.1) + Adresse_locale (0.0.0) + Fonction
  STram[0] = Demand_ID; // Fonction

  STram[1] = local_ID[0]; // Troisième octet d'adresse locale = 0 (pas encore affecté)
  STram[2] = local_ID[1]; // Deuxième octet d'adresse locale = 0 (pas encore affecté)
  STram[3] = local_ID[2]; // Premier octet d'adresse locale = 0 (pas encore affecté)
  STram[4] = dest_ID[0];  // Troisième octet d'adresse destination = 192
  STram[5] = dest_ID[1];  // Deuxième octet d'adresse destination = 168
  STram[6] = dest_ID[2];  // Premier octet d'adresse destination = 1
  STram[7] = 0xFF;        // 0xFF pour indiquer la fin de la Tram / nécessaire au niveau du collecteur
  // Appel à la fonction envoyerTram pour envoyer la Tram
  envoyerTram(&STram[0]);
  // Recevoir la Tram d'Identifiant
  int taillePaquet = LoRa.parsePacket();
  while (!LoRa.available()); // Attend dans la boucle jusqu'à l'arrivée d'une Tram
  Serial.print("Paquet reçu '");
  // Lecture de la Tram
  while (LoRa.available()) {
    RTram += (char)LoRa.read();
  }
  Serial.print(RTram);
  int tailleRTram = RTram.length();
  // Affichage du RSSI du paquet
  Serial.print("' avec RSSI ");
  Serial.println(LoRa.packetRssi());
  // Extraction de l'adresse IP
  ID[0] = String(RTram[12]) + String(RTram[13]) + String(RTram[14]); // 192
  ID[1] = String(RTram[9]) + String(RTram[10]) + String(RTram[11]); // 168
  ID[2] = String(RTram[8]);                                        // Numéro affecté
  Serial.print("Adresse IP reçue est : ");
  Serial.print(ID[0]);
  Serial.print(".");
  Serial.print(ID[1]);
  Serial.print(".");
  Serial.println((char)ID[2][0]);
  // Conversion de l'ID en entier pour éviter le problème d'affichage en réception (ex. ID = 2 => ID_reçu = 50)
  for (int i = 2; i < 10; i++) {
    if (ID[2] == (String)i) {
      nodeID = i;
      Serial.print("ID : ");
      Serial.println(nodeID);
      break;
    }
  }
  // Fixation de la première partie de la Tram (Fonction + adresse_IP + adresse de destination)
  STram[0] = Send_data; // Fonction
  STram[1] = 192;        // Troisième octet d'adresse locale
  STram[2] = 168;        // Deuxième octet d'adresse locale
  STram[3] = nodeID;     // Premier octet d'adresse locale : identifiant affecté
  STram[4] = dest_ID[0]; // Troisième octet d'adresse destination = 192
  STram[5] = dest_ID[1]; // Deuxième octet d'adresse destination = 168
  STram[6] = dest_ID[2]; // Premier octet d'adresse destination = 1
  // À partir de l'indice 7, on peut mettre des données + la fin de la Tram est 0xFF
  Serial.println("Temperature:,Humidity:"); // Libellés pour le tracé
}

void loop() {
  int compteur = 0;
  for (int i = 0; i < 3; i++) { // Envoyer trois mesures de température puis demander la sortie
    // Générer des valeurs aléatoires de température et d'humidité
    float temperature = random(30, 50);
    float humidite = random(10, 30);

    Serial.print(temperature);
    Serial.print(" ");
    Serial.println(humidite);

    // Mettre les valeurs dans la Trame
    STram[7] = temperature;
    STram[8] = humidite;
    STram[9] = 0xFF;

    // Envoyer la Trame
    envoyerTram(&STram[0]);

    compteur++;
    delay(3000);
    if (compteur == 3) { // Après trois mesures, envoyer la demande de sortie du nœud
      // Mettre la fonction de la Trame à 2 (sortie du réseau)
      STram[0] = Out_of_Net;

      // Envoyer la Trame
      envoyerTram(&STram[0]);

      Serial.println("Sortie du réseau.");
      delay(2000);
      asm volatile("jmp 0");
      delay(5000);
      // while (1); // Attendre la réinitialisation
    }
  }
}
