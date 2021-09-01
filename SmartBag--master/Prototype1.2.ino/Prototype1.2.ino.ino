#include <Wire.h>
#include "DHT.h"
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h> // include i/o class header

// declare the lcd object
hd44780_I2Cexp lcd; // auto locate and autoconfig interface pins

#define DHTPIN 11       // what pin humidity sensor is connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE, 15);

// tell the hd44780 sketch the lcd object has been declared
#define HD44780_LCDOBJECT
#ifndef HD44780_LCDOBJECT

#error "Special purpose sketch: Use i/o class example wrapper sketch instead."

/*
 * If not using a hd44780 library i/o class example wrapper sketch,
 * you must modify the sketch to include any needed header files for the
 * intended library and define the lcd object.
 *
 * Add your includes and constructor.
 * The lcd object must be named "lcd"
 * and comment out the #error message.
 */

#endif
/*
 * Define your LCD  parameters
 * These must match what you configured in LCDd.conf on the linux host
 */
#ifndef LCD_COLS
#define LCD_COLS 20
#endif

#ifndef LCD_ROWS
#define LCD_ROWS 4
#endif

#define ONE_WIRE_BUS 5
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

 float Celcius=0;
 float Fahrenheit=0;
 
const int incButtonPin = 6; // button to increment
const int decButtonPin = 7; // button to decrement
const int srtButtonPin = 8; // button to start 

const int relayHum = 52;
const int relayTmp = 53;

int tempValue = 70; // base number for temp
int buttonStateInc = 0; // current button state of inc
int buttonStateDec = 0; // current button state of dec
int buttonStateIncPrev = 0; // previous button state for inc
int buttonStateDecPrev = 0; // previous button state for dec
int buttonStateSrt = 0; // current button state of srt
int buttonStateSrtPrev = 0; // previous button state for srt
int interval = 50000;
int counter = 0;
int rT = 0; // standard deviation of temperature
unsigned long previousMillis = 0;
int numberOfDevices;
bool Ext = false;

void setup()
{ 
	//Serial.begin(BAUDRATE);
	// set up the LCD's number of columns and columns: 
	lcd.begin(LCD_COLS, LCD_ROWS);

#ifdef WIRECLOCK
#if (ARDUINO > 10507) && !defined(MPIDE)
	Wire.setClock(WIRECLOCK); // set i2c clock bit rate, if asked
#endif
#endif

Serial.begin(9600);

  dht.begin();
  
  sensors.begin();
  sensors.requestTemperatures(); 
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);

  numberOfDevices = sensors.getDeviceCount();

 /*Buttons*Begin**************************************************************/
  pinMode(incButtonPin, INPUT);
  pinMode(decButtonPin,INPUT);
  pinMode(srtButtonPin, INPUT);
  pinMode(relayHum, OUTPUT);
  pinMode(relayTmp, OUTPUT);
/* lcd.clear();
 
 lcd.print("Warm-N-Dry");
 
	lcd.setCursor(0,1);
 lcd.print("Bag Temp: "); //Temp inside the bag
 float t = dht.readTemperature();
 lcd.print(t);
 
  lcd.setCursor(0,2);
 lcd.print("Humidity: ");
 float h = dht.readHumidity();
 lcd.print(h);
 lcd.print("%");
 

  lcd.setCursor(0,3);
 lcd.print("HP Temp:");
 lcd.print(" F ");
 lcd.print(Fahrenheit);
	
	delay(2000);*/
 
}

uint8_t serial_getch()
{
int incoming;  

	while (!Serial.available()){ }
	// read the incoming byte:
	incoming = Serial.read();

	return (incoming & 0xff);
 
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
    if((tempValue + 5) > 90 )
       tempValue = tempValue;
    else
       tempValue = tempValue + 5;
    Serial.print("temp:");
    Serial.println(tempValue);
    }
  
  if(buttonStateDec == LOW)
  {
    if((tempValue - 5) < 70 )
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
 Serial.print("BValue: ");
 Serial.println(buttonStateSrt);
  if((buttonStateSrt == LOW)||((currentMillis - previousMillis) >= cInterval)){
      Ext = true;
  }
  
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

void loop()
{
  //delay(1000);
  Serial.print("in loop");
  
/*uint8_t rxbyte;

	rxbyte = serial_getch(); // Fetch byte

	if(rxbyte==0xFE) // If command 
		lcd.command(serial_getch()); // Pass through raw hd44780 command
	else
		lcd.write(rxbyte); //Otherwise send it as text character*/

/************************Temperature Sensor Serial Data**************v*/

/*Button*Read*******************************************/
  Serial.print("in if-statement");
  Serial.println(Ext);
  if(Ext == false)
  {
  buttonHitCheck();
  return;
  }

 Serial.print("BValue: ");
 Serial.println(buttonStateSrt);
 
  sensors.requestTemperatures();
 
  float h = dht.readHumidity();
  float t = dht.readTemperature(true);

  float tAvg;
  tAvg = getAvg(tAvg);

  rT = tempValue - 10;

  
unsigned long currentMillis = millis();
Serial.println(currentMillis);
if(counter == 0){ // if this is the first action we do not want the time condition 
    if(h > 90)  // fan start/stop
    {
        digitalWrite(relayHum, HIGH);
        delay(20000);
        digitalWrite(relayHum, LOW);
        previousMillis = currentMillis;
        counter++;
        goto jmp;
    }
    if((t < tempValue) && (t < rT) ) // heat start/stop
    {
     
      digitalWrite(relayTmp, HIGH);
      delay(20000);
      digitalWrite(relayTmp, LOW);
      previousMillis = currentMillis;
      counter++;
      goto jmp;
      }
}
else{ // now that it is not the first action we can use delays between every action
  if((currentMillis - previousMillis) >= interval){
    if(h > 90)  // fan start/stop
    {
        digitalWrite(relayHum, HIGH);
        delay(20000);
        digitalWrite(relayHum, LOW);
        previousMillis = currentMillis;
        counter++;
        goto jmp;
    }
    if((t < tempValue) && (t < rT)) // heat start/stop
    {
      digitalWrite(relayTmp, HIGH);
      delay(20000);
      digitalWrite(relayTmp, LOW);
      previousMillis = currentMillis;
      counter++;
      goto jmp;
      }
  }


}
jmp:
   lcd.clear();
 
 lcd.print("Warm-N-Dry");
 
  lcd.setCursor(0,1);
 lcd.print("Bag Temp: "); //Temp inside the bag
 lcd.print(t);
 
  lcd.setCursor(0,2);
 lcd.print("Humidity: ");
 lcd.print(h);
 lcd.print("%");
 

  lcd.setCursor(0,3);
 lcd.print("HP Temp:");
 lcd.print(" F ");
 lcd.print(tAvg);
  
/**/   
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);
  Serial.print("Test: ");
  Serial.print(" C  ");
  Serial.print(Celcius);
  Serial.print(" F  ");
  Serial.println(Fahrenheit);

  Serial.print("AvgValue: ");
  Serial.println(tAvg);

  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" C");

}
