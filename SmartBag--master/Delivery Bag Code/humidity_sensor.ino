#include "DHT.h"
#include <LiquidCrystal.h>
#include <Wire.h>

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT22   // DHT 22
//#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); 

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
//  lcd.begin(20,4);
//  lcd.setCursor(3,0);
//  lcd.print(("DHTxx test!"));
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // h variable reads from readHumidity function
  float h = dht.readHumidity();
  //lcd.begin(20,4);
  lcd.setCursor(2,0);
  //lcd.print((h));
  

  // Check if sensor readings failed and exit early (to try again).
  if (isnan(h)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  Serial.print(F("Humidity: "));
  Serial.println(h);
  lcd.print(h);
}
