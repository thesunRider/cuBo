
#include <TinyStepper_28BYJ_48.h>
#include <ezButton.h>
#include <Wire.h>
// code by surya for QMC5883P from datasheet https://www.qstcorp.com/upload/pdf/202202/%EF%BC%88%E5%B7%B2%E4%BC%A0%EF%BC%8913-52-19%20QMC5883P%20Datasheet%20Rev.C(1).pdf

#define compass_address  0x2C
//
// pin assignments, any digital pins can be used
//
const int LED_PIN = 13;

const int MOTORX_IN1_PIN = 4;
const int MOTORX_IN2_PIN = 5;
const int MOTORX_IN3_PIN = 6;
const int MOTORX_IN4_PIN = 7;

const int MOTORY_IN1_PIN = 8;
const int MOTORY_IN2_PIN = 9;
const int MOTORY_IN3_PIN = 10;
const int MOTORY_IN4_PIN = 11;


const int SW1 = A5;
const int SW2 = A4;
const int SW3 = A3;
const int SW4 = A2;

const int STEPS_PER_REVOLUTION = 2048;

TinyStepper_28BYJ_48 stepperX;
TinyStepper_28BYJ_48 stepperY;

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete


//
// move both X & Y motors together in a coordinated way, such that they each
// start and stop at the same time, even if one motor moves a greater distance
//

ezButton button1(SW1);
ezButton button2(SW2);
ezButton button3(SW3);
ezButton button4(SW4);

int activityCount = 0;
unsigned long lastCheck = 0;
const unsigned long window = 5000;  // 5 seconds
const int activityThreshold = 20;   // Adjust as needed




void compass_WriteReg(byte Reg, byte val) {
  Wire.beginTransmission(compass_address);  //start talking
  Wire.write(Reg);                          // Tell the HMC5883 to Continuously Measure
  Wire.write(val);                          // Set the Register
  Wire.endTransmission();
}

byte compass_ReadReg(byte adrs) {
  Wire.beginTransmission(compass_address);  //start talking
  Wire.write(adrs);                         // Set the Register
  Wire.endTransmission();
  Wire.requestFrom(compass_address, 1);
  return Wire.read();
}

void compass_init() {
  compass_WriteReg(0x29, 0x06);  //Write Register 29H by 0x06 (Define the sign for X Y and Z axis)
  compass_WriteReg(0x0B, 0x08);  //Define Set/Reset mode, with Set/Reset On, Field Range 8Guass
  compass_WriteReg(0x0A, 0xCD);  //normal mode,odr = 200HZ
}


void compass_softReset() {
  compass_WriteReg(0x0B, 0x80);  // soft reset
  compass_WriteReg(0x29, 0x06);  //Write Register 29H by 0x06 (Define the sign for X Y and Z axis)
  compass_WriteReg(0x0B, 0x08);  //Define Set/Reset mode, with Set/Reset On, Field Range 8Guass
  compass_WriteReg(0x0A, 0xCD);  //normal mode,odr = 200HZ
}

/**
 * read values from device
 * @return status value:
 *  - 0:success
 *  - 1:data too long to fit in transmit buffer
 *  - 2:received NACK on transmit of address
 *  - 3:received NACK on transmit of data
 *  - 4:other error
 *  - 8:overflow (magnetic field too strong)
 */
int compass_read_val(int* x, int* y, int* z) {

  delay(10);

  byte chip_id = compass_ReadReg(0x00);
  if (chip_id != 0x80) { return 0; }

  *x = (int)(int16_t)(compass_ReadReg(1) | (compass_ReadReg(2) << 8));
  *y = (int)(int16_t)(compass_ReadReg(3) | (compass_ReadReg(4) << 8));
  *z = (int)(int16_t)(compass_ReadReg(5) | (compass_ReadReg(6)<< 8));
  return 1;
}

float compass_azimuth(int* a, int* b) {
  float azimuth = atan2((int)*a, (int)*b) * 180.0 / PI;
  return azimuth < 0 ? 360 + azimuth : azimuth;
}

int compass_read(int* x, int* y, int* z, int* a) {
  int err = compass_read_val(x, y, z);
  *a = compass_azimuth(y, x);
  return err;
}

