#include <Wire.h>
// code by surya for QMC5883P from datasheet https://www.qstcorp.com/upload/pdf/202202/%EF%BC%88%E5%B7%B2%E4%BC%A0%EF%BC%8913-52-19%20QMC5883P%20Datasheet%20Rev.C(1).pdf

#define compass_address  0x2C

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

int x=10,y=11,z=12;
int az=5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin();
  compass_init();

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  compass_read(&x,&y,&z,&az);
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(z);
  Serial.println(" ");
}
