#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "DHT.h"


// See
// https://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/

// Note about pin mappings
// static const uint8_t D0   = 16;
// static const uint8_t D1   = 5;
// static const uint8_t D2   = 4;
// static const uint8_t D3   = 0;
// static const uint8_t D4   = 2;
// static const uint8_t D5   = 14;
// static const uint8_t D6   = 12;
// static const uint8_t D7   = 13;
// static const uint8_t D8   = 15;
// static const uint8_t D9   = 3;
// static const uint8_t D10  = 1;

const char* ssid = "horsekilden8";
const char* password = "whatmeworry";
 
#define LED_PIN 13 // D7
#define DHT_PIN 2 // D4
#define DHT_TYPE DHT11

WiFiServer server(80);
DHT dht(DHT_PIN, DHT_TYPE);

int ledValue = LOW;
int dht_status;
float dht_h;
float dht_t;
float dht_hic;
// float dht_f;
// float dht_hif;


void setup() {
    Serial.begin(115200);
    delay(10);
  
    pinMode(LED_PIN, OUTPUT);
    ToggleLED(LOW);
  
    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  
    WiFi.begin(ssid, password);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
  
    // Start the server
    server.begin();
    Serial.println("Server started");
  
    // Print the IP address
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/"); 

    dht_status = 0;
}

void loop() {
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    // Wait until the client sends some data
    Serial.println("new client");
    while(!client.available()){
        delay(1);
    }

    // Read the first line of the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    if (request.indexOf("/LED=ON") != -1)  {
        ToggleLED(HIGH);
    }
    else if (request.indexOf("/LED=OFF") != -1)  {
        ToggleLED(LOW);
    }
    else if (request.indexOf("/MEASURE") != -1)  {
        ReadDHT();
    }

    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println(""); //  do not forget this one
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");

    client.print("Led pin is now: ");

    if (ledValue == HIGH) {
        client.print("On");
    } else {
        client.print("Off");
    }
    client.println("<br><br>");
    client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
    client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  

    client.println("<br><br>");
    if (dht_status == 0) 
    {
        client.println("No measurement yet");
    }
    else if (dht_status == 1) 
    {
        client.println("Failed to read from DHT sensor!");
    }
    else
    {
        client.print("Humidity: ");
        client.print(dht_h);
        client.print(" %");
        client.println("<br>");
        client.print("Temperature: ");
        client.print(dht_t);
        client.print(" *C ");
        client.println("<br>");
        client.print("Heat index: ");
        client.print(dht_hic);
        client.print(" *C ");
    }

    client.println("<br><br>");
    client.println("<a href=\"/MEASURE\"\"><button>New measurement</button></a>");

    client.println("</html>");
 
    delay(1);
    Serial.println("Client disonnected");
    Serial.println(""); 
}

void ToggleLED(int value)
{
    digitalWrite(LED_PIN, value);
    ledValue = value;
}


void ReadDHT() {    

    // Wait a few seconds between measurements.
    delay(500);
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    dht_h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    dht_t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    // dht_f = dht.readTemperature(true);
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(dht_h) || isnan(dht_t)) {
      dht_status = 1;
      return;
    }
  
    // Compute heat index in Fahrenheit (the default)
    // dht_hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    dht_hic = dht.computeHeatIndex(dht_t, dht_h, false);

    dht_status = 2;
}



 