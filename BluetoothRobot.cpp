#include "Arduino.h"
#include "BluetoothRobot.h"


#define    STX          0x02
#define    ETX          0x03
#define    SLOW         750                            // Datafields refresh rate (ms)
#define    FAST         250                             // Datafields refresh rate (ms)

BluetoothRobot::BluetoothRobot(const int * motorLeft, const int * motorRight, const int RX, const int TX, const int * buttons): PWMRobot(motorLeft,motorRight), mySerial(RX,TX), _buttons(buttons){

buttonStatus = 0;
displayStatus = "";
sendInterval = SLOW;

}


void BluetoothRobot::setup()
{
  int i;
  for(i = 0; i < 2; i++){
    pinMode(_motorLeft[i], OUTPUT);
    pinMode(_motorRight[i], OUTPUT);
  }

  for(i = 0; i < 5; i++){
    pinMode(_buttons[i], OUTPUT);
  }


  mySerial.begin(9600);
  Serial.begin(9600);
  _previousMillis = 0;
  while(mySerial.available())  mySerial.read();         // empty RX buffer
  Serial.println("Bluetooth Robot setup ready");
}
// ---------------------------------Bluetooth reading
//
void BluetoothRobot::readBluetooth(){
  byte cmd[8];
  if(mySerial.available())  {                           // data received from smartphone
    delay(2);
    cmd[0] =  mySerial.read();
    if(cmd[0] == STX)  {
      int i=1;
      while(mySerial.available())  {
        delay(1);
        cmd[i] = mySerial.read();
        if(cmd[i]>127 || i>7)                 break;     // Communication error
        if((cmd[i]==ETX) && (i==2 || i==7))   break;     // Button or Joystick data
        i++;
      }
      if     (i==2)           getButtonState(cmd[1]);    // 3 Bytes  ex: < STX "C" ETX >
      else if(i==7)          _getJoystickState(cmd);     // 6 Bytes  ex: < STX "200" "180" ETX >
    }
  }




}

void BluetoothRobot::sendBluetoothData(){

  long currentMillis = millis();
  if(currentMillis - _previousMillis > sendInterval) {   // send data back to smartphone
    _previousMillis = currentMillis;

// Data frame transmitted back from Arduino to Android device:
// < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >
// < 0X02      "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example

    mySerial.print((char)STX);                                             // Start of Transmission
    mySerial.print(getButtonStatusString());  mySerial.print((char)0x1);   // buttons status feedback
    mySerial.print(GetdataInt1());            mySerial.print((char)0x4);   // datafield #1
    mySerial.print(GetdataFloat2());          mySerial.print((char)0x5);   // datafield #2
    mySerial.print(displayStatus);                                         // datafield #3
    mySerial.print((char)ETX);                                             // End of Transmission
  }

}

void BluetoothRobot::_getJoystickState(byte data[8])    {
  _joyX = (data[1]-48)*100 + (data[2]-48)*10 + (data[3]-48);       // obtain the Int from the ASCII representation
  _joyY = (data[4]-48)*100 + (data[5]-48)*10 + (data[6]-48);
  _joyX = _joyX - 200;                                                  // Offset to avoid
  _joyY = _joyY - 200;                                                  // transmitting negative numbers


// commmunication error
  if(_joyX<-100 || _joyX>100 || _joyY<-100 || _joyY>100) {
    _joyX = 0;
    _joyY = 0;
  }
  _joyY = _joyY/100;
  _joyX = _joyX/100;
  // Your code here ...
      Serial.print("Joystick position:  ");
      Serial.print(_joyX);
      Serial.print(", ");
      Serial.println(_joyY);
      _processJoystickState();

}
/**
 * [BluetoothRobot::_processJoystickState set speed and direction based in joystick data]
 * TODO[Evaluate if algorithm works]
 */
void BluetoothRobot::_processJoystickState(){

  int motorLeftSpeed;
  int motorRightSpeed;


  if(_joyY>0){

    if(_joyX>0){
      motorLeftSpeed = _pwmLimit(255*(_joyY-_joyX));
      motorRightSpeed = _pwmLimit(255*(_joyX+_joyY));
      Serial.print("motor speed:  ");
      Serial.print(motorLeftSpeed);
      Serial.print(", ");
      Serial.println(motorRightSpeed );
      PWMRobot::setSpeed(motorLeftSpeed, motorRightSpeed);
      PWMRobot::driveForward();

    }else{
      Serial.print("motor speed:  ");
      Serial.print(motorLeftSpeed);
      Serial.print(", ");
      Serial.println(motorRightSpeed );
      motorLeftSpeed = _pwmLimit(255*(_joyY-_joyX));
      motorRightSpeed = _pwmLimit(255*(_joyX+_joyY));
      PWMRobot::setSpeed(motorLeftSpeed, motorRightSpeed);
      PWMRobot::driveForward();
    }
  }else{

    if(_joyX>0){
      Serial.print("motor speed:  ");
      Serial.print(motorLeftSpeed);
      Serial.print(", ");
      Serial.println(motorRightSpeed );
      motorLeftSpeed = _pwmLimit(255*(-_joyY-_joyX));
      motorRightSpeed = _pwmLimit(255*(_joyX-_joyY));
      PWMRobot::setSpeed(motorLeftSpeed, motorRightSpeed);
      PWMRobot::driveBackward();

    }else{
      Serial.print("motor speed:  ");
      Serial.print(motorLeftSpeed);
      Serial.print(", ");
      Serial.println(motorRightSpeed );
      motorLeftSpeed = _pwmLimit(255*(-_joyY-_joyX));
      motorRightSpeed = _pwmLimit(255*(_joyX-_joyY));
      PWMRobot::setSpeed(motorLeftSpeed, motorRightSpeed);
      PWMRobot::driveBackward();

  }
 }
}

