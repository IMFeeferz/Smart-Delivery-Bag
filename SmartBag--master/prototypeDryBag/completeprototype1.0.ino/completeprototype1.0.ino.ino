#include "DHT.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#define DHTPIN 8     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT22   // DHT 22
//#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 7
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress tempDeviceAddress;
/********************************************************************/ 
const int incButtonPin = 13; // button to increment

const int decButtonPin = 10; // button to decrement

const int srtButtonPin = 9; // button to start 

const int relayPin = 23; // pin for the fan relay
const int fanPin = 24; // pin for output of fan

// const int relayPin2 = 25; // pin for heat relay

int tempValue = 50; // base number for temp
int buttonStateInc = 0; // current button state of inc
int buttonStateDec = 0; // current button state of dec
int buttonStateIncPrev = 0; // previous button state for inc
int buttonStateDecPrev = 0; // previous button state for dec
int buttonStateSrt = 0; // current button state of srt
int buttonStateSrtPrev = 0; // previous button state for srt
int interval = 0;
unsigned long previousMillis = 0;
int numberOfDevices;
bool Ext = false;

/**********************************************************************************/
void setup() {
  // begin all sensors,lcd,etc
 /*Displays*Begin******************************************************************/
  Serial.begin(9600);
  lcd.begin(16, 2);
  /*Sensors*Begin***************************************************************/
  sensors.begin();
  dht.begin();
  /*DS18b20*Sensor*Code*********************************************************/
  numberOfDevices = sensors.getDeviceCount();
  /*Buttons*Begin**************************************************************/
  pinMode(incButtonPin, INPUT);
  pinMode(decButtonPin,INPUT);
  pinMode(srtButtonPin, INPUT);
/*****************************************************************************/
  pinMode(relayPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, HIGH);
  
}



void loop() {
  // Wait a few seconds between measurements.
  //delay(2000);
/*Button*Read*******************************************/
  if(Ext == false)
  {
  buttonHitCheck();
  return;
  }
  
/*Sensors*read and print*********************************/
sensors.requestTemperatures();
 float h = dht.readHumidity();
// float t2 = sensors.getTempCByIndex(0); // Send the command to get temperature readings 
// float t = (t2*(9/5))+32;
  float tAvg;
  tAvg = getAvg(tAvg);
/*Serial*print*for*testing*************************************************/
 // Serial.println(t2);
 // Serial.println(t);

int tA;
tA = tAvg;

unsigned long currentMillis = millis();
Serial.println(currentMillis);
if(tA > tempValue)  // fan start
 {
  digitalWrite(relayPin, HIGH);
  delay(3000);
  //delay(60000);
  digitalWrite(relayPin, LOW);
 // delay(240000);
  }
  /*
if((tA < tempValue ){  // heat start
  
  digitalWrite(relayPin2, HIGH);
  delay(1000);
  digitalWrite(relayPin2, LOW);
  
}
  
*/

 
/*Humid*LCD*print*********************************************************/
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("Hmd:");
  lcd.print(h);
  lcd.print("%");
/*Temperature*LCD*print**************************************************/
  lcd.setCursor(0,0);
  lcd.print("TA:");
  lcd.print(tAvg);
  lcd.print("F");
/***********************************************************************/
  lcd.setCursor(10,0);
  lcd.print("Th:");
  lcd.print(tempValue);
  lcd.print("F");
  
}

void buttonHitCheck(){
 
buttonStateInc = digitalRead(incButtonPin);
buttonStateDec = digitalRead(decButtonPin);
buttonStateSrt = digitalRead(srtButtonPin);
lcd.setCursor(0,0);
lcd.print("Ins Threshold:");
lcd.setCursor(0,1);
lcd.print("StrB to continue");
if((buttonStateInc != buttonStateIncPrev) || (buttonStateDec != buttonStateDecPrev))
{
  
  if(buttonStateInc == LOW)
  {
    if((tempValue + 5)>80 )
       tempValue = tempValue;
    else
       tempValue = tempValue + 5;
    Serial.print("temp:");
    Serial.println(tempValue);
    }
  
  if(buttonStateDec == LOW)
  {
    if((tempValue - 5) < 50 )
      tempValue = tempValue;
    else
      tempValue = tempValue - 5;
    Serial.print("temp:");
    Serial.println(tempValue);
    }
 
  }
  lcd.setCursor(14,0);
  lcd.print(tempValue);
 int cInterval = 30000;
 unsigned long currentMillis = millis();
  if((buttonStateSrt == LOW) || ((currentMillis - previousMillis) >= cInterval))
      Ext = true;
  buttonStateIncPrev = buttonStateInc;
  buttonStateDecPrev = buttonStateDec;
  buttonStateSrtPrev = buttonStateSrt;
  
 }

float getAvg(float tAvg)
{
  float tmp[numberOfDevices];
  float tempAvg,sum;

  sum = 0.00;
   sensors.requestTemperatures(); // Send the command to get temperatures from all sensors.
  
  // Loop through each device, print out temperature one by one
  for(int i=0; i<numberOfDevices; i++) {
    
    // Search the wire for address and store the address in tempDeviceAddress
    if(sensors.getAddress(tempDeviceAddress, i)){
    
    tmp[i] = sensors.getTempC(tempDeviceAddress); //Temperature in degree celsius
    tmp[i] = DallasTemperature::toFahrenheit(tmp[i]);
    }
  }
  for(int j = 0; j<numberOfDevices; j++){
    sum = sum + tmp[j];
    }
    tempAvg = sum/numberOfDevices;

    return tempAvg;
}
