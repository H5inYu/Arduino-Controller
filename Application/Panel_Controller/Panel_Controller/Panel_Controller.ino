// This code is for communicating from HC NodeMCU to Unity Server

#include <ESP8266WiFi.h>

/* WIFI */
const char *ssid =  "";     
const char *pass =  "";
const char *host = "";
int port = 26950;
WiFiClient client;

/* Command */
struct WelcomeBack{
  int packetLength = 44;
  int cmdID = 1;
  byte msg[40];
};
struct PanelInfo{
  int packetLength = 40;
  int cmdID = 50;
  int redBtn;
  int blueBtn;
  int slider0;
  int slider1;
  int slider2;
  int slider3;
  int x;
  int y;
  int degree;
};
String cmdFromServer;
int sendMode = 1;
byte panelInformation[36];
float timeAcc = 0;
int resetPin = D3;
void setup() {
  Serial.begin(500000);
  Setup_WIFI();
  Server_Connect();
  memset(panelInformation, 0, sizeof(panelInformation));
  pinMode(resetPin, OUTPUT);
  ResetMega2560();
}

void loop() {
  if(!client.connected()){
    Server_Connect();
    CleanSerial();
    memset(panelInformation, 0, sizeof(panelInformation));
  }
  if (client.connected() && client.available()){
     ReadCommandFromServer();
  }
  if(Serial.available()){
    if (Serial.read() == '\n'){
      Serial.readBytes(panelInformation, 36);
    }
  }
  if(sendMode == 1 && (millis()-timeAcc >100)){
    ReportPanelInfo();
    timeAcc = millis();
  }
}
void ReportPanelInfo(){
  PanelInfo* packet = new PanelInfo();
  packet->redBtn = ByteToInt(panelInformation, 0,4);
  packet->blueBtn = ByteToInt(panelInformation, 4,8);
  packet->slider0 = ByteToInt(panelInformation, 8,12);
  packet->slider1 = ByteToInt(panelInformation, 12,16);
  packet->slider2 = ByteToInt(panelInformation, 16,20);
  packet->slider3 = ByteToInt(panelInformation, 20,24);
  packet->x = ByteToInt(panelInformation, 24,28);
  packet->y = ByteToInt(panelInformation, 28,32);
  packet->degree = ByteToInt(panelInformation, 32,36);
  client.write((byte*)packet, sizeof(*packet)); 
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
    case 51:
      SendModeOnOff(command);
      break;
  }
}

void SendWelcomeBack(){
  WelcomeBack* packet = new WelcomeBack();
  String message = "Hello from Panel";
  message.getBytes(packet->msg, message.length()+1);
  client.write((byte*)packet, sizeof(*packet));
}

void SendModeOnOff(byte* command){
  ResetMega2560();
  sendMode = ByteToInt(command, 4, 8);
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
void ResetMega2560(){
  digitalWrite(resetPin, LOW);
  delay(500);
  digitalWrite(resetPin, HIGH);
}
