#include <SPI.h>
#include <LoRa.h>

//int sw = 0x12 ;
int sf = 7, index;
bool found;
char typeT;
String gateway = "001", node, trame="", data, cmd;
String availableId[8] = {"xx","xx","xx","xx","xx","xx","xx","xx"}; // pour stocker les ID
String nodes[8] = {"xx","xx","xx","xx","xx","xx","xx","xx"}; // stocker le numero de noeud
                                                              //correspond à chaque ID

String chooseNode(){
  int c = 0;
  for(int i = 0 ; i < 8 ; i++){
    if(nodes[i] != "xx"){
      c++; // compter le nombre de noeuds connectées
      found = true;
    }
  }
  if (c == 0) found = false; // il y a pas de noeud
  index = random(0,c); // choisir un noeud par hasard
  return nodes[index];
}

void sendPayload(){
  trame = "";
  trame = giveId() + node + gateway + typeT;
  LoRa.beginPacket();
  LoRa.print(trame);
  LoRa.endPacket();
}

bool checkNode(){
  for(int i = 0 ; i < 8 ; i++){
    if(nodes[i] == node){
      index = i;
      found = true;
      break;
    }
    else found = false;
  } return found;
}

String giveId(){
  for(int i = 0 ; i < 8 ; i++){
    if(availableId[i] == "xx" && !checkNode()){
      nodes[i] = node;
      return availableId[i] = "0" + String(i);
    } else if (checkNode()) return availableId[index];
  }
}


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(863.4E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
    
  }
    LoRa.setSyncWord(0x12);
    LoRa.setSpreadingFactor(sf); 
}

void loop() {
  trame = "";
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    trame = "";
    while (LoRa.available()) {
      trame = trame + (char)LoRa.read(); // recevoir le trame
    }}

    switch (trame[trame.length()-1]) {
      case '0':
        typeT = '1';
        node = String(trame[trame.length()-4]) + String(trame[trame.length()-3]) 
            + String(trame[trame.length()-2]);
        
        Serial.print("trame recu : "); Serial.print(trame); Serial.print(" de noeud : "); 
        Serial.println(node);
        sendPayload();
        Serial.print("trame identifiant envoyée : "); Serial.println(trame);
      break;

      case '2':
        node = String(trame[trame.length()-4]) + String(trame[trame.length()-3]) 
            + String(trame[trame.length()-2]);
        if(checkNode()){
          availableId[index] = "xx";
          nodes[index] = "xx";
          Serial.print("noeud supprimé : ");Serial.println(node);
        }
      break;

      case '3':
        node = String(trame[trame.length()-4]) + String(trame[trame.length()-3]) 
              + String(trame[trame.length()-2]);
              
        data = String(trame[trame.length()-9]) + String(trame[trame.length()-8]);
        //Serial.println(node);
        if(node == "007") {Serial.print("Temperature recue : ");Serial.println(data);}
        if(node == "002") {Serial.print("Humidité recue : ");Serial.println(data);}
      break;
      
      default:
      break;
    }    
    
    int proba = random(0,100000);
    chooseNode();
    if (proba == 7 && found){
      cmd = String(random(0,2));
      typeT = '4';
      trame = "";
      trame = cmd + chooseNode() + gateway + typeT;
      LoRa.beginPacket();
      LoRa.print(trame);
      LoRa.endPacket();
      Serial.print("trame commande envoyée : "); Serial.println(trame);
      trame = "";
    }
    //Serial.println();
    // print RSSI of packet
    //Serial.print("' with RSSI ");
    //Serial.println(LoRa.packetRssi());
}
