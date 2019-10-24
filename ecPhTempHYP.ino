#include "DHT.h"
#include <DFRobot_PH.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "ArduinoJson.h"

#include <ThingsBoard.h>
#include <SPI.h> 


// Update these with values suitable for your network.
const char* ssid ="anil";
const char* password = "12345678";
const char* mqtt_server = "157.245.102.35";
#define mqtt_port 1883
#define MQTT_USER "dinothunder"
#define MQTT_PASSWORD ""
#define MQTT_SERIAL_PUBLISH_CH "v1/devices/me/telemetry"
#define MQTT_SERIAL_RECEIVER_CH "v1/devices/me/telemetry"



#define ONE_WIRE_BUS 34
#define DHTTYPE DHT22
#define DHTPIN 14
DFRobot_PH ph;
#define ESPADC 4096.0   //the esp Analog Digital Convertion value
#define ESPVOLTAGE 3300 //the esp voltage supply value
#define PH_PIN 35    //the esp gpi b o data pin number
float voltage, phValue, temperature = 25;
DHT dht(DHTPIN, DHTTYPE);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float Celcius=0;
float Fahrenheit=0;
DynamicJsonDocument doc(1024) ;
DynamicJsonDocument ht(512);
char charBuf[150] ;
int i=0,j=0;
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;


WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(),MQTT_USER,MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      Serial.println("Parsing start: ");
 
       //char JSONMessage[] = " {\"temp1\": \"111\", }"; //Original message
         //StaticJsonDocument<256> doc;
        //JsonObject& parsed = doc.parseObject(JSONMessage); //Parse message
        ///JsonObject root;
        //root["co"]=111;
        //Serial.println(root["co"]);
//        JsonObject& parsed = deserializeJson(doc, JSONMessage);


 
      client.publish("v1/devices/me/telemetry","{\"co\":111}");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(500);// Set time out for 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  reconnect();
      Serial.println(F("DHTxx test!"));
  ph.begin();
  dht.begin();
   sensors.begin();
 // dht.begin();
 // reconnect();
 // Ro = MQCalibration(MQ8);
}

void loop() {
   client.loop();
 

   doc["humiditya"]=dht22().substring(0,5);
   doc["tempa"] = dht22().substring(6,11);
   doc["watertemp"] =  water_temp();//== NULL ?float(0):water_temp() ;
   doc["ph"] = pH_value();//== NULL ?float(0):pH_value() ; 
     serializeJson(doc,charBuf);
    serializeJson(doc,Serial);
  client.publish("v1/devices/me/telemetry",charBuf);
  Serial.println(charBuf);
 // client.publish("v1/devices/me/telemetry",charBuf);
  
  delay(1000);
  Serial.println("Sent");
 }

 
/*float pH_value()
{
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) //time interval: 1s
  {
    timepoint = millis();
    //voltage = rawPinValue / esp32ADC * esp32Vin
    voltage = analogRead(PH_PIN) / ESPADC * ESPVOLTAGE; // read the voltage
    //Serial.print("voltage:");
    //Serial.println(voltage, 4);
   
    temperature = dht.readTemperature();  // read your temperature sensor to execute temperature compensation
    //Serial.print("temperature:");
    //Serial.print(temperature, 1);
    //Serial.println("^C");

    phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
    //Serial.print("pH:");
   // Serial.println(phValue, 4);
  }
  ph.calibration(voltage, temperature); // calibration process by Serail CMD
  Serial.print("pH:");
  Serial.println(phValue);
  return phValue;
 
  //client.publish(pubtopic, String(phValue).c_str(), true);
  //client.subscribe(subtopic);
  //delay(2000);
}*/

String dht22()
{
 
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  //float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) )
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return "None";
  }

  // Compute heat index in Fahrenheit (the default)
  //float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  //float hic = dht.computeHeatIndex(t, h, false);

 Serial.print(F("Humidity: "));
 Serial.println(h);
 Serial.print(F("Temperature: "));
 Serial.println(t);
 

 return String(String(h)+"-"+String(t)+"-");
  //Serial.print(F("°C "));
  //Serial.print(f);
 // Serial.println(F("°F "));
//  Serial.print(F(" Heat index: "));
 // Serial.print(hic);
 // Serial.print(F("°C "));
 // Serial.print(hif);
 // Serial.println(F("°F"));
  //client.publish(pubtopic, String(h).c_str(), true);  
  //client.publish(pubtopic, String(t).c_str(), true);
 // client.publish(pubtopic, String(f).c_str(), true);  
  //client.publish(pubtopic, String(hic).c_str(), true);
 // client.publish(pubtopic, String(hif).c_str(), true);
//  client.subscribe(subtopic);
 // delay(1000);
}


float water_temp()
{
  sensors.requestTemperatures();
  Celcius=sensors.getTempCByIndex(0);
  Fahrenheit=sensors.toFahrenheit(Celcius);
  //Serial.print("water_temp: ");
  //Serial.print(Celcius);
  //Serial.println("°C");
  return float(Celcius);
  /*Serial.print("water_temp: ");
  Serial.print(Fahrenheit);
  Serial.println("°F");
  client.publish(pubtopic, String(Celcius).c_str(), true);  
  client.publish(pubtopic, String(Fahrenheit).c_str(), true);
  client.subscribe(subtopic);
  delay(1000);*/
}

float pH_value() {
 for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(PH_PIN);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];
 float pHVol=(float)avgValue*5.0/1024/6;
 float phValue = -5.70 * pHVol + 21.34;
 Serial.print("sensor = ");
 Serial.println(phValue);

 delay(20);
}

 
