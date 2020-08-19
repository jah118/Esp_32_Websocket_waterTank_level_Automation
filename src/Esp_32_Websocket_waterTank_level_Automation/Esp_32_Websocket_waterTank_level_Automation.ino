
/////////////////////////////////////////////////////////////////
//                  ESP32 Project:                             //
//                                                             //
//            Author:  Jonas                                   //
//             Year:    2020                                   //
/////////////////////////////////////////////////////////////////

/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                   source:                                                                                                   //
  // https://shawnhymel.com/1882/how-to-create-a-web-server-with-websockets-using-an-esp32-in-arduino/                           //
  //                                                                                                                             //
  // https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/ this need to implenment at later stage             //
  //                                                                                                                             //
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/

/*Notes
   der mangler test af ekstr sensor til tank 2 så den stopper som den skal og fylder når den den.
   der skal laves ota upload

   der skal lave

*/


#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

/***********************************************************
   variables
************************************************************/

// debug if true prints data to Serial
bool debugPrint = false;                 //print values to serial

// makes sure serial is not spammed
unsigned long previousMillis = 0;        // will store last time it was updated
const long interval = 4000;           // interval at which to blink (milliseconds)

//BME280
Adafruit_BME280 bme; // I2C sat

// Constants
#define SEALEVELPRESSURE_HPA (1013.25)
unsigned long delayTime;

//switch pin
const int toggleSwitch_pin = 26;

int buttonState      = 0;     // current state of the button
int lastButtonState  = 0;     // previous state of the button

//relay pins
const int relay_1_pin = 33;
const int relay_2_pin = 25;

int relay_state = 0;
bool relaySafeToUse = false;


//Led pins
const int led_1_pin = 15;
const int led_2_pin = 16;
const int led_3_pin = 4;
const int led_4_pin = 17;
const int led_5_pin = 18;

//water level sensons pins
const int tankFloating_sen_1 = 36;
const int tankFloating_sen_2 = 39;
const int tankFloating_sen_3 = 34;
const int tankFloating_sen_4 = 35;
const int tankFloating_sen_5 = 32;

//Waterlevel sensor values
int sensorOn = 2400 ; //700 old
int tankSensorValue1_1 = 0;
int tankSensorValue1_2 = 0;
int tankSensorValue1_3 = 0;
int tankSensorValue1_4 = 0;
int tankSensorValue1_5 = 0;

//WaterLevel state
String waterLevelState = "Level_0";

//WaterLevel 2 tanks state
bool isWaterTankFull = false; // ------------------------------ dette er til andet sæt af sensor i 2 tank og den systemere.

// pin 14 /13 / 27 free pins

//water level sensons pins 2 tank
const int tankFloating_2_sen_1 = 27;
const int tankFloating_2_sen_2 = 14;
const int tankFloating_2_sen_3 = 13;

//Wifi
const char* ssid     = "Hnet_TP-LINK"; //_TP-LINK
const char* password = "1PoulThomsen";

// Globals
const char* PARAM_MESSAGE = "message";
String header;
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];


/***********************************************************
   Functions
************************************************************/

//--------WaterLevel / sensor Functions --------//

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
  unsigned long currentMillis = millis();

  tankSensorValue1_1 = analogRead(tankFloating_sen_1);
  tankSensorValue1_2 = analogRead(tankFloating_sen_2);
  tankSensorValue1_3 = analogRead(tankFloating_sen_3);
  tankSensorValue1_4 = analogRead(tankFloating_sen_4);
  tankSensorValue1_5 = analogRead(tankFloating_sen_5);

  //tankSensorValue1_1 > sensorOn && tankSensorValue1_2 < sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn

  if (tankSensorValue1_1 < sensorOn ) {
    relay_state = 0;
    relaySafeToUse  = false;
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank er er tom SAFETY RELAY OFFF");
    }
  }

  if (tankSensorValue1_1 < sensorOn && tankSensorValue1_2 < sensorOn ) {
    relay_state = 0;
    relaySafeToUse  = false;
    waterLevelState = "Level_0";
    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank er er tom");
    }

  } else if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 < sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    //relay_state = 0;
    relaySafeToUse  = true;
    waterLevelState = "Level_12.5";
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank level er 12.5%");
    }

  } else if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    //relay_state = 0;
    relaySafeToUse  = true;
    waterLevelState = "Level_25";
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank level er 25%");
    }

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    relaySafeToUse  = true;
    waterLevelState = "Level_50";
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank level er 50%");
    }

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 > sensorOn && tankSensorValue1_5 < sensorOn) {
    relaySafeToUse  = true;
    waterLevelState = "Level_75";
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, LOW);
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank level er 75%");
    }

  } else  if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 > sensorOn  && tankSensorValue1_3 > sensorOn && tankSensorValue1_4 > sensorOn && tankSensorValue1_5 > sensorOn) {
    relaySafeToUse  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, HIGH);
    waterLevelState = "Level_100";
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      Serial.println("Tank level er 100%");
    }

  } else {
    relay_state = 0;
    relaySafeToUse  = false;
    waterLevelState = "Level_error";

    Serial.println("sensor error");

    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    delay(200);
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, HIGH);
    digitalWrite(led_3_pin, HIGH);
    digitalWrite(led_4_pin, HIGH);
    digitalWrite(led_5_pin, HIGH);
    delay(200);
    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("sensor error");
  }
}

