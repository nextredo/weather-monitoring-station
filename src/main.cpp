#include <Arduino.h>
#include <DHT.h>
#include <Wire.h> // need for DHT22
#include <Adafruit_Sensor.h> // need for DHT22
// #include <SPI.h>
// not sure if I even need Wire.h or SPI.h
// SPI.h is for MISO and MOSI devices, which I don't have (i think) (might need for DHT22)
// Wire.h is for I2C devices (BMP280) pretty sure I need that

#include <SoftwareSerial.h> // for HC-12 and possibly BMP280?
SoftwareSerial HC12(12, 13); // TX (ON HC12), RX (ON HC12) SET PIN IS PIN 11
// CHANGE BMP280 PINS TO BE ON I2C PINS (SDA=pin18, SCL=pin19)

#include <Adafruit_BMP280.h>
Adafruit_BMP280 bme;
#define DHTPin 2
#define DHTType DHT22

#define voltPin  A0
#define lightPin A1
#define soilPin  A2
// defines analog pins labelled as sensor inputs


#define setPin 11
DHT dht(DHTPin, DHTType);
String strDHTTemp;
String strDHTHumid;
String strBMPTemp;
String strBMPPress;
String strVolts;
String strSoil;
String strLux;

float volts;
float soil;

float temp;
float humid;
float BMPTemp;
float BMPPressure;
float roundBMPPressure;

void setup() {
  pinMode(DHTPin,     INPUT);
  pinMode(setPin,    OUTPUT);
  digitalWrite(setPin, HIGH);
  

  dht.begin();
  bme.begin();
  HC12.begin(9600);
  Serial.begin(9600);
  delay(200);
  digitalWrite(setPin, LOW);
  delay(400);
  HC12.write("AT+C002");
  // don't have to do this every time
  // HC12 remembers data channel after reboot
  while(HC12.available()) {
    Serial.write(HC12.read());
    // must be Serial.write not Serial.print
    // because it will echo character values if .print and actual characters if .
  }
  delay(200);
  digitalWrite(setPin, HIGH);
}

void loop() {
  delay(6000);
  temp = dht.readTemperature();
  humid = dht.readHumidity();
  BMPTemp = bme.readTemperature();
  BMPPressure = bme.readPressure();
  roundBMPPressure = BMPPressure / 100; // DO THIS ON OTHER SIDE INSTEAD KEK
  //float BMPHeight = bme.readAltitude(1013.25); // THIS ONE TOO (or don't even need it)

  volts = analogRead(voltPin);
  volts = volts*(5.0/1023.0);
  //volts = volts/4.092;
  //volts = (volts)/10;
  //commented functions are for voltage divider that i'm not using

  float lVolts = analogRead(lightPin) * 5.0 / 1024.0; // converts analogRead into voltage
  float lAmps = lVolts / 10000.0;  // across 10,000 Ohms
  float lMicroamps = lAmps * 1000000; // amps to microamps
  float lux = lMicroamps * 2.0; // microamps to lux

  soil = analogRead(soilPin);
  soil = map(soil, 193, 1018, 0, 100); // experimentally determined with diagnostic test
  soil = (100 - soil);
  
  strVolts = String(volts);
  strLux = String(lux);
  strDHTTemp = String(temp);
  strDHTHumid = String(humid);
  strBMPTemp = String(BMPTemp);
  strBMPPress = String(roundBMPPressure);
  strSoil = String(soil);
  Serial.print(temp);
  Serial.write("DT");
  Serial.print(" ");
    
  Serial.print(humid);
  Serial.write("DH");
  Serial.print(" ");
    
  Serial.print(BMPTemp);
  Serial.write("BT");
  Serial.print(" ");

  Serial.print(BMPPressure);
  Serial.write("BP");
  Serial.print(" ");

  Serial.print(volts);
  Serial.write("V");
  Serial.print(" ");

  Serial.print(lux);
  Serial.write("Lux");
  Serial.print(" ");

  Serial.print(soil);
  Serial.write("%");
  Serial.print(" ");
  Serial.println("");

//HC-12 Data Sending
  
  HC12.print(strDHTTemp); // 4 digits (12.34)
  HC12.print("A");
  HC12.print(strDHTHumid); // 4 digits (12.34)
  HC12.print("B");
  HC12.print(strBMPTemp); // 4 digits (12.34)
  HC12.print("C");
  HC12.print(strBMPPress); // 8 digits (123456.78)
  HC12.print("D");
  HC12.print(strVolts); // 3 digits (1.23)
  HC12.print("E");
  HC12.print(strLux); // varies significantly
  HC12.print("F");
  HC12.print(strSoil); // 3 digits (1.23)
  HC12.print("G");

  // DATA RECEPTION CONFORMATION
    // yes do this so that you can make program wait before sending fresh set of data
    // means they can sync up
    // do with the following
    // while(!(HC12.available())) { CODE FOR SENDING }
    // while (HC12.available()) { CODE FOR STOPPING SENDING AND SYNCING UP TIMES }

    // other arduino should have code for sending something when valuenumber reaches 4
    // to tell this arduino to stop and wait
    
    
    // DATA PACKET ORDERING VS DATA PACKET LABELLING
    // Don't just order data packets sent, attach a special figure or letter to the end of the string
    // so that the other arduino can differentiate and tell which packet of information was sent

    // starting with letters means it's more human friendly when determining order (rather than symbols)

    // have other arduino use IF statements to add HCinputString to a certain DataBuffer array position

    // have the character denoting the previous value be the splitter as well (makes shit easier)
}