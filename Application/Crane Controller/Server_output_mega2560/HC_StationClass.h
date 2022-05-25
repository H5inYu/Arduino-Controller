#ifndef HC_StationClass_h
#define HC_StationClass_h

#include "Arduino.h"

class Station
{
public:
  Station(String name, int in1, int in2, int PWMpin, int EM, int LSU, int LSD);
  void Exe_Movement(int operationID, int value);
  void Setting(int operationID, int value);
  int Getting(int operationID);
  void Retrieve();
  void Allocate();
  
  void GOUP();
  void GODOWN();
  void UPSTOP();
  void DOWNSTOP();
  void ON();
  void OFF();
  
  int GetCraneState();
  void SetCraneState(int state);
  int GetOperationState();
  void SetOperationState(int state);
  int GetMovingState();
  void SetMovingState(int state);
private:
  String _name;
  int _in1;
  int _in2;
  int _PWMpin;
  int _EM;
  int _LSU;
  int _LSD;
  int _isLoaded;
  bool _EM_Switch;
  int unloadedUpSpeed;
  int unloadedDownSpeed;
  int unloadedStop;
  int loadedUpSpeed;
  int loadedDownSpeed;
  int loadedStop;
  int downSpeed;
  int upSpeed;
  int staySpeed;
  int counter;
  bool check;
  int duration;
  int _movingState; // 0 = stop, 1 = up, -1 = down;
  int _operationState;
};

#endif
