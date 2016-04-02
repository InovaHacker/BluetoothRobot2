/*
  BluetoothRobot.h - Library for BluetoothRobot extends PWM Robot with bluetooth communication.
  Created by João Campos.
  This library is intended to be used with joystick bluetooth commander app.
  All the bluetooth commands are from \n\nAndroTest V2.0 - @kas2014\ndemo for V5.X App (6 button version)".

  Released into the public domain.
*/

#ifndef   BluetoothRobot_h
#define   BluetoothRobot_h

#include "Arduino.h"
#include "PWMRobot.h"
#include "SoftwareSerial.h"

class   BluetoothRobot : public PWMRobot
{
  public:
    BluetoothRobot(const int * motorLeft, const int * motorRight, const int RX, const int TX, const int * buttons);
    SoftwareSerial mySerial;
    void setup();
    void readBluetooth();
    void sendBluetoothData();
    String getButtonStatusString();
    int GetdataInt1();
    float GetdataFloat2();
    void getButtonState(int bStatus);
    int sendInterval;
  private:
   const  int * _motorLeft;
   const  int * _motorRight;
   const  int * _buttons;
   long _previousMillis;
   float _joyX;
   float _joyY;
   void _getJoystickState(byte data[8]);
   void _processJoystickState();
   int _pwmLimit(float value);
   char * displayStatus;
   int buttonStatus;
};


#endif
