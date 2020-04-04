 // Import required libraries 
#include "ESP8266WiFi.h"  
#include <aREST.h>  
int relayInput = 2;
const int sensorIn = A0;
int mVperAmp = 185; // use 185 for 5A, 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
  
 
// Create aREST instance  
aREST rest = aREST(); 
 
 
// WiFi parameters  
const char* ssid = "wifi-name";  
const char* password = "wifi-pass"; 
 
// The port to listen for incoming TCP connections  
#define LISTEN_PORT 80
 
// Create an instance of the server  
WiFiServer server(LISTEN_PORT); 

// Variables to be exposed to the API 
 
void setup(void) 
{ 
// Start Serial
pinMode(relayInput, OUTPUT); // initialize pin as OUTPUT  
pinMode(A0, INPUT);// intialize pin as input
Serial.begin(115200);
delay(10);
Serial.println(F("Init...."));

// Init variables and expose them to REST API  
rest.variable("Voltage",&Voltage);  
rest.variable("VRMS",&VRMS); 
 
// Give name and ID to device  
rest.set_id("1");  
rest.set_name("esp8266"); 
 
// Connect to WiFi  
WiFi.begin(ssid, password);  
while (WiFi.status() != WL_CONNECTED) {  
delay(500);  
Serial.print("."); }
 
Serial.println("");  
Serial.println("WiFi connected"); 
 
// Start the server  
server.begin();  
Serial.println("Server started");
  
// Print the IP address  
Serial.println(WiFi.localIP()); }
 
void loop() {
Voltage = getVPP();
VRMS = (Voltage/2.0) *0.707; // sq root
AmpsRMS = (VRMS * 1000)/mVperAmp;
float Wattage = (220*AmpsRMS)-18; //Observed 18-20 Watt when no load was connected, so substracting offset value to get real consumption.
Serial.print(AmpsRMS);
Serial.println(" Amps RMS ");
Serial.print(Wattage);
Serial.println(" Watt ");

// Handle REST calls  
WiFiClient client = server.available();  
if (!client) {  
return; 
}
 
while(!client.available()){  
delay(1);
 
}
 
rest.handle(client); 
}
float getVPP()
{
float result;

int readValue; //value read from the sensor
int maxValue = 0; // store max value here
int minValue = 1024; // store min value here

uint32_t start_time = millis();

while((millis()-start_time) < 1000) //sample for 1 Sec
{
readValue = analogRead(sensorIn);
// see if you have a new maxValue
if (readValue > maxValue)
{
/*record the maximum sensor value*/
maxValue = readValue;
}
if (readValue < minValue)
{
/*record the maximum sensor value*/
minValue = readValue;
}
/* Serial.print(readValue);
Serial.println(" readValue ");
Serial.print(maxValue);
Serial.println(" maxValue ");
Serial.print(minValue);
Serial.println(" minValue ");
delay(1000); */
}

// Subtract min from max
result = ((maxValue - minValue) * 5)/1024.0;

return result;
}
