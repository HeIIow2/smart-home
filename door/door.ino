#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define BUTTON_PIN 5


void setup()
{
  pinMode(BUTTON_PIN, OUTPUT);

  // wlan data
  char ssid[32] = "NETGEAR2";
  char passphrase[64] = "R8EUmsqcVD9a8T";

  Serial.begin(9600);         // Serielle schnittstelle initialisieren
  EEPROM.begin(64);             // EEPROM initialisieren mit 64 Byts

  // Wlan-Zugang konfigurieren
  WiFi.mode(WIFI_STA);        
  WiFi.begin(ssid, passphrase);
  WiFi.printDiag(Serial);       // Wlan Daten seriell ausgeben

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");         
  Serial.print("Verbunden mit ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());// Aktuelle IP des ESP8266-servers zeigen
}


void loop()         
{
  bool door_changed = digitalRead(BUTTON_PIN);
  Serial.println(door_changed);
}