int x_cmps=10,y_cmps=11,z_cmps=12;
int az_cmps=5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);

   Wire.begin();
  compass_init();

  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(SW3, INPUT);
  pinMode(SW4, INPUT);

  inputString.reserve(200);
  stepperX.connectToPins(MOTORX_IN1_PIN, MOTORX_IN2_PIN, MOTORX_IN3_PIN, MOTORX_IN4_PIN);
  stepperY.connectToPins(MOTORY_IN1_PIN, MOTORY_IN2_PIN, MOTORY_IN3_PIN, MOTORY_IN4_PIN);

  stepperX.setSpeedInStepsPerSecond(300);
  stepperX.setAccelerationInStepsPerSecondPerSecond(500);

  stepperY.setSpeedInStepsPerSecond(300);
  stepperY.setAccelerationInStepsPerSecondPerSecond(500);



  stepperX.setCurrentPositionInSteps(0);
  stepperY.setCurrentPositionInSteps(0);
}

void run_command(String input) {
  if (inputString.startsWith("sm;")) {
    int s1Val = -1;
    int s2Val = -1;

    // Find s1:
    int s1Index = inputString.indexOf("s1:");
    if (s1Index != -1) {
      int s1End = inputString.indexOf(";", s1Index);
      s1Val = inputString.substring(s1Index + 3, s1End).toInt();
      stepperX.setupRelativeMoveInSteps(s1Val);
    }

    // Find s2:
    int s2Index = inputString.indexOf("s2:");
    if (s2Index != -1) {
      int s2End = inputString.indexOf(";", s2Index);
      if (s2End == -1) s2End = inputString.length();
      s2Val = inputString.substring(s2Index + 3, s2End).toInt();
      stepperY.setupRelativeMoveInSteps(s2Val);
    }
    Serial1.print("ACK;sm");
  } else if (inputString.startsWith("smt1;")) {
    stepperX.setupStop();
    Serial1.print("ACK;smt1");
  } else if (inputString.startsWith("smt2;")) {
    stepperY.setupStop();
    Serial1.print("ACK;smt2");
  } else if (inputString.startsWith("sws;")) {
    Serial1.print("ACK;sws:");
    Serial1.print(digitalRead(SW1));
    Serial1.print(digitalRead(SW2));
    Serial1.print(digitalRead(SW3));
    Serial1.print(digitalRead(SW4));
  }
  else if (inputString.startsWith("swa;")) {
    Serial1.print("ACK;swa:");
    Serial1.print(activityCount);
  } else if (inputString.startsWith("cmps;")) {
    compass_read(&x_cmps,&y_cmps,&z_cmps,&az_cmps);
    Serial1.print("ACK;cmps:");
    Serial1.print(x_cmps);
    Serial1.print(y_cmps);
    Serial1.print(z_cmps);
    Serial1.print(az_cmps);
  }
}

void loop() {
  button1.loop();  // MUST call the loop() function first
  button2.loop();  // MUST call the loop() function first
  button3.loop();  // MUST call the loop() function first
  button4.loop();

  if (button1.isPressed() || button2.isPressed() || button3.isPressed() || button4.isPressed()){
    activityCount += 1;
  }
    

  // put your main code here, to run repeatedly:
  if (Serial1.available()) {
    while (Serial1.available()) {
      // get the new byte:
      char inChar = (char)Serial1.read();
      Serial.println(inChar);
      // add it to the inputString:
      inputString += inChar;
      // if the incoming character is a newline, set a flag so the main loop can
      // do something about it:
      if (inChar == '\n') {
        stringComplete = true;
      }
    }
  }

  if (stringComplete) {
    Serial.println(inputString);
    run_command(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  if ((!stepperX.motionComplete()) || (!stepperY.motionComplete())) {
    stepperX.processMovement();
    stepperY.processMovement();
  }

  if (millis() - lastCheck >= window) {
    if (activityCount >= activityThreshold) {
      Serial.println("⚡ High switch activity detected! Taking action...");
      // <-- Put your action here
    } else {
      Serial.print("Activity count: ");
      Serial.println(activityCount);
    }

    activityCount = 0;
    lastCheck = millis();
  }
}
