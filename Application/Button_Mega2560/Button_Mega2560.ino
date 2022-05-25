int const start = 22;
int const offset = 15;
int curButtonState;
int lastButtonState[offset];
void setup(){
  Serial.begin(500000);
  SetupPins();
  SetupButtonState();
}

void loop(){
  ReadFromButtons();
  delay(10);
}

void ReadFromButtons(){
  for (int i = start ; i < start+offset; i++){
    curButtonState = digitalRead(i);
    if (curButtonState == HIGH && lastButtonState[i-start] == 0){
      Serial.print(String(i-start)+"\n");
    }
    lastButtonState[i-start] = curButtonState;
  } 
  
}

void SetupPins(){
  for (int i = start; i < start+offset; i++){
    pinMode(i, INPUT_PULLUP);
  }
}
void SetupButtonState(){
  memset(lastButtonState, 1, offset);
}
