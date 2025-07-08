// Device: VALTRACK_V4_MFW_ESP32_C3
#include <WiFi.h>
#include <WebServer.h>

#define SERIAL_PRINT
#define THEXOR // OCIO! comment out this line if you != me
#ifdef THEXOR
#include "/home/io/Vaults/Segretoni/include/wifi.h"
#else
const char *ssid = "TODO: SSID";                        // <<<<<<<<<<<<<<<<<<<<<<<<<
const char *password = "TODO: PASSWORD";                // <<<<<<<<<<<<<<<<<<<<<<<<<
const char *dnsProbe = "TODO: WIFI CONNECTIVITY CHECK"; // <<<<<<<<<<<<<<<<<<<<<<
#endif

const int outputPin = 2; // Pin connesso al mosfet/rele/ecc
const int inputPin = 3;  // pin di input (sensore se il device e' acceso)
unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 5000; // Check every 5 seconds
bool serverRunning = false;

WebServer server(80);

void sendStatus()
{
  if (digitalRead(inputPin) == LOW)
    server.send(200, "text/html", "0");
  else
    server.send(200, "text/html", "1");
}

void handleRoot()
{
  if (digitalRead(inputPin) == LOW)
    server.send(200, "text/html", "<p>Software version 2</p><h1>DSD status: OFF</h1><p><a href=\"/on\">Turn On</a></p>");
  else
    server.send(200, "text/html", "<p>Software version 2</p><h1>DSD status: ON</h1><p><a href=\"/off\">Turn Off</a></p>");
}

void handleStatus()
{
  sendStatus();
}

void commuta()
{
#ifdef SERIAL_PRINT
  Serial.println("___ PRESSING BUTTON (so to speak) ___");
#endif

  digitalWrite(outputPin, HIGH);
  delay(2000);
  digitalWrite(outputPin, LOW);
}

void handleOn()
{
  if (digitalRead(inputPin) == LOW)
    commuta();

  sendStatus();
}

void handleOff()
{
  if (digitalRead(inputPin) == HIGH)
    commuta();

  sendStatus();
}

void check_connection()
{
  unsigned long currentTime = millis();

  if (currentTime - lastWiFiCheck < wifiCheckInterval)
  {
    startWebServer();
    return; // Don't check too frequently
  }
  lastWiFiCheck = currentTime;

  if (WiFi.status() != WL_CONNECTED)
  {
    stopWebServer();

#ifdef SERIAL_PRINT
    Serial.println("**** CONNECTION LOST ****");
#endif

    WiFi.disconnect();
    WiFi.begin(ssid, password);
#ifdef SERIAL_PRINT
    Serial.println("Reconnecting to WiFi...");
#endif
    for (int k = 0; k < 10; k++)
    {
      if (WiFi.status() == WL_CONNECTED)
        break;
      delay(500);
#ifdef SERIAL_PRINT
      Serial.print(".");
#endif
    }
  }
}

void setup()
{
  pinMode(outputPin, OUTPUT);
  pinMode(inputPin, INPUT_PULLDOWN);
  digitalWrite(outputPin, LOW);

#ifdef SERIAL_PRINT
  Serial.begin(115200);
  Serial.println("WOL is starting");
  Serial.println("Connecting to WiFi...");
#endif

  WiFi.begin(ssid, password);
  for (int k = 0; k < 10; k++)
  {
    if (WiFi.status() == WL_CONNECTED)
      break;
    delay(500);
#ifdef SERIAL_PRINT
    Serial.print(".");
#endif
  }

  if (WiFi.status() == WL_CONNECTED)
  {
#ifdef SERIAL_PRINT
    Serial.println(" connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
#endif
  }
}

void stopWebServer()
{
  if (serverRunning)
  {
    server.close();
    serverRunning = false;
  }
}

void startWebServer()
{
  if (!serverRunning && WiFi.status() == WL_CONNECTED)
  {
    server.on("/", handleRoot);
    server.on("/on", handleOn);
    server.on("/off", handleOff);
    server.on("/status", handleStatus);

    server.begin();
    serverRunning = true;

#ifdef SERIAL_PRINT
    Serial.println("HTTP server started");
#endif
  }
}

void loop()
{
  if (serverRunning)
    server.handleClient();
  check_connection();
}
