
#include <SPI.h>
#include <LoRa.h>

#define sizeofdata 247
#define sizeoftram 255
#define Affect_ID 1 // Tram d'affectation d'ID
#define error 5 // Tram d'indication de saturation

unsigned char local_ID[3] = {192, 168, 1};
// Tableau qui recoi la tram
String RTram = ""; // Variable pour stocker la tram reçu sous forme de stirng
 char STram[sizeoftram] = {0};
 char data[sizeofdata];
 char noeud_ID[8] = {0}; // tab qui stock les adresses affectes
String MAC[8] = {""}; // Tableau pour enregistrement des adresse MAC de 8 nodes
String Mac_ad = ""; // var pour enregistrer mac adresse lors de demande d'identification
String IP_ad = ""; // var pour enregistré l'adresse IP (affecté) lors de reception d'un trame de donnée ou une tram de sortie de réseau
String Data = ""; // var pour enregistré les données arrivés lors de reception de data
volatile int nbr_noeud = 0;
String get_Mac(String Rtram) { // fonction pour extraire l'adresse MAC_ad
  String Mac = (String)Rtram[1] + (String)Rtram[2] + (String)Rtram[3];
  return Mac;
}
String get_ID(String Rtram) { // fonction qui permet de trouver l'ID du noeud
  String ID = "";
  for (int i = 1 ; i < 8 ; i++) {
    ID += (String)Rtram[i];
  }
  return ID;
}
String get_data(String Rtram) {
  String data = (String)Rtram[8] + (String)Rtram[9];
  return data;
}
int chek_if_exist(String Mac_Ad) {
  for (int i = 0 ; i < 8 ; i++) {
    if (MAC[i] == Mac_Ad) {
      Serial.print("MAC Adresse trouvée : ");
      Serial.println(Mac_Ad);
      return 1;
    }
  }
  Serial.print("nouveau noeud - Adresse MAC : ");
  Serial.println(Mac_Ad);
  return 0;
}


int get_indice(String Mac_ad) {
  for (int i = 0 ; i < 8 ; i++) {
    if (MAC[i] == Mac_ad) return i;
  }
  return 8;
}
void Set_Tram(char Tram_ID, char Noeud_ID) {
  STram[0] = Tram_ID;
  STram[1] = local_ID[2]; // 1
  STram[2] = local_ID[1]; // 168
  STram[3] = local_ID[0]; // 192
  STram[4] = Noeud_ID;// Premier octet X de l'adress a attribuer
  STram[5] = local_ID[1]; // 168
  STram[6] = local_ID[0]; // 192 -> adress 192,168,X
  STram[7] = 0xFF;
}
// Fonction qui permet d'envoyer une Tram
void sendTram(unsigned char *Tram) {
  unsigned int i = 0;
  LoRa.beginPacket();
  while (Tram[i] != 0xFF) {
    LoRa.print(Tram[i]);
    i++;
  }
  LoRa.endPacket();
}
void setup() {
  // Initialisation du port serie
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Noeud Collecteur");
  // Initialisation LoRa a la frequence 866 MHz
  if (!LoRa.begin(866.7E6)) {
    Serial.println("Initialisation échoué");
    while (1);
  }
}
void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    //received a packet
    Serial.println("Received packet:");
    // read packet
    int i = 0;
    while (LoRa.available()) {
      RTram += (char)LoRa.read();
    }
    Serial.println(RTram);
    //print RSSI of packet
    //Serial.print("' with RSSI ");
    //Serial.println(LoRa.packetRssi());


    switch (RTram[0]) {
      case '0' :
        Serial.println("Tram recu, type : Demande d'identifiant");
        Mac_ad = get_Mac(RTram);
        if (chek_if_exist(Mac_ad) == 1) {
          int ind = get_indice(Mac_ad); // Prendre l'indice où l'adr MAC est enregistré pour retrouvé l'ancien ID affecté à ce noeud
          if ( ind != 8 ) {
            Set_Tram(Affect_ID, noeud_ID[ind]);
          }

          else Set_Tram(Affect_ID, 0);
          sendTram(&STram[0]);
          Serial.println("Tram d'identification envoyé.");
          RTram = "";
          break;
        }
        // Si un nouveau noeud, on enregistre l'adresse MAC
        if (nbr_noeud < 8) {
          MAC[nbr_noeud] = Mac_ad;
          noeud_ID[nbr_noeud] = (char)(nbr_noeud + 2);
          nbr_noeud++;
        }
        Set_Tram(Affect_ID, noeud_ID[nbr_noeud - 1]);
        sendTram(&STram[0]);
        Serial.println("Tram d'identification envoyé.");
        RTram = "";
        break;
      case '2' :
        // Traitement d'une demande de sortie du réseau d'un noeud X
        IP_ad = get_ID(RTram);
        Serial.print("Demande de sortie de reseau de la part de : ");
        Serial.println(IP_ad);
        for (int i = 0 ; i < 8 ; i++) {

          if (noeud_ID[i] == IP_ad[7]) { // lorsqu'on trouve l'adresse on suprime data
            noeud_ID[i] == "";
            MAC[i] == "";
            nbr_noeud = i; // on met l'indice i sur nbr_noeud si un autre noeud vient de connecter on l'enregistre sur la case vide
            Serial.print("Noeud suprimer.");

            break; // fin boucle

          }
        }
        RTram = "";
        break;
      case '3' :
        Serial.println("Tram recu, type : Tram de données");
        IP_ad = get_ID(RTram);
        Serial.print("ID de noeud : ");
        Serial.println(IP_ad);
        Data = get_data(RTram);
        
        Serial.print("Temperature : ");
        Serial.println((String)RTram[15] + (String)RTram[16]);
        
        Serial.print("Humidite : ");
        Serial.println((String)RTram[17] + (String)RTram[18]);
        //Serial.println(humidite);

        RTram = "";
        break;
      default :


        break;
    }
  }
}
