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
void IntToByte(byte b[4], int val)
{
  b[3] = (byte )((val >> 24) & 0xff);
  b[2] = (byte )((val >> 16) & 0xff);
  b[1] = (byte )((val >> 8) & 0xff);
  b[0] = (byte )((val) & 0xff);
}
