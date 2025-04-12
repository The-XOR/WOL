// Device: valtrack esp32-c3
#include <WiFi.h>
#include <WebServer.h>

const char *ssid = "TODO: SSID";  // <<<<<<<<<<<<<<<<<<<<<<<<<
const char *password = "TODO: PASSWORD"; // <<<<<<<<<<<<<<<<<<<<<<<<<

const int outputPin = 2; // Pin connesso al mosfet/rele/ecc
const int inputPin = 3;   // pin di input (sensore se il device e' acceso)

WebServer server(80);

void handleRoot() {
  if(digitalRead(inputPin) == LOW)
    server.send(200, "text/html", "<h1>DSD status: OFF</h1><p><a href=\"/on\">Turn On</a></p>");
  else
    server.send(200, "text/html", "<h1>DSD status: ON</h1><p><a href=\"/off\">Turn Off</a></p>");
}

void sendStatus()
{
  if(digitalRead(inputPin) == LOW)
    server.send(200, "text/html", "DSD status: OFF");
  else
    server.send(200, "text/html", "DSD status: ON");
}

void commuta()
{
    digitalWrite(outputPin, HIGH);
    delay(2000);
    digitalWrite(outputPin, LOW);
}
void handleOn() 
{
  if(digitalRead(inputPin) == LOW)
    commuta();
  
  sendStatus();
}

void handleOff() {
  if(digitalRead(inputPin) == HIGH)
    commuta();

  sendStatus();
}

void setup() {
  //Serial.begin(115200);
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT_PULLDOWN);
  digitalWrite(outputPin, LOW);

  WiFi.begin(ssid, password);
  //Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println(" connected!");
  //Serial.print("IP Address: ");
  //Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  server.begin();
  //Serial.println("HTTP server started");
}

void loop() 
{
  server.handleClient();
}
