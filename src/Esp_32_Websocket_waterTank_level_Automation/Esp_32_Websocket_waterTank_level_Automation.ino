
/////////////////////////////////////////////////////////////////
//                  ESP32 Project:                             //
//                                                             //
//            Author:  Jonas                                   //
//             Year:    2020                                   //
/////////////////////////////////////////////////////////////////


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


Adafruit_BME280 bme; // I2C sat
// Constants
#define SEALEVELPRESSURE_HPA (1013.25)


// Sensor
//unsigned long delayTime; //delete later
//float temperatureP = 0;
//float humidityP = 0;
//float pressureP = 0;
////float PrintInSerial = 1;

//relay pins
const int relay_1_pin = 33;
const int relay_2_pin = 25;

int led_state = 0; // dette bruge til simulere relay
const int led_pin = 15; // dette bruge til simulere relay i websocket.

bool relayState;
bool relaySAFETYState;

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

//Wifi
char* ssid     = "Hnet_TP-LINK"; //_TP-LINK
char* password = "1PoulThomsen";


// Globals
const char* PARAM_MESSAGE = "message";
String header;
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);
char msg_buf[10];

String temps = "0";


//mangler alle funk fra andet sript


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

  if (tankSensorValue1_1 < sensorOn && tankSensorValue1_2 < sensorOn ) {
    relayState = false;
    relaySAFETYState  = false;
    digitalWrite(led_1_pin, LOW);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank er er tom");

  } else if (tankSensorValue1_1 > sensorOn && tankSensorValue1_2 < sensorOn  && tankSensorValue1_3 < sensorOn && tankSensorValue1_4 < sensorOn && tankSensorValue1_5 < sensorOn) {
    relayState = false;
    relaySAFETYState  = true;
    digitalWrite(led_1_pin, HIGH);
    digitalWrite(led_2_pin, LOW);
    digitalWrite(led_3_pin, LOW);
    digitalWrite(led_4_pin, LOW);
    digitalWrite(led_5_pin, LOW);
    Serial.println("Tank level er 12.5%");

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


//----------------------------------------------------------------------

String readBME280Temperature() {
  // Read temperature as Celsius (the default)
  float t = bme.readTemperature();
  temps = t;
  // Convert temperature to Fahrenheit
  //t = 1.8 * t + 32;
  if (isnan(t)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(t);
    return String(t);
  }
}

String readBME280Humidity() {
  float h = bme.readHumidity();
  if (isnan(h)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(h);
    return String(h);
  }
}

String readBME280Pressure() {
  float p = bme.readPressure() / 100.0F;
  if (isnan(p)) {
    Serial.println("Failed to read from BME280 sensor!");
    return "";
  }
  else {
    Serial.println(p);
    return String(p);
  }
}

void printSensor() {
  Serial.println(readBME280Temperature());
  Serial.println(readBME280Humidity());
  Serial.println(readBME280Pressure());
}

void printBMEValues() {
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
        led_state = led_state ? 0 : 1;
        Serial.printf("Toggling LED to %u\n", led_state);
        digitalWrite(led_pin, led_state);

        // Report the state of the LED
      } else if ( strcmp((char *)payload, "getLEDState") == 0 ) {
        sprintf(msg_buf, "%d", led_state);
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

      } else if (strcmp((char *)payload, "/getPressure") == 0) {
        String p = String(readBME280Pressure());

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

  server.on("/getTemperature", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Temperature().c_str());
  });
  server.on("/getHumidity", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Humidity().c_str());
  });
  server.on("/getPressure", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/plain", readBME280Pressure().c_str());
  });

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

  relayState = false;
  relaySAFETYState  = false;

}

void loop() {
  tankLevelCheck();
  printTankLevelSensorValue(); // debug for senvalues
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

  // Look for and handle WebSocket data
  webSocket.loop();

  delay(500);

}
