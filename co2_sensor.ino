
#include <ESP8266WiFi.h>


String apiKey = "0E0ZK6ACF6Y3ZXLF";     //  Enter your Write API key from ThingSpeak

const char *ssid =  "wlan_saltuaria";     // replace with your wifi ssid and wpa2 key
const char *pass =  "antlabpolitecnicomilano";
const char* server = "api.thingspeak.com";

const int analog_ip = A0; //Naming analog input pin
int co2  = 0;        //Variable to store analog input values
long rssi = 0;
int count = 0;

WiFiClient client;

//=================================================================
//============================= Creazione della connessione wifi


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  Serial.begin(9600);    // Initiating Serial communication

  delay(10);
  Serial.println("Connecting to ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) 
  {
      delay(500);
      Serial.print(".");
   }
      Serial.println("");
      Serial.println("WiFi connected");

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off
  
  co2 = analogRead (analog_ip); // Analog Values 0 to 1023
  rssi = WiFi.RSSI();         // read the rssi

  if (client.connect(server,80))   //   api.thingspeak.com
  {
    
    String postStr = apiKey;
    postStr +="&field1=";
    postStr += String(co2);
    postStr +="&field2=";
    postStr += String(rssi);
    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.print("CO2: ");
    Serial.print(co2);
    Serial.println(". Send to Thingspeak.");
    count +=1;

  }

  client.stop();
  delay(60000);                      

  if(count%100==0){
    digitalWrite(LED_BUILTIN, LOW);   
    delay(1000); 
  }
}