int BluetoothRobot::_pwmLimit(float value){
  if (value>255){
    return 255;
  }
  if(value<0){
    return 0;
  }
  int intValue = int(value);


  return intValue;
}

void BluetoothRobot::getButtonState(int bStatus)  {
  switch (bStatus) {
// -----------------  BUTTON #1  -----------------------
    case 'A':
      buttonStatus |= B000001;        // ON
      Serial.println("\n** Button_1: ON **");
      // your code...
      displayStatus = "LED <ON>";
      Serial.println(displayStatus);
      digitalWrite(_buttons[0], HIGH);
      break;
    case 'B':
      buttonStatus &= B111110;        // OFF
      Serial.println("\n** Button_1: OFF **");
      // your code...
      displayStatus = "LED <OFF>";
      Serial.println(displayStatus);
      digitalWrite(_buttons[0], LOW);
      break;

// -----------------  BUTTON #2  -----------------------
    case 'C':
      buttonStatus |= B000010;        // ON
      Serial.println("\n** Button_2: ON **");
      // your code...
      displayStatus = "Button2 <ON>";
      Serial.println(displayStatus);
      digitalWrite(_buttons[1], HIGH);
      break;
    case 'D':
      buttonStatus &= B111101;        // OFF
      Serial.println("\n** Button_2: OFF **");
      // your code...
      displayStatus = "Button2 <OFF>";
      Serial.println(displayStatus);
      digitalWrite(_buttons[1], LOW);
      break;

// -----------------  BUTTON #3  -----------------------
    case 'E':
      buttonStatus |= B000100;        // ON
      Serial.println("\n** Button_3: ON **");
      // your code...
      displayStatus = "Motor #1 enabled"; // Demo text message
      Serial.println(displayStatus);
      digitalWrite(_buttons[2], HIGH);
      break;
    case 'F':
      buttonStatus &= B111011;      // OFF
      Serial.println("\n** Button_3: OFF **");
      // your code...
      displayStatus = "Motor #1 stopped";
      Serial.println(displayStatus);
      digitalWrite(_buttons[2], LOW);
      break;

// -----------------  BUTTON #4  -----------------------
    case 'G':
      buttonStatus |= B001000;       // ON
      Serial.println("\n** Button_4: ON **");
      // your code...
      displayStatus = "Datafield update <FAST>";
      Serial.println(displayStatus);
      digitalWrite(_buttons[3], HIGH);
      sendInterval = FAST;
      break;
    case 'H':
      buttonStatus &= B110111;    // OFF
      Serial.println("\n** Button_4: OFF **");
      // your code...
      displayStatus = "Datafield update <SLOW>";
      Serial.println(displayStatus);
      digitalWrite(_buttons[3], LOW);
      sendInterval = SLOW;
     break;

// -----------------  BUTTON #5  -----------------------
    case 'I':           // configured as momentary button
//      buttonStatus |= B010000;        // ON
      Serial.println("\n** Button_5: ++ pushed ++ **");
      // your code...
      displayStatus = "Button5: <pushed>";
      digitalWrite(_buttons[4], HIGH);
      break;
//   case 'J':
//     buttonStatus &= B101111;        // OFF
//     // your code...
//     break;

// -----------------  BUTTON #6  -----------------------
    case 'K':
      buttonStatus |= B100000;        // ON
      Serial.println("\n** Button_6: ON **");
      // your code...
       displayStatus = "Button6 <ON>"; // Demo text message
       digitalWrite(_buttons[5], HIGH);
     break;
    case 'L':
      buttonStatus &= B011111;        // OFF
      Serial.println("\n** Button_6: OFF **");
      // your code...
      digitalWrite(_buttons[5], LOW);
      displayStatus = "Button6 <OFF>";
      break;
  }
// ---------------------------------------------------------------
}

String BluetoothRobot::getButtonStatusString()  {
  String bStatus = "";
  for(int i=0; i<6; i++)  {
    if(buttonStatus & (B100000 >>i))      bStatus += "1";
    else                                  bStatus += "0";
  }
  return bStatus;
}

int BluetoothRobot::GetdataInt1()  {              // Data dummy values sent to Android device for demo purpose
  static int i= -30;              // Replace with your own code
  i ++;
  if(i >0)    i = -30;
  return i;
}

float BluetoothRobot::GetdataFloat2()  {           // Data dummy values sent to Android device for demo purpose
  static float i=50;               // Replace with your own code
  i-=.5;
  if(i <-50)    i = 50;
  return i;
}
