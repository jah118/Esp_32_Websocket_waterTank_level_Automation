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


void tankLevelCheck () {
    // put your main code here, to run repeatedly:

  tankSensorValue1_1 = analogRead(tankFloating_sen_1);
  tankSensorValue1_2 = analogRead(tankFloating_sen_2);
  tankSensorValue1_3 = analogRead(tankFloating_sen_3);
  tankSensorValue1_4 = analogRead(tankFloating_sen_4);
  tankSensorValue1_5 = analogRead(tankFloating_sen_5);

  Serial.println("----Sensor Values---- ");
  Serial.println();
  Serial.print("sensor 1: ");
  Serial.print(tankSensorValue1_1);
  Serial.println();
  Serial.print("sensor 2: ");
  Serial.print(tankSensorValue1_2);
  Serial.println();
  Serial.print("sensor 3: ");
  Serial.print(tankSensorValue1_3);
  Serial.println();
  Serial.print("sensor 4: ");
  Serial.print(tankSensorValue1_4);
  Serial.println();
  Serial.print("sensor 5: ");
  Serial.print(tankSensorValue1_5);
  Serial.println();
  //digitalWrite(led_1_pin, LOW);
  //digitalWrite(led_2_pin, LOW);
  //digitalWrite(led_3_pin, LOW);

  //tankSensorValue1_1 > sensorOn && tankSensorValue1_2 < sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn

  if (tankSensorValue1_1 < sensorOn && tankSensorValue1_2 < sensorOn ) {
    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);

    Serial.println("Tank er er tom");


  } else if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 < sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 12.5%");

  } else if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 25%");

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 50%");

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 > sensorOn && tankSensorValue1_5 < sensorOn) {
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 75%");
    
  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 > sensorOn && tankSensorValue1_5 > sensorOn) {
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, HIGH);
    Serial.println("Tank level er 100%");
  } else {
    Serial.println("sensor error");
  }


  
  delay(1000);
}


void setup() {
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

  // Start Serial port
  Serial.begin(115200);

  delay(10);
  Serial.println("serial has started");

}

void loop() {
  tankLevelCheck();

}
