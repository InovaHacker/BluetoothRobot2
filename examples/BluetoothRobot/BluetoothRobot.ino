/**
 * PWM Robot
 *
 * Test sketch for pwm robot
 *
 *	author: João Campos
 *
 *
 *
 */

// include PWM Robot library for robot
#include "BluetoothRobot.h"

// --------------------------------------------------------------------------- Motors
const int motorLeft[] = {9, 10};
const int motorRight[] = {5, 6};
const int buttons[] = {13,2,4,7,8,15};
const int RX = 11;
const int TX = 12;

BluetoothRobot InovaHackerRobot(motorLeft, motorRight, RX, TX, buttons);

// --------------------------------------------------------------------------- Setup
void setup() {
//Serial.begin(9600);

//begin robot
InovaHackerRobot.setup();
}

// --------------------------------------------------------------------------- Loop

void loop() {

  InovaHackerRobot.readBluetooth();
  InovaHackerRobot.sendBluetoothData();
}
