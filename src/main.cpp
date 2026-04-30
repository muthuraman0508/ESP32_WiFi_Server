#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include <WiFiAP.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2  // Set the GPIO pin where you connected your test LED
#endif

// Set these to your desired credentials.
const char *ssid = "Zed";
const char *password = "Wireless4U!";

// 1. CHANGED: NetworkServer is now WiFiServer
WiFiServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println();
  
  // Set ESP32 to connect to a router (Station Mode)
  WiFi.mode(WIFI_STA);
  
  Serial.print("Connecting to WiFi network: ");
  Serial.println(ssid);

  // Try to connect to your home WiFi
  WiFi.begin(ssid, password);

  // Wait until it successfully connects
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected successfully!");
  
  // Print the IP address your home router assigned to the ESP32
  Serial.print("ESP32 IP address: ");
  Serial.println(WiFi.localIP());
  
  server.begin();
  Serial.println("Server started");
}
void loop() {
  // 2. CHANGED: NetworkClient is now WiFiClient, and server.accept() is server.available()
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");

            client.println();
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}