#include <PubSubClient.h>
#include <WiFi101.h>
#include <ArduinoJson.h>


char ssid[] = "wlan_saltuaria";        // your network SSID (name)
char pass[] = "antlabpolitecnicomilano";    // your network password (use for WPA, or use as key for WEP)
const char broker[] = "54.68.43.185"; //broker IP address, you can use this one, the broker installed on your pc or a broker on the cloud
int        port     = 1883;
const char topic_pub[]  = "hackathon/team1/data";
const char topic_sub[]  = "hackathon/team1/ctrl";

const long interval = 1000;
unsigned long previousMillis = 0;

int count = 0;

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

WiFiClient wifiClient;
PubSubClient mqttClient(broker, port, callback, wifiClient);

/****** setup JSON array ******/
char JSONmessagebuffer[10000];
StaticJsonBuffer<10000>  JSONbuffer;
JsonObject& root = JSONbuffer.createObject();
//JsonArray& history = root.createNestedArray("count");  here an array inside a json is created. try to not bufferoverflow the json dimension when u add element to it



// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("A message has just received on topic ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect((char *) millis())) {
       Serial.println("CONNECTED");
      // Once connected, you can publish an announcement...
      //mqttClient.publish(topic_pub,"hello world");
      // or subscribe to a topic
      mqttClient.subscribe(topic_sub);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

}

void loop() {
  // call poll() regularly to allow the library to send MQTT keep alives which
  // avoids being disconnected by the broker
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  //use millis to avoid having delays in loop
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;


    //convert the integer in an array of char
    char to_char [50];
    sprintf(to_char, "%ld", count); 

    //create the json
    root["counter"] = to_char;
    //history.add(to_char); to add element to the array inside the json
    
    //print the json
    root.printTo(JSONmessagebuffer);

    // send the json
    mqttClient.publish(topic_pub, JSONmessagebuffer);
    Serial.println("message json sent");

    //clear the JSON buffer
    //JSONbuffer.clear();
    //JsonObject& root = JSONbuffer.createObject();
    //JsonArray& history = root.createNestedArray("count");


    count++;


  }
}

