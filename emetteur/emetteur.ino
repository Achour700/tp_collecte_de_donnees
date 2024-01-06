/* Programme d'un noeud emetteur dont l'adresse est affecté par le gateway
* Notez bien que le dernier valeur dans la Tram doit etre 0xFF
*/
#define sizeofdata 247 // maximum d'octets de données
#define sizeoftram 255
#define Demand_ID 0 // Tram de demande d'ID
#define Out_of_Net 2 // Tram informant la sortie du noeud
#define Send_data 3 // Tram d'envoi de donnees
//#include <SPI.h>
#include <LoRa.h>

volatile unsigned char data[sizeofdata];
volatile unsigned char STram[sizeoftram]; // Sending Tram - pour faciliter la reconstitution
String Rtram = ""; // Receiving Tram - sous forme de string pour faciliter le traitement de Tram
volatile unsigned char local_ID[3] = {0, 0, 1}; // Adress local du noeud / Adress MAC
unsigned char dest_ID[3] = {192, 168, 1}; // Adress de destinataire - Collecteur
String ID[3]={"","",""}; // Adress IP / Affected adress
int id;
// Fonction qui permet d'envoyer une tram
void sendTram(unsigned char *Tram){
unsigned int i = 0;
LoRa.beginPacket();
while(Tram[i] != 0xFF){
LoRa.print(Tram[i]);
i++;
}
LoRa.endPacket();
}
void setup() {
// Initialisation de port série & LoRa
Serial.begin(9600);
while(!Serial);
Serial.println("Test begin :");
if(!LoRa.begin(866.7E6)){
Serial.println("Starting LoRa failed!");
while(1);
}
Serial.println("Demande d'identification.");
// Constituant la Tram de demande d'ID
// Tram = data + Destination_adresse (192.168.1) + Local_ID (0.0.0) + Fonction
STram[0] = Demand_ID; // Fonction



STram[1] = local_ID[0]; // Troisième octet d'adresse local = 0 (pas encore affecté)
STram[2] = local_ID[1]; // Deuxième octet d'adresse local = 0 (pas encore affecté)
STram[3] = local_ID[2]; // Premier octet d'adresse local = 0 (pas encore affecté)
STram[4] = dest_ID[0]; // Troisième octet d'adresse destination = 192
STram[5] = dest_ID[1]; // Deuxième octet d'adresse destination = 168
STram[6] = dest_ID[2]; // Premier octet d'adresse destination = 1
STram[7] = 0xFF; // 0xFF pour indiqué la fin de la Tram / necessaire au niveau gateway
// Appel à la fonction sendTram pour envoyer la tram
sendTram(&STram[0]);
// Recevoir La Tram d'Identifiant
int packetSize = LoRa.parsePacket();
while (!LoRa.available()); // Attend dans la boucle jusqu'a arriver d'une tram
Serial.print("Received packet '");
// read packet
while (LoRa.available()) {
//Serial.print((char)LoRa.read());
Rtram += (char)LoRa.read();
}
Serial.print(Rtram);
int sizeofRT = Rtram.length();
// print RSSI of packet
Serial.print("' with RSSI ");
Serial.println(LoRa.packetRssi());
ID[0] = String(Rtram[12]) + String(Rtram[13]) + String(Rtram[14]); // 192
ID[1] = String(Rtram[9]) + String(Rtram[10]) + String(Rtram[11]); // 168
ID[2] = String(Rtram[8]); // Numéro affecté
Serial.print("Adresse IP reçu est : ");
Serial.print(ID[0]);
Serial.print(".");
Serial.print(ID[1]);
Serial.print(".");
Serial.println((char)ID[2][0]);
// Retrouvons l'ID sous fourme d'entier pour évité le problème d'affichage en reception (ex. ID = 2 => ID_reçu = 50)
for(int i = 2; i < 10; i++){
if(ID[2] == (String)i) {
id = i;
Serial.print("ID : ");
Serial.println(id);
break;
}
}
// Fixation de la première partie de la tram (Fonction + adresse_IP + adresse de destination)
STram[0] = Send_data; // Fonction
STram[1] = 192; // Troisième octet d'adresse local


STram[2] = 168; // Deuxième octet d'adresse local
STram[3] = id; // Premier octet d'adresse local : identifiant affecté
STram[4] = dest_ID[0]; // Troisième octet d'adresse destination = 192
STram[5] = dest_ID[1]; // Deuxième octet d'adresse destination = 168
STram[6] = dest_ID[2]; // Premier octet d'adresse destination = 1
// à partir de l'indice 7 on peut mettre des données + la fin de tram est 0xFF
Serial.println("Temperature:,Humidity:"); // Plot labels
}
void loop() {
  int out = 0;
  for (int i = 0; i < 3; i++) { // envoyer trois mesures de température puis demande de sortie
    // Générer des valeurs random de température et d'humidité
    float temperature = random(0, 10);
    float humidity = random(60, 90);

    Serial.print(temperature);
    Serial.print(" ");
    Serial.println(humidity);

    // Mettre les valeurs dans la trame
    STram[7] = temperature;
    STram[8] = humidity;

    // Envoyer la trame
    sendTram(&STram[0]);

    out++;
    delay(3000);
  }

  if (out == 103) { // après trois mesures, envoyer le noeud demande de sortir du réseau
    // Mettre la fonction de la trame à 3 (sortie du réseau)
    STram[0] = Out_of_Net;

    // Envoyer la trame
    sendTram(&STram[0]);

    Serial.println("Sorti du réseau.");
    while (1);
  }
}
