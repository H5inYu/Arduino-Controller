#include <ESP8266WiFi.h>

/* WIFI Setting*/
const char *ssid =  "Your SSID";     
const char *pass =  "Your Password";
const char *host = "Your host ip"; 
int port = 80;
WiFiClient client;

/* WIFI Connection */ 
void Setup_WIFI(){
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) delay(500);
}

/* Server Connection */ 
void Server_Connect(){
  while(!client){
    client.connect(host, port);
    delay(500);
  }
}

/* Utils */ 
void CleanSerial(){
  while(Serial.read() != -1){}
}

/* Define command for communicating with server*/
struct PacketTemplate{
  int packetLength = 44; // change this 
  int cmdID = 1; //define this
  char msg[40];
  // add the other data
};

struct PacketExample{
  int packetLength = 8;
  int cmdID = 7;
  int supervisor;
};

/* Parse command from server*/
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
      SendWelcomeBack(command);
      break;
  }
}

/* Define handling function */
void SendWelcomeBack(byte* command){
  WelcomeBack* packet = new WelcomeBack();
  String message = "Hello from client";
  message.getBytes(packet->msg, message.length()+1);
  client.write((byte*)packet, sizeof(*packet));
}


/* Arduino setup and loop */
void setup() {
  Serial.begin(500000);
  Setup_WIFI();
  Server_Connect();
}

void loop() {
  if(!client.connected()){
    Server_Connect();
    CleanSerial();
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
