#include "HC_StationClass.h"
Station station0("Station 0", 2,  3,   4, 22, 30, 31); //name, in1, in2, pwm, Em, LSU, LSD, 
Station station1("Station 1", 5,  6,   7, 24, 32, 33);
Station station2("Station 2", 8,  9,  10, 26, 34, 35);
Station station3("Station 3", 11, 12, 13, 28, 36, 37); 
Station stationList[] = {station0, station1, station2, station3};

float elevatorWait[4];

byte lastStationStatus[16];
byte stationStatus[16];
byte ServerCommand[36];
float timeAcc;
float craneOperationTime;
void setup() {
  Serial.begin(500000);   //monitor
  Serial1.begin(500000); //data from esp8266
  timeAcc = millis();
  memset(stationStatus, 0, 16);
  memset(ServerCommand, 0, 36);
}

void loop() {
  ReadCommandFromNodeMCU();
  ReadCommandFromSerialPort();
  MonitorOperation();
  ReportStatus();
}

void ReadCommandFromNodeMCU(){
  if(Serial1.available()){
    if(Serial1.read() == '\n'){
      Serial1.readBytes(ServerCommand, 36);
      ExecuteCommand();
      memset(ServerCommand, 0, 36);
    }
  }
}
void ReadCommandFromSerialPort(){
  if(Serial.available()){
    craneOperationTime = millis();
    String cmd = Serial.readStringUntil('\n');
    Serial.println("The command = " + cmd);
    Serial.println("The length of command = " + String(cmd.length()));
    int OperationCategory = (int)cmd[0]-48;
    if ((cmd.length()-2)%3 == 0){
      int stationCount = (cmd.length()-1)/3;
      for (int i=0; i<stationCount; i++){
        int station_number = (int)cmd[1+3*i]-48;
        int Op_ID = (int)cmd[2+3*i]-48;
        int value = (int)cmd[3+3*i]-48;
        Serial.println(String(station_number) + String(Op_ID)+ String(value));
        switch(OperationCategory){
          case 0:
            stationList[station_number].Exe_Movement(Op_ID, value);
            break;
          case 1:
            stationList[station_number].Setting(Op_ID, value);
            cmd = "";
            break;
        }
      } 
    }
  }
}
void ExecuteCommand(){
  int op_category = ByteToInt(ServerCommand, 0, 4);
  int op_id = 0;
  int value = 0;
  for(int i=0; i<4; i++){
    op_id = ByteToInt(ServerCommand, i*8+4, i*8+8);
    value = ByteToInt(ServerCommand, i*8+8, i*8+12);
    switch(op_category){
      case 0://movement
        stationList[i].Exe_Movement(op_id, value);
        break;
      case 1:
        stationList[i].Setting(op_id, value);
        break;
    }
  }
}

void ReportStatus(){
  if(memcmp(stationStatus, lastStationStatus, 16) != 0){
    Serial1.print('\n');
    Serial1.write(stationStatus, sizeof(stationStatus));
    memcpy(lastStationStatus, stationStatus, 16);
  }
//  Serial.println("");
}

void MonitorOperation(){
  for (int i = 0; i<4; i++){
    int operationState = stationList[i].GetOperationState();
    int movingState =  stationList[i].GetMovingState();
    byte state[4];
    IntToByte(state, movingState);
    for (int j = i*4; j < i*4+4; j++){
      stationStatus[j] = state[j-i*4];
    }
    switch (operationState){
      case 0:
        if (movingState == 1){ // Up
          int check = 0;
          check = digitalRead(30+2*i);
          if (!check){
            stationList[i].Exe_Movement(3, 0);
          }
        }
        else if (movingState == -1){ // Down
          int check = 0;
          check = digitalRead(31+2*i);
          if (!check){
            stationList[i].Exe_Movement(4, 0);
          }
        }
        break;
      case 1://lift
        if (movingState == 1){ // Up
          int check = 0;
          check = digitalRead(30+2*i);
          if (!check){
            stationList[i].Exe_Movement(3, 0);
            stationList[i].Setting(3,0); //Set operation status = 0
            float duration = millis()-craneOperationTime;
            Serial.print("lift time" + String(duration));
          }
        }
        else if (movingState == -1){ // Down
          int check = 0;
          check = digitalRead(31+2*i);
          if (!check){
            stationList[i].Exe_Movement(4, 0);
            elevatorWait[i] = millis();
          }
        }
        else if (movingState == -2){ //bottom
          stationList[i].Exe_Movement(5, 0);
          if(millis()-elevatorWait[i] > 500){
            stationList[i].Setting(1, 1); //Set crane status to loaded = 1
            stationList[i].Exe_Movement(1, 0);
          }
        }
        break;
      case 2://place
        if (movingState == 1){ // Up
          int check = 0;
          check = digitalRead(30+2*i);
          if (!check){
            stationList[i].Exe_Movement(3, 0);
            stationList[i].Setting(3,0); //Set operation status = 0
            float duration = millis()-craneOperationTime;
            Serial.print("lift time" + String(duration));
          }
        }
        else if (movingState == -1){ // Down
          int check = 0;
          check = digitalRead(31+2*i);
          if (!check){
            stationList[i].Exe_Movement(4, 0);
            elevatorWait[i] = millis();
          }
        }
        else if (movingState == -2){
          stationList[i].Exe_Movement(6, 0);
          if(millis()-elevatorWait[i] > 500){
            stationList[i].Setting(1, 0); //Set crane status to unloaded = 0
            stationList[i].Exe_Movement(1, 0);
          }
        }
      break;
    }
  }
}
