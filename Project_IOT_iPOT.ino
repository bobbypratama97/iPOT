// Menambahkan library yang dibutuhkan
#include <HygrometerSensor.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#define RELAY_ON 0
#define RELAY_OFF 1
#define RELAY_1 D2
// Create Analog rain drop sensor (or moisture sensor) instance on D4
HygrometerSensor analog_rain_drop(HygrometerSensor::ANALOG, D4);
// Declare ssid and password for WiFi Connectify
char ssid[] = "AndroidAP";
char pass[] = "678760000";

// This method using for request API to watch iPOT status.
void httpClientWiFi() {
  if (WiFi.status() == WL_CONNECTED) {                      // Check WiFi connection status 
    HTTPClient http;                                        //Declare an object of class HTTPClient
    
    http.begin("http://ipot.satyadara.com/action-get.php"); //Specify request destination
    int httpCode = http.GET();                              //Send the request
    
    if (httpCode > 0) {                                     //Check the returning code
      String payload = http.getString();                    //Get the request response payload
      Serial.print("Action : ");
      Serial.println(payload);                              //Print the response payload
      
      if (payload == "1") {                                 //Check payload value
        digitalWrite(RELAY_1, RELAY_ON);                    //Turn relay on
        delay(2000);
        digitalWrite(RELAY_1, RELAY_OFF);                   //Turn relay off
        http.begin("http://ipot.satyadara.com/action-off.php");//Specify request destination
        int httpCodex = http.GET();                         //Send the request
      }
    }
    http.end();                                             //Close connection
  }
}

// This method using for set status pot to wet
void setStatusWet() {
  if (WiFi.status() == WL_CONNECTED) {                      //Check WiFi connection status
    HTTPClient http;                                        //Declare an object of class HTTPClient
    
    http.begin("http://ipot.satyadara.com/status-wet.php"); //Specify request destination
    int httpCode = http.GET();                              //Send the request
    http.end();                                             //Close connection
  }
}

// This method using for set status pot to dry
void setStatusDry() {
  if (WiFi.status() == WL_CONNECTED) {                      //Check WiFi connection status
    HTTPClient http;                                        //Declare an object of class HTTPClient
 
    http.begin("http://ipot.satyadara.com/status-dry.php"); //Specify request destination
    int httpCode = http.GET();                              //Send the request
    
    http.end();                                             //Close connection
  }
}

void setup(void)
{
  Serial.begin(9600);
  pinMode(RELAY_1, OUTPUT);         //set pin as output
  digitalWrite(RELAY_1, RELAY_OFF); //initialize relay one as off
  
  WiFi.begin(ssid, pass);           //Connect WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..\n");
  }

  // Set analog parameters (min value, max value, value to switch from "dry" to "is raining") (optional call)
  if (!analog_rain_drop.setAnalogParameters(ANALOG_HUMIDITY_MIN, ANALOG_HUMIDITY_MAX, 800)) {
    Serial.print("Error while setting Analog parameters\n");
  }
  // Get analog parameters (optional call)
  int min, max, is_raining;
  analog_rain_drop.getAnalogParameters(min, max, is_raining);
  Serial.print("Minimum analog value: ");
  Serial.print(min, DEC);
  Serial.print("\nMaximum analog value: ");
  Serial.print(max, DEC);
  Serial.print("\nValue used as switch from 'dry' to 'is raining': ");
  Serial.print(is_raining, DEC);
  Serial.print("\n");
}

void loop()
{
  // Check rain state and value every 3 sec

  Serial.print("Rain drop value: ");
  Serial.print(analog_rain_drop.readHumidityValue());
  Serial.print(" (");
  if (analog_rain_drop.isHumid()) {
    Serial.print("raining");
    digitalWrite(RELAY_1, RELAY_OFF);
    setStatusWet();
  } else {
    Serial.print("dry");
    digitalWrite(RELAY_1, RELAY_ON);
    delay(2000);
    digitalWrite(RELAY_1, RELAY_OFF);
    setStatusDry();
  }
  Serial.print(")\n");
  
  httpClientWiFi();
  
  delay(3000);
}
