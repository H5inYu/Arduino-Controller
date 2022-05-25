// This code is for communicating from HC NodeMCU to Unity Server

#include <ESP8266WiFi.h>

/* WIFI */
const char *ssid =  "";     
const char *pass =  "";
const char *host = "";
int port = 26950;
WiFiClient client;
bool isExecuting = false;

/* Command */
struct WelcomeBack{
  int packetLength = 44;
  int cmdID = 1;
  byte msg[40];
};
struct ElevatorStatus{
  int packetLength = 24;
  int cmdID = 7;
  int supervisor;
  int station0;
  int station1;
  int station2;
  int station3;
};
struct ExecuteDone{
  int packetLength = 4;
  int cmdID = 20;
};

byte stationStatus[16];
byte readyStatus[16];


void setup() {
  Serial.begin(500000);
  Setup_WIFI();
  Server_Connect();
  memset(stationStatus, 0, 16);
  memset(readyStatus, 2, 16);
}

void loop() {
  if(!client.connected()){
    Server_Connect();
    CleanSerial();
    memset(stationStatus, 0, 16);
  }
  if (client.connected() && client.available()){
     ReadCommandFromServer();
  }
  if(Serial.available()){
    if(Serial.read() == '\n'){
      Serial.readBytes(stationStatus, 16);
    }
  }
  if(isExecuting && memcmp(stationStatus, readyStatus, 16) == 0){
      SendExecutionDone();
    }
}

void ReadCommandFromServer(){  
  byte packetLengthBytes[4];
  for (int i = 0; i < sizeof(packetLengthBytes);i++){
    packetLengthBytes[i] = client.read();
  }
  int packetLength = ByteToInt(packetLengthBytes);
  byte command[packetLength];
  for (int i = 0; i < packetLength; i++){
    command[i] = client.read();
  }
  int cmdID = ByteToInt(command,0,4);
  
  switch(cmdID){
    case 1:
      SendWelcomeBack();
      break;
    case 5:
      TransferCommand2Mega(command);
      break;
    case 7:
      RequestCraneMovingState(command);
      break;
    
  }
}

void SendWelcomeBack(){
  WelcomeBack* packet = new WelcomeBack();
  String message = "Hello from HC";
  message.getBytes(packet->msg, message.length()+1);
  client.write((byte*)packet, sizeof(*packet));
}
void SendExecutionDone(){
  ExecuteDone* packet = new ExecuteDone();
  client.write((byte*)packet, sizeof(*packet));
}
void RequestCraneMovingState(byte* command){
  ElevatorStatus* packet = new ElevatorStatus();
  packet->supervisor = ByteToInt(command, 4, 8);
  packet->station0 = ByteToInt(stationStatus, 0, 4);
  packet->station1 = ByteToInt(stationStatus, 4, 8);
  packet->station2 = ByteToInt(stationStatus, 8, 12);
  packet->station3 = ByteToInt(stationStatus, 12, 16);
  client.write((byte*)packet, sizeof(*packet));
}
void ElevatorMove(byte* command){
  int supervisor = ByteToInt(command, 4, 8);
  int station = ByteToInt(command, 8, 12);
  int movement = ByteToInt(command, 12, 16);
  String moveCode = "";
  switch (movement){
    case 0:
      moveCode = "s";
    break;
    case 1:
      moveCode = "S";
    break;
    case 2:
      moveCode = "u";
    break;
    case 3:
      moveCode = "d";
    break;
    case 4:
      moveCode = "o";
    break;
    case 5:
      moveCode = "f";
    break;
    case 6:
      moveCode = "l";
    break;
    case 7:
      moveCode = "p";
    break;
    case 8:
      moveCode = "r";
    break;
    
  }
  Serial.print(String(station) + moveCode + '\n');
//  delay(1000);
}
void TransferCommand2Mega(byte* command){
  Serial.print('\n');
  Serial.write(command+4, 40);
  memset(stationStatus, 0, 16);
}


// Connection 
void Setup_WIFI(){
//  Serial.println("Connecting to ");
//  Serial.println(ssid); 
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
//    Serial.print(".");
  }
//  Serial.println("");
//  Serial.println("WiFi connected");
  
}

void Server_Connect(){
  while(!client){
    client.connect(host, port);
//    Serial.print(".");
    delay(500);
  }
//  Serial.println("Client connects successfully.");
}
void CleanSerial(){
  while(Serial.read() != -1){}
}
