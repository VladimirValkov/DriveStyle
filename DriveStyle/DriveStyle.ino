#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include<Wire.h>

File myFile;
Sd2Card card;
SdVolume volume;
SdFile root;
RTC_DS1307 rtc;

unsigned long m = 0, m1 = 0;
int buff_id = 0;
const int MPU_addr=0x69;
int16_t AcX,AcY,AcZ, AcXold, AcYold, AcZold;
float dX, dY , dZ,dXa,dYa,dZa;
char filename[20];
const int chipSelect = 10;

void fLog()
{
     
     myFile = SD.open(filename, FILE_WRITE);
     
   if (myFile) {
    
    DateTime now3 = rtc.now();
    myFile.print(now3.year(), DEC);
    myFile.print('-');
    myFile.print(now3.month(), DEC);
    myFile.print('-');
    myFile.print(now3.day(), DEC);
    myFile.print(' ');   
    myFile.print(now3.hour(), DEC);
    myFile.print(':');
    myFile.print(now3.minute(), DEC);
    myFile.print(':');
    myFile.print(now3.second(), DEC);
    myFile.print(';');
    myFile.print(dXa, DEC);
    myFile.print(';');
    myFile.print(dYa, DEC);
    myFile.print(';');
    myFile.println(dZa, DEC);
      
    }
    myFile.close();
      
}

void readAcc()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
}



void setup() {
  Serial.begin(9600);
  DateTime now = rtc.now();

  //setup Accelerometer
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  //end Accelerometer
  //setup RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    //rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
//rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
 //rtc.adjust(DateTime(2019, 10, 5, 23, 59, 0));
 //end RTC
 sprintf(filename, "%02d%02d%02d%02d.csv",  now.month(), now.day(), now.hour(), now.minute()); //set filename
 
 
//setup SD
    SD.begin(chipSelect);
  
}

void loop() {
  
  if(millis() - m  >= 1000)
      {
        m = millis();
        dXa = round(sqrt(dXa/ 10));
        dYa = round(sqrt(dYa/10));
        dZa = round(sqrt(dZa/10));
        fLog();
        Serial.print(dXa);
        Serial.print(" ");
        Serial.print(dYa);
        Serial.print(" ");
        Serial.println(dZa);
        dXa = 0;
        dYa = 0;
        dZa = 0;
      } 
      if(millis() - m1 >= 100)
      {
        m1 = millis();
        DateTime now2 = rtc.now();
        AcXold = AcX;
        AcYold = AcY;
        AcZold = AcZ;
        readAcc();
        dX = AcX - AcXold;
        dY = AcY - AcYold;
        dZ = AcZ - AcZold;
        dXa += dX * dX;
        dYa += dY * dY;
        dZa += dZ * dZ;
        
 
     }
}
