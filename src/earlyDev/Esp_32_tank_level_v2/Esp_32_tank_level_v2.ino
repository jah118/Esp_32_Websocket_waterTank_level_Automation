
/////////////////////////////////////////////////////////////////
//                  ESP32 Project:                             //
//                                                             //
//            Author:  Jonas                                   //
/////////////////////////////////////////////////////////////////


#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C
// variables
#define SEALEVELPRESSURE_HPA (1013.25)


// Sensor
unsigned long delayTime;
float temperature = 0;
float humidity = 0;
float pressure = 0;
float PrintInSerial = 1;

const int relay_1_pin = 33;
const int relay_2_pin = 25;

bool relayState;
bool relaySAFETYState;

const int led_1_pin = 15;
const int led_2_pin = 16;
const int led_3_pin = 4;
const int led_4_pin = 17;
const int led_5_pin = 18;


const int tankFloating_sen_1 = 36;
const int tankFloating_sen_2 = 39;
const int tankFloating_sen_3 = 34;
const int tankFloating_sen_4 = 35;
const int tankFloating_sen_5 = 32;


int sensorOn = 2400 ; //700 old
int tankSensorValue1_1 = 0;
int tankSensorValue1_2 = 0;
int tankSensorValue1_3 = 0;
int tankSensorValue1_4 = 0;
int tankSensorValue1_5 = 0;

void printTankLevelSensorValue() {
  Serial.println("----Sensor Values---- ");
  Serial.println();
  Serial.print("sensor 1:");
  Serial.print(tankSensorValue1_1);
  Serial.println();
  Serial.print("sensor 2:");
  Serial.print(tankSensorValue1_2);
  Serial.println();
  Serial.print("sensor 3:");
  Serial.print(tankSensorValue1_3);
  Serial.println();
  Serial.print("sensor 4:");
  Serial.print(tankSensorValue1_4);
  Serial.println();
  Serial.print("sensor 5:");
  Serial.print(tankSensorValue1_5);
  Serial.println();
}


void tankLevelCheck () {
  // put your main code here, to run repeatedly:

  tankSensorValue1_1 = analogRead(tankFloating_sen_1);
  tankSensorValue1_2 = analogRead(tankFloating_sen_2);
  tankSensorValue1_3 = analogRead(tankFloating_sen_3);
  tankSensorValue1_4 = analogRead(tankFloating_sen_4);
  tankSensorValue1_5 = analogRead(tankFloating_sen_5);

  //tankSensorValue1_1 > sensorOn && tankSensorValue1_2 < sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn

  if (tankSensorValue1_1 < sensorOn ) {
    relayState = false;
    relaySAFETYState  = false;
    Serial.println("Tank er er tom SAFETY RELAY OFFF");
  }

  if (digitalRead(tankSensorValue1_1) == LOW && digitalRead(tankSensorValue1_2) == LOW) {
    relayState = false;
    relaySAFETYState  = false;
    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank er er tom");

  } else if (digitalRead(tankSensorValue1_1) == HIGH && digitalRead(tankSensorValue1_2) == LOW  && digitalRead(tankSensorValue1_3) == LOW && digitalRead(tankSensorValue1_4) == LOW && digitalRead(tankSensorValue1_5) == LOW) {
    relayState = false;
    relaySAFETYState  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 12.5%");
    Serial.println(digitalRead(tankSensorValue1_1));

  } else if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    relaySAFETYState  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 25%");

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    relayState = true; // only for testing -----------------------------
    relaySAFETYState  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 50%");

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 > sensorOn && tankSensorValue1_5 < sensorOn) {
    relaySAFETYState  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 75%");

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 > sensorOn && tankSensorValue1_5 > sensorOn) {
    relaySAFETYState  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, HIGH);
    Serial.println("Tank level er 100%");
  } else {

    relaySAFETYState  = false;
    Serial.println("sensor error");

    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    delay(100);
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, HIGH);
    delay(100);
    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("sensor error");
  }

}


void printBMEValues() {
  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
    Serial.print(1.8 * bme.readTemperature() + 32);
    Serial.println(" *F");*/

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}




void setup() {

  // Start Serial port
  Serial.begin(115200);
  delay(150);
  Serial.println();
  Serial.println("Serial is begining");
  while (!Serial) // Waiting for serial connection
  {
  }
  delay(100);
  Serial.println("serial has started");

  // Init LED and turn off
  pinMode(led_1_pin, OUTPUT);
  digitalWrite(led_1_pin, LOW);
  pinMode(led_2_pin, OUTPUT);
  digitalWrite(led_2_pin, LOW);
  pinMode(led_3_pin, OUTPUT);
  digitalWrite(led_3_pin, LOW);
  pinMode(led_4_pin, OUTPUT);
  digitalWrite(led_4_pin, LOW);
  pinMode(led_5_pin, OUTPUT);
  digitalWrite(led_5_pin, LOW);

  pinMode(relay_1_pin, OUTPUT);
  digitalWrite(relay_1_pin, LOW);
  pinMode(relay_2_pin, OUTPUT);
  digitalWrite(relay_2_pin, LOW);



  pinMode(tankFloating_sen_1, INPUT);
  pinMode(tankFloating_sen_2, INPUT);
  pinMode(tankFloating_sen_3, INPUT);
  pinMode(tankFloating_sen_4, INPUT);
  pinMode(tankFloating_sen_5, INPUT);


  bool status;

  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  relayState = false;
  relaySAFETYState  = false;

}

void loop() {
  tankLevelCheck();
  printTankLevelSensorValue();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println(digitalRead(tankSensorValue1_1));
  Serial.println(digitalRead(tankSensorValue1_2));
  Serial.println(digitalRead(tankSensorValue1_3));
  Serial.println(digitalRead(tankSensorValue1_4));
  Serial.println(digitalRead(tankSensorValue1_5));
  Serial.println();
  Serial.println();
  Serial.println();
  printBMEValues();
  Serial.println(relayState);

  if (relayState == true && relaySAFETYState == true) {
    Serial.println("ON");
    digitalWrite(relay_1_pin, HIGH); // HIGH tænder
    digitalWrite(relay_1_pin, HIGH); // HIGH tænder
  } else {
    Serial.println("OFF");
    digitalWrite(relay_1_pin, LOW); // LOW slukker
    digitalWrite(relay_2_pin, LOW); // LOW slukker
  }

  delay(500);

}
