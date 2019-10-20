
#define SOFTREG             0x07                    // Byte to read software
#define CMDBYTE             0x00                    // Command byte
#define SPEEDBYTE           0x02                    // Byte to write to speed register
#define CURRENTREG          0x05                    // Byte to read motor current

//motor: 0 = Left, 1 = Right
void setMotor(int velocity, bool motor){
  byte motorAddress = NULL;
  if (motor){
    motorAddress = 0x58;
  } else {
    motorAddress = 0x59;
  }
  Serial.println("setting motor");
  Serial.println(motorAddress);
  Serial.println(abs(velocity));
  sendData(SPEEDBYTE, abs(velocity), motorAddress);             // Sets speed to i
  if (velocity < 0){
    sendData(CMDBYTE, 2, motorAddress);          // Sets motor to direction, A value of 1 runs the motor forward and 2 runs backward
  } else {
    sendData(CMDBYTE, 1, motorAddress);
  }
  Serial.println(getData(CURRENTREG,motorAddress));
}

byte getData(byte reg, byte address){    
// function for getting data from MD03
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(address, 1);
  for( int i = 0; i < 100 && Wire.available() < 1; i++){delay(5);};
  byte data = Wire.read();
  return(data);
}

void sendData(byte reg, byte val, byte address){
  Wire.beginTransmission(address);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}
