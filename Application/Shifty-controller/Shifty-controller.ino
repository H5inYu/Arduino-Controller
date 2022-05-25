/* 
This code modify Shifty to wireless.
For more detail, please refer to the following link
https://ieeexplore.ieee.org/document/7833030
https://github.com/AndreZenner/shifty
*/

#include <ESP8266WiFi.h>

/* WIFI */
const char *ssid =  "";     // replace with your wifi ssid and wpa2 key
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
struct ShiftyStatus{
  int packetLength = 8;
  int cmdID = 41;
  int currentMode;
};

/*Stepper Motor Setup*/
int stepPin  = 4; //D2
int dirPin = 5; //D1
int number_of_steps = 200; //total number of steps for this motor
int motorSpeed = 200;
int last_step_time = 0;
unsigned long step_delay = 60L*1000L /number_of_steps/motorSpeed;

int Shifty_Mode = 0; //0: Tennis, 1: Sword
int motorDistance = 650;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(500000);
  Setup_WIFI();
  Server_Connect();
  Setup_Motor();
}

void loop() {
  if(!client.connected()){
    Server_Connect();
    CleanSerial();
  }
  if (client.connected() && client.available()){
     ReadCommandFromServer();
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
    case 40:
      SwitchMode(command);
      break;
    case 41:
      ReportStatus();
      break;
    case 42:
      SetMode(command);
      break;
    case 43:
      int dir = ByteToInt(command, 4, 8);
      int motorStep = ByteToInt(command, 8, 12);
      Move(dir, motorStep);
      break;
  }
}


void SendWelcomeBack(){
  WelcomeBack* packet = new WelcomeBack();
  String message = "Hello from Shifty";
  message.getBytes(packet->msg, message.length()+1);
  client.write((byte*)packet, sizeof(*packet));
}

void SwitchMode(byte* command){
  int nextMode = ByteToInt(command,4,8);
  if (nextMode == 0){
    TennisMode();
    Shifty_Mode = 0;
    Serial.println("Shifty changes state to tennis");
  }
  else if (nextMode == 1){
    SwordMode();
    Shifty_Mode = 1;
    Serial.println("Shifty changes state to sword");
  }
  ReportStatus();
}
void ReportStatus(){
  ShiftyStatus* packet = new ShiftyStatus();
  packet->currentMode = Shifty_Mode;
  client.write((byte*)packet, sizeof(*packet));
}
void SetMode(byte* command){
  Shifty_Mode = ByteToInt(command,4,8);
}

void Move(int dir, int distance){
  digitalWrite(dirPin, dir);
  for (int i =0; i <= distance; i++){
    digitalWrite(stepPin, HIGH);
    delay(3);
    digitalWrite(stepPin, LOW);
    delay(3);
  }
  Serial.println("Move");
}


void TennisMode(){
  if(Shifty_Mode!= 0){
    Move(1, motorDistance);
  }
}
void SwordMode(){
  if(Shifty_Mode != 1){
    Move(0, motorDistance);
  }
}

void Setup_Motor(){
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);  
}

// Connection 
void Setup_WIFI(){
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
  }
}
void Server_Connect(){
  while(!client){
    client.connect(host, port);
    delay(500);
  }
}
void CleanSerial(){
  while(Serial.read() != -1){}
}
