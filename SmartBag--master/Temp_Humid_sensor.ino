#include <DHT.h>

DHT tmp_hum;

#define DHT11_PIN 7

void setup(){
  Serial.begin(9600);
}

void loop() {
  int check = tmp_hum.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(tmp_hum.readTemperature); 
  Serial.print("Humidity = ");
  Serial.println(tmp_hum.readHumidity); 
  delay(1000);
}
