#include <SPI.h>
#include <LoRa.h>

#define sizeofdata 247
#define sizeoftram 255
#define Affect_ID 1 // Tram d'affectation d'ID

unsigned char local_ID[3] = {192, 168, 1}; // Identifiant local du nœud collecteur
String RTram = "";                          // Variable pour stocker la tram reçue sous forme de String
char STram[sizeoftram] = {0};               // Trame à envoyer
char data[sizeofdata];                      // Données à transmettre
char noeud_ID[8] = {0};                      // Tableau pour stocker les identifiants de nœuds affectés
String Adresses[8] = {""};                  // Tableau pour enregistrer les adresses de 8 nœuds
String AdresseActuelle = "";                // Variable pour stocker l'adresse actuelle lors de la réception d'une demande d'identification
String IP_ad = "";                          // Variable pour enregistrer l'identifiant lors de la réception d'une demande de sortie du réseau
String Data = "";                           // Variable pour enregistrer les données reçues lors de la réception d'une tram de données
volatile int nbr_noeud = 0;                 // Nombre actuel de nœuds enregistrés

// Fonction pour obtenir l'adresse à partir de la tram
String obtenir_Adresse(String tram) {
  String Adresse = (String)tram[1] + (String)tram[2] + (String)tram[3];
  return Adresse;
}

// Fonction pour obtenir l'ID à partir de la tram
String obtenir_ID(String tram) {
  String ID = "";
  for (int i = 1; i < 8; i++) {
    ID += (String)tram[i];
  }
  return ID;
}

// Fonction pour obtenir les données à partir de la tram
String obtenir_donnees(String tram) {
  String donnees = (String)tram[8] + (String)tram[9];
  return donnees;
}

// Fonction pour vérifier l'existence d'une adresse dans le tableau
int verifier_existence(String Adresse) {
  for (int i = 0; i < 8; i++) {
    if (Adresses[i] == Adresse) {
      Serial.print("Adresse trouvée : ");
      Serial.println(Adresse);
      return 1;
    }
  }
  Serial.print("Nouveau nœud - Adresse : ");
  Serial.println(Adresse);
  return 0;
}

// Fonction pour obtenir l'indice d'une adresse dans le tableau
int obtenir_indice(String Adresse) {
  for (int i = 0; i < 8; i++) {
    if (Adresses[i] == Adresse)
      return i;
  }
  return 8;
}

// Fonction pour configurer la tram d'envoi avec l'ID et l'adresse locale
void configurer_Tram(char Tram_ID, char Noeud_ID) {
  STram[0] = Tram_ID;
  STram[1] = local_ID[2];
  STram[2] = local_ID[1];
  STram[3] = local_ID[0];
  STram[4] = Noeud_ID;
  STram[5] = local_ID[1];
  STram[6] = local_ID[0];
  STram[7] = 0xFF;
}

// Fonction pour envoyer une tram
void envoyer_Tram(unsigned char *Tram) {
  unsigned int i = 0;
  LoRa.beginPacket();
  while (Tram[i] != 0xFF) {
    LoRa.print(Tram[i]);
    i++;
  }
  LoRa.endPacket();
}

// Fonction pour l'initialisation
void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("Nœud Collecteur");
  Serial.println("--------------------------------");

  if (!LoRa.begin(866.7E6)) {
    Serial.println("Initialisation échouée");
    while (1)
      ;
  }
}

// Fonction pour la boucle principale
void loop() {
  int taille_paquet = LoRa.parsePacket();
  if (taille_paquet) {
    Serial.println("Paquet reçu:");
    int i = 0;
    while (LoRa.available()) {
      RTram += (char)LoRa.read();
    }
    Serial.println(RTram);

    switch (RTram[0]) {
      case '0':
        Serial.println("Tram reçue, type : Demande d'identifiant");
        AdresseActuelle = obtenir_Adresse(RTram);
        if (verifier_existence(AdresseActuelle) == 1) {
          int ind = obtenir_indice(AdresseActuelle);
          if (ind != 8)
          {
            configurer_Tram(Affect_ID, noeud_ID[ind]);
          }
          else
            configurer_Tram(Affect_ID, 0);
          envoyer_Tram(&STram[0]);
          Serial.println("Tram d'identification envoyée.");
          RTram = "";
          break;
        }
        if (nbr_noeud < 8) {
          Adresses[nbr_noeud] = AdresseActuelle;
          noeud_ID[nbr_noeud] = (char)(nbr_noeud + 2);
          nbr_noeud++;
        }
        configurer_Tram(Affect_ID, noeud_ID[nbr_noeud - 1]);
        envoyer_Tram(&STram[0]);
        Serial.println("Tram d'identification envoyée.");
        Serial.println("--------------------------------");
        Serial.println("--------------------------------");
        RTram = "";
        break;
      case '2':
        IP_ad = obtenir_ID(RTram);
        Serial.print("Demande de sortie de réseau de la part de : ");
        Serial.println(IP_ad);
        for (int i = 0; i < 8; i++) {
          if (noeud_ID[i] == IP_ad[7]) {
            noeud_ID[i] == "";
            Adresses[i] == "";
            nbr_noeud = i;
            Serial.println("Nœud supprimé.");
            Serial.println("--------------------------------");
            Serial.println("--------------------------------");

            break;
          }
        }
        RTram = "";
        break;
      case '3':
        Serial.println("Tram reçue, type : Tram de données");
        IP_ad = obtenir_ID(RTram);
        Serial.print("ID de nœud : ");
        Serial.println(IP_ad);
        Data = obtenir_donnees(RTram);

        Serial.print("Température : ");
        Serial.println((String)RTram[15] + (String)RTram[16]);

        Serial.print("Humidité : ");
        Serial.println((String)RTram[17] + (String)RTram[18]);
        Serial.println("--------------------------------");

        RTram = "";
        break;
      default:
        break;
    }
  }
}