//----------------------------------------------------------------------\\


//Read Temperature sensor and returns values as a string.
String readBME280Temperature() {
  // Read temperature as Celsius (the default)
  float t = bme.readTemperature();
  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  if (isnan(t)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    return String(t);
  }
}

//Read Humidity sensor and returns values as a string.
String readBME280Humidity() {
  float h = bme.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    return String(h);
  }
}

//Read Pressure sensor and returns values as a string.
String readBME280Pressure() {
  float p = bme.readPressure() / 100.0F;
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    return String(p);
  }
}


void printBMEValues() {
  delay(1000);
  Serial.println();
  Serial.println();

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

//--------Websocket Functions --------//

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch (type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:
      // Print out raw message
      Serial.printf("[%u] Received text: %s\n", client_num, payload);

      // Toggle LED
      if ( strcmp((char *)payload, "toggleLED") == 0 ) {
        relay_state = relay_state ? 0 : 1;
        Serial.printf("Toggling relay to %u\n", relay_state);
        digitalWrite(relay_1_pin, relay_state);

        // Report the state of the LED
      } else if ( strcmp((char *)payload, "getLEDState") == 0 ) {
        sprintf(msg_buf, "%d", relay_state);
        Serial.printf("Sending to [%u]: %s\n", client_num, msg_buf);

        webSocket.sendTXT(client_num, msg_buf);

      } else if (strcmp((char *)payload, "/getTemperature") == 0)  {
        String d = readBME280Temperature();

        // allocate the memory for the document
        const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
        StaticJsonDocument<capacity> doc;

        // create an object
        JsonObject obj = doc.to<JsonObject>();
        obj["SensorType"] = "Temperature";
        obj["value"] = d;

        char  buffer[200]; // create temp buffer
        size_t len = serializeJson(obj, buffer);  // serialize to buffer

        webSocket.sendTXT(client_num, buffer, len);

      } else if (strcmp((char *)payload, "/getHumidity") == 0)  {
        String h = String(readBME280Humidity());

        // allocate the memory for the document
        const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
        StaticJsonDocument<capacity> docs;

        // create an object
        JsonObject obj = docs.to<JsonObject>();
        obj["SensorType"] = "Humidity";
        obj["value"] = h;

        char  buffers[200]; // create temp buffer
        size_t len = serializeJson(obj, buffers);  // serialize to buffer

        webSocket.sendTXT(client_num, buffers, len);

      } else if (strcmp((char *)payload, "/getWaterLevel") == 0) {
        String w = waterLevelState;

        // allocate the memory for the document
        const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
        StaticJsonDocument<capacity> docs;

        // create an object
        JsonObject obj = docs.to<JsonObject>();
        obj["SensorType"] = "Water Level";
        obj["value"] = w;

        char  buffers[200]; // create temp buffer
        size_t len = serializeJson(obj, buffers);  // serialize to buffer

        webSocket.sendTXT(client_num, buffers, len);

      } else if (strcmp((char *)payload, "/getPressure") == 0) {
        String p = String(readBME280Pressure());//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk

        // allocate the memory for the document
        const size_t capacity = JSON_OBJECT_SIZE(2) + 60;
        StaticJsonDocument<capacity> docs;

        // create an object
        JsonObject obj = docs.to<JsonObject>();
        obj["SensorType"] = "Pressure";
        obj["value"] = p;

        char  buffers[200]; // create temp buffer
        size_t len = serializeJson(obj, buffers);  // serialize to buffer
        webSocket.sendTXT(client_num, buffers, len);
      } else  {
        // Message not recognized
        Serial.println("[%u] Message not recognized");
      }
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}

/***********************************************************
                          Main
************************************************************/


void setup() {



  //Init Switch
  pinMode(toggleSwitch_pin, INPUT);

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



  bool status;

  // Start Serial portw
  Serial.begin(115200);
  delay(150);
  Serial.println();
  Serial.println("Serial is begining");
  while (!Serial) // Waiting for serial connection
  {
    Serial.println(".");
  }
  delay(100);
  Serial.println("serial has started");

  // Make sure we can read the file system
  if ( !SPIFFS.begin(true)) {
    Serial.println("Error mounting SPIFFS");
    while (1);
  }

  // Start access point
  //  WiFi.softAP(ssid, password);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  //Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);
  // noty use as server is the same as were data is created, so same data.
  //  server.on("/getTemperature", HTTP_GET, [](AsyncWebServerRequest * request) {
  //    request->send_P(200, "text/plain", readBME280Temperature().c_str());
  //  });
  //  server.on("/getHumidity", HTTP_GET, [](AsyncWebServerRequest * request) {
  //    request->send_P(200, "text/plain", readBME280Humidity().c_str());
  //  });
  //  server.on("/getPressure", HTTP_GET, [](AsyncWebServerRequest * request) {
  //    request->send_P(200, "text/plain", readBME280Pressure().c_str());
  //  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE)) {
      message = request->getParam(PARAM_MESSAGE)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, GET: " + message);
  });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest * request) {
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
      message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
      message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
  });

  //handle error and runs funktion notFopund
  server.onNotFound(notFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);


  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  //rates ande code from https://github.com/adafruit/Adafruit_BME280_Library/blob/master/examples/advancedsettings/advancedsettings.ino
  // and bme280 doc

  // indoor navigation
  Serial.println("-- Indoor Navigation Scenario --");
  Serial.println("normal mode, 16x pressure / 2x temperature / 1x humidity oversampling,");
  Serial.println("0.5ms standby period, filter 16x");
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X16,  // temperature
                  Adafruit_BME280::SAMPLING_X16, // pressure
                  Adafruit_BME280::SAMPLING_X16,  // humidity
                  Adafruit_BME280::FILTER_X16,                 Adafruit_BME280::STANDBY_MS_0_5 );

  // suggested rate is 25Hz
  // 1 + (2 * T_ovs) + (2 * P_ovs + 0.5) + (2 * H_ovs + 0.5)
  // T_ovs = 2
  // P_ovs = 16
  // H_ovs = 1
  // = 40ms (25Hz)
  // with standby time that should really be 24.16913... Hz
  delayTime = 41;


  relay_state = 0;
  relaySafeToUse  = false;
  delay(1000);

}

