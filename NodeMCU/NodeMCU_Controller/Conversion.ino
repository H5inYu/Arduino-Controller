int ByteToInt(byte* byteArray){
  int value = 0;
  int offset = 8;
  for (int i = 0; i < sizeof(byteArray); i++){
      value = ((int)byteArray[i]) << (offset*i) | value;
  }
    return value;
}

int ByteToInt(byte* byteArray, int _start, int _stop){
  int value = 0;
  int offset = 8;
  for (int i = _start; i < _stop; i++){
      value = ((int)byteArray[i]) << (offset*(i-_start)) | value;
  }
    return value;
}


String ByteToString(byte* byteArray, int _start, int _stop){
  String msg = "";
  int index = 0;
  for (int i = _start; i < _stop; i++){
    msg += (char)byteArray[i];
  }
  return msg;
}
