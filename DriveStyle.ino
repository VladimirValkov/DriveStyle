#include "RTClib.h"
#include <SPI.h>
//#include <SD.h>
#include "SdFat.h"
#include<Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

unsigned long m = 0;
int counter = 0;
int buff_id = 0;
const int MPU_addr=0x69;
int16_t AcX,AcY,AcZ;
int16_t minX=32767,maxX=-32768,minY=32767,maxY=-32768,minZ=32767,maxZ=-32768;
char filename[20],csv[100];
const int chipSelect = 10;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
double latitude=0.0, longitude=0.0, velocity=0.0;

File myFile;
SdFat SD;
RTC_DS1307 rtc;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);



void fLog()
{
  char tstamp[30];
  myFile = SD.open(filename, FILE_WRITE);  
  if (myFile) 
  {
    DateTime now3 = rtc.now();
    sprintf(tstamp,"%04d-%02d-%02d %02d:%02d:%02d;",now3.year(),now3.month(),now3.day(),now3.hour(),now3.minute(),now3.second());
    myFile.print(tstamp);
    myFile.print((unsigned int)maxX-minX, DEC);
    myFile.print(';');
    myFile.print((unsigned int)maxY-minY, DEC);
    myFile.print(';');
    myFile.print((unsigned int)maxZ-minZ, DEC);
    myFile.print(';');
    myFile.print(latitude,5);
    myFile.print(';');
    myFile.print(longitude,5);
    myFile.print(';');
    myFile.println(velocity,0);
      
  }
  myFile.close();
      
}

void readAcceleration()
{
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true); 
  AcX=Wire.read()<<8|Wire.read();   
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
}

void setup() {
  Serial.begin(9600); 
  ss.begin(GPSBaud);// setup GPS

  //setup Accelerometer
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  //setup RTC
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
  }
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  //generate filename
  DateTime now = rtc.now();
  sprintf(filename, "%02d%02d%02d%02d.csv",  now.month(), now.day(), now.hour(), now.minute()); //set filename
 
  //setup SD card
  SD.begin(chipSelect);

  //initialize accelerations 
  readAcceleration();
}

void loop() {
  DateTime now2 = rtc.now();
  //log the data into the SD card every 1s
  if(millis() - m  >= 1000)
  {
    latitude = gps.location.lat();
    longitude = gps.location.lng();
    velocity = gps.speed.kmph(); 
    fLog();

    
    Serial.print("Latitude = "); 
    Serial.print(latitude, 5);      
    Serial.print(" Longitude = "); 
    Serial.print(longitude, 5);
    Serial.print(" Speed = ");
    Serial.print(velocity, 0);
    Serial.print(" km/h X = ");
    Serial.print(maxX-minX, 0);
    Serial.print(" Y = ");
    Serial.print(maxY-minY, 0);
    Serial.print(" Z = ");
    Serial.println(maxZ-minZ, 0);  

    minX=32767;
    maxX=-32768;
    minY=32767;
    maxY=-32768;
    minZ=32767;
    maxZ=-32768;
    counter = 0;

    m = millis();
  } 
  readAcceleration();
  if (minX>AcX) minX=AcX; 
  if (maxX<AcX) maxX=AcX;
  if (minY>AcY) minY=AcY;
  if (maxY<AcY) maxY=AcY;
  if (minZ>AcZ) minZ=AcZ;
  if (maxZ<AcZ) maxZ=AcZ;  
  counter++;
  
  //loop to get GPS cordinates
  while(ss.available() > 0)
  {
    gps.encode(ss.read());
  }
}
