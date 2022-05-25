int redButton = 23; // red buton signal
int blueButton = 22; // bule button signal
int slider0 = A0;
int slider1 = A1;
int slider2 = A2;
int slider3 = A3;
int xPos = A4;
int yPos = A5;
int encoderCLK = 3;
int encoderDT = 4;
int encoderSW = 5;
int encoderState;
int encoderLastState;

byte redState[4];
byte blueState[4];
byte slider0State[4];
byte slider1State[4];
byte slider2State[4];
byte slider3State[4];
byte xState[4];
byte yState[4];
byte degreeState[4];

int degree=0;
byte lastPanelInformation[36];
byte panelInformation[36];
float timeAcc = 0;
bool sendMode = true;
void setup(){
  Serial.begin(500000);
  Serial1.begin(500000);
  SetupPins();
}

void loop(){
  ReadFromButtons();
  ReadFromSliders();
  ReadFromJoyStick();
  ReadFromEncoder();
  UpdateStatus();
  if(millis()-timeAcc > 100){//different
    Serial1.write('\n');
    Serial1.write(panelInformation, sizeof(panelInformation));
    PrintPanelInfo();
    timeAcc = millis();
  }
  delay(100);
}

void ReadFromButtons(){
  int red = digitalRead(redButton);
  if(red == HIGH){
    IntToByte(redState, 0);
  }
  else IntToByte(redState, 1);
  int blue = digitalRead(blueButton);
  if(blue ==HIGH){
    IntToByte(blueState, 0);
  }
  else IntToByte(blueState, 1);;
}
void ReadFromSliders(){
  IntToByte(slider0State, map(analogRead(slider0), 0, 1023, 9, 0));
  IntToByte(slider1State, map(analogRead(slider1), 0, 1023, 9, 0));
  IntToByte(slider2State, map(analogRead(slider2), 0, 1023, 9, 0));
  IntToByte(slider3State, map(analogRead(slider3), 0, 1023, 9, 0));
}
void ReadFromJoyStick(){
  IntToByte(xState, map(analogRead(xPos),0, 1023, 1023, 0));
  IntToByte(yState, map(analogRead(yPos),0, 1023, 1023, 0));
}
void ReadFromEncoder(){
  encoderState = digitalRead(encoderCLK);
  if(encoderState != encoderLastState){
//    Serial.println("last state = "+ String(encoderLastState));
//    Serial.println("current state = "+ String(encoderState));
    int DT = digitalRead(encoderDT);
//    Serial.println("DT = " +String(DT));1111
    
    if(DT != encoderState){
      degree= (degree+40+1)%40;
    }else{
      degree = (degree+40-1)%40;
    }
    IntToByte(degreeState, degree);
  }
  encoderLastState = encoderState;
}
void UpdateStatus(){
  memcpy(&panelInformation[0], redState, sizeof(redState));
  memcpy(&panelInformation[4], blueState, sizeof(blueState));
  memcpy(&panelInformation[8], slider0State, sizeof(slider0State));
  memcpy(&panelInformation[12], slider1State, sizeof(slider1State));
  memcpy(&panelInformation[16], slider2State, sizeof(slider2State));
  memcpy(&panelInformation[20], slider3State, sizeof(slider3State));
  memcpy(&panelInformation[24], xState, sizeof(xState));
  memcpy(&panelInformation[28], yState, sizeof(yState));
  memcpy(&panelInformation[32], degreeState, sizeof(degreeState));
}

void PrintPanelInfo(){
  int a = ByteToInt(panelInformation,0,4);
  int b = ByteToInt(panelInformation,4,8);
  int c = ByteToInt(panelInformation,8,12);
  int d = ByteToInt(panelInformation,12,16);
  int e = ByteToInt(panelInformation,16,20);
  int f = ByteToInt(panelInformation,20,24);
  int g = ByteToInt(panelInformation,24,28);
  int h = ByteToInt(panelInformation,28,32);
  int i = ByteToInt(panelInformation,32,36);
  Serial.println(String(a)+" "+String(b)+" "+String(c)+" "+String(d)+" "+String(e)+" "+String(f)+" "+String(g)+" "+String(h)+" "+String(i));
}


void SetupPins(){
  pinMode(redButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  digitalWrite(redButton, HIGH);
  digitalWrite(blueButton, HIGH);
  pinMode(slider0, INPUT);
  pinMode(slider1, INPUT);
  pinMode(slider2, INPUT);
  pinMode(slider3, INPUT);

  pinMode(xPos, INPUT);
  pinMode(yPos, INPUT);

  pinMode(encoderCLK, INPUT);
  pinMode(encoderDT, INPUT);
  pinMode(encoderSW,INPUT);
  encoderLastState = digitalRead(encoderCLK);
  memset(panelInformation,0,36);
  memset(lastPanelInformation, 0, 36);
}
