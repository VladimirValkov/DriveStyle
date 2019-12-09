#include "RTClib.h"
#include <SPI.h>
//#include <SD.h>
#include "SdFat.h"
#include<Wire.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

unsigned long m = 0, m1 = 0;
int counter = 0;
int buff_id = 0;
const int MPU_addr=0x69;
int16_t AcX,AcY,AcZ, AcXold, AcYold, AcZold;
float dX, dY , dZ,dXa,dYa,dZa,dXaold,dYaold,dZaold;
char filename[20],csv[100];
const int chipSelect = 10;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
double latitude=0.0, longitude=0.0, velocity=0.0;

File myFile;
//Sd2Card card;
//SdVolume volume;
//SdFile root;
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
    myFile.print((unsigned int)dXa, DEC);
    myFile.print(';');
    myFile.print((unsigned int)dYa, DEC);
    myFile.print(';');
    myFile.print((unsigned int)dZa, DEC);
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
//log the data into the SD card every 1s
  if(millis() - m  >= 1000)
  {
    m = millis();
    dXa = round(sqrt(dXa/ counter));
    dYa = round(sqrt(dYa/counter));
    dZa = round(sqrt(dZa/counter));
    counter = 0;
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
    Serial.print(dXa, 0);
    Serial.print(" Y = ");
    Serial.print(dYa, 0);
    Serial.print(" Z = ");
    Serial.println(dZa, 0);   
  } 
  //get acceleration every 100ms
  if(millis() - m1 >= 100)
  {
    m1 = millis();
    DateTime now2 = rtc.now();
    AcXold = AcX;
    AcYold = AcY;
    AcZold = AcZ;
    readAcceleration();
    dX = AcX - AcXold;
    dY = AcY - AcYold;
    dZ = AcZ - AcZold;
    dXa += dX * dX;
    dYa += dY * dY;
    dZa += dZ * dZ;   
    counter++;
  }
  //loop to get GPS cordinates
  while(ss.available() > 0)
  {
    gps.encode(ss.read());
  }
}
