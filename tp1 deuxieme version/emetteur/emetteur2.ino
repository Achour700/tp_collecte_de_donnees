#include<SPI.h>
#include<LoRa.h>


int sf = 7; // Spreading Factor
long frequency = 863.4E6; // Fréquence utilisée pour communiquer entre les cartes

bool noID = true;
String gateway = "001", nodeNumber = "007", requestId = "", cmd, trame, id;
char typeT;
int data;

// fonction pour envoyer les données
void sendData(){
    typeT = '3';
    trame = String(data) + gateway + nodeNumber + typeT;
   // Envoi des données
    LoRa.beginPacket();
    LoRa.print(trame);
    LoRa.endPacket();
    Serial.print("trame de données envoyée : "); Serial.println(trame);
    
    delay(4000);
}

void setup() {

  Serial.begin(9600);
  while(!Serial);
  
  if(!LoRa.begin(863.4E6)){
    Serial.println("Strating LoRa failed!");
    while(1);
  }

  // configuration de notre LoRa
  LoRa.setSyncWord(0x12);
  LoRa.setSpreadingFactor(sf); 

  // Envoi d'un trame au Geteway pour demander l'identifiant
  LoRa.beginPacket();
  LoRa.print(gateway+nodeNumber+'0');
  LoRa.endPacket();
  
  Serial.print("Trame demande identifiant : ");
  Serial.println(gateway+nodeNumber+'0');

  while(noID){ // on ne sort pas de de la boucle tant que l'ID n'est pas recu.
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
  
      requestId = "";
      while(LoRa.available()){
      requestId = requestId + (char)LoRa.read();     
      }
      Serial.print("trame recu : "); Serial.println(requestId);
    }
    
    delay(500);

    if(String(requestId[requestId.length()-4]) + String(requestId[requestId.length()-3]) 
      + String(requestId[requestId.length()-2]) == gateway && requestId[requestId.length()-1] == '1'){

    id = String(requestId[requestId.length()-9]) + String(requestId[requestId.length()-8]);
    noID = false;
    }
  
  }
  
  Serial.print("identifiant attribué : "); Serial.println(id);
}

void loop() {
  int packetSize = LoRa.parsePacket();
    if (packetSize) {
  
      trame = "";
      while(LoRa.available()){
      trame = trame + (char)LoRa.read();     
      }
      Serial.print("trame recu : "); Serial.println(trame);
    }

    if(String(trame[trame.length()-7]) + String(trame[trame.length()-6]) 
      + String(trame[trame.length()-5]) == nodeNumber && String(trame[trame.length()-4]) + String(trame[trame.length()-3]) 
      + String(trame[trame.length()-2]) == gateway && trame[trame.length()-1] == '4'){
      cmd = String(trame[trame.length()-8]);
    }

  data = random(11, 45);
  
  if(cmd == "0"){
    sendData();
  }

  if(cmd == "1"){
    if(data > 20){
      sendData();
      delay(10);
    }
  }
}
