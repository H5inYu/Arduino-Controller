#include "Arduino.h"
#include "HC_StationClass.h"
/*
 * Operation table
 * 0: crane "goes up"
 * 1: crane "goes down"
 * 2: crane "stop at top"
 * 3: crane "stop at bottom"
 * 4: Electromagnet turns "on"
 * 5: Electromagnet turns "off"
 * 6: crane "retrieves" device
 * 7: crane "allocates" device
 * 8: set crane status => isLoaded or not
 * 9: 
 */
Station::Station(String name, int in1, int in2, int PWMpin, int EM, int LSU, int LSD)
{
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(PWMpin, OUTPUT);
  pinMode(EM, OUTPUT);
  pinMode(LSU, INPUT);
  pinMode(LSD, INPUT);

  _name = name;
  _in1 = in1;
  _in2 = in2;
  _PWMpin = PWMpin;
  _EM = EM;
  _LSU = LSU;
  _LSD = LSD;
  _EM_Switch = false;
  unloadedUpSpeed = 70;
  unloadedDownSpeed = 30;
  unloadedStop = 0;
  loadedUpSpeed = 100;
  loadedDownSpeed = 10;
  loadedStop = 10;
  
  _operationState = 0;// 0: onoe, 1 Lift, 2 Place
  _movingState = 0; // 1 go up, -1 go down
}

void Station::GOUP() {
  if(_movingState < 1){
    Serial.println(_name + ": up"); 
    _movingState = 1;
    if(_isLoaded){
      analogWrite(_PWMpin, loadedUpSpeed);
    }
    else{
      analogWrite(_PWMpin, unloadedUpSpeed);
    }
    digitalWrite(_in1, HIGH);
    digitalWrite(_in2, LOW);
  }
  
  // Serial.println(check);
}
void Station::GODOWN() {
  if(_movingState > 0){
    Serial.println(_name + ": down");
    _movingState = -1;
    if(_isLoaded){
      analogWrite(_PWMpin, loadedDownSpeed);
    }
    else{
      analogWrite(_PWMpin, unloadedDownSpeed);
    }
    digitalWrite(_in1, LOW);
    digitalWrite(_in2, HIGH);
  }
}
void Station::UPSTOP(){
  Serial.println(_name + ": up stop");
  _movingState = 2;
  if(_isLoaded){
    analogWrite(_PWMpin, loadedStop);
  }
  else{
    analogWrite(_PWMpin, unloadedStop);
  }
  
  digitalWrite(_in1, HIGH);
  digitalWrite(_in2, LOW);
  
}
void Station::DOWNSTOP(){
  Serial.println(_name + ": down stop");
  _movingState = -2;
  digitalWrite(_in1, LOW);
  digitalWrite(_in2, LOW);
  
}
void Station::ON() {
  Serial.println(_name + ": ON");
  digitalWrite(_EM, HIGH);
  _EM_Switch = true;
}
void Station::OFF() {
  Serial.println(_name + ": OFF");
  digitalWrite(_EM, LOW);
  _EM_Switch = false;
}
void Station::Retrieve(){
  _operationState = 1;
  Serial.println(_name + ": lift _operationState = "+ String(_operationState));
  GODOWN();
}
void Station::Allocate(){
  _operationState = 2;
  Serial.println(_name + ": place _operationState = "+ String(_operationState));
  GODOWN();
}
void Station::Exe_Movement(int operationID, int value)
{
  switch (operationID) 
  {
    case 1: // Go UP
      GOUP();
      break;
    case 2: // Go Down
      GODOWN();
      break;
    case 3: // Up Stop
      UPSTOP();
      break;
    case 4: // Down Stop 
      DOWNSTOP();
      break;
    case 5: // on
      ON();
      break;
    case 6: // off
      OFF();
      break;
    case 7: // retrieve
      Retrieve();
      break;
    case 8: // allocate
      Allocate();
      break;
  }
}
void Station::Setting(int operationID, int value){
  switch(operationID){
    case 1: // set crane isLoaded or not
      _isLoaded = value;
      break;
    case 2:
      _movingState = value;
      break;
    case 3:
      _operationState = value;
      break;
  }
}

int Station::Getting(int operationID){
  int result = 0;
  switch(operationID){
    case 1: // get crane status
      result = _isLoaded;
      break;
    case 2: // get moving status
      result = _movingState;
      break;
    case 3:
      result = _operationState;
      break;      
  }
  return result;
}
int Station::GetCraneState(){
  return _isLoaded;
}
void Station::SetCraneState(int state){
  _isLoaded = state;
}
int Station::GetMovingState(){
  //Serial.print(movingStatus + '\n');
  return _movingState;
}
void Station::SetMovingState(int state){
  _movingState = state;
}
int Station::GetOperationState(){
  return _operationState;
}
void Station::SetOperationState(int state){
  _operationState = state;
}
