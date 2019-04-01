#include <PubSubClient.h>
#include <WiFi101.h> 

char ssid[] = "wlan_saltuaria";        // your network SSID (name)
char pass[] = "antlabpolitecnicomilano";    // your network password (use for WPA, or use as key for WEP)
const char broker[] = "54.68.43.185";
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
      Serial.println(" try again in 2 seconds");
      // Wait 5 seconds before retrying
      delay(2000);
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
  // check connection regularly to allow the library to send MQTT keep alives which
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
    
    // send the message
    mqttClient.publish(topic_pub, to_char); //the first field is the topic, the second one the message you want to send (it accepts only char [])
    Serial.println("message sent");

    count++;
  }
}