void loop() {
  // Look for and handle WebSocket data
  webSocket.loop();
  //Checks were sensors is at and  sets level values og make sure there is water when the pump runs
  tankLevelCheck();
  //Print all sensor values as fast as serial
  if (debugPrint) {
    // currentMillis varible is used to keep count of time passed so  serial is not spammed
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      // save the last time sent to serial;
      previousMillis = currentMillis;
      printTankLevelSensorValue(); // debug for senvalues
      printBMEValues();
      delay(delayTime);
      bool cState = relay_state;
      if (relay_state > 0 ) {
        Serial.println("state is  ON");
      } else {
        Serial.println("state is  OFF");
      }
    }
  }

  // read the Switchs input pin
  buttonState = digitalRead(toggleSwitch_pin);

  if (buttonState != lastButtonState) {

    // change the state of the led when someone flip the switch
    if (buttonState == 1 && relaySafeToUse == true && isWaterTankFull == false) {
      Serial.println("Switching ON");
      relay_state = 1;
    } else if (buttonState == LOW) {
      Serial.println("Switching OFF");
      relay_state = 0;
    }
    // remember the current state of the button
    lastButtonState = buttonState;
  }

  //Change the state of the relays 
  if (relay_state > 0 && relaySafeToUse == true && isWaterTankFull == false) {
    digitalWrite(relay_1_pin, HIGH); // HIGH tænder
    digitalWrite(relay_2_pin, HIGH); // HIGH tænder
  } else {
    digitalWrite(relay_1_pin, LOW); // LOW slukker
    digitalWrite(relay_2_pin, LOW); // LOW slukker
  }


}
