#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "ESP8266WebServer.h"

extern "C" {
#include "user_interface.h"
}

#define Start_Stop 13

WiFiServer server(80);

void setup()
{
  pinMode(Start_Stop, OUTPUT);
    
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  
  IPAddress localIp(192,168,1,1);
  IPAddress gateway(192,168,1,1);
  IPAddress subnet(255,255,255,0);

  Serial.begin(115200);
  Serial.println();
  WiFi.softAPConfig(localIp, gateway, subnet);
  boolean result = WiFi.softAP("ESPap","Ciacho");
  Serial.print("Setting soft-AP ... ");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Start the server
  server.begin();
  Serial.println("\nServer started");
}

void loop()
{
  WiFiClient client = server.available();       // Sprawdzenie czy klient połączył się z serwerem
  if (!client) {
    return;
  } 

  // Wait until the client sends some data
  while(!client.available())
  {
    delay(1);
  }
 
  String data = checkClient (client);           // Odczytanie żądania klienta
  client.println("HTTP/1.1 200 OK");            // Odesłanie odpowiedzi
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("");
  client.println("");

  if (data == "Start") start();               // Sprawdzenie czy otrzymano komendę 
  else if (data == "Stopp") stopp();          // do włączenia lub wyłączenia platformy
  client.stop();
}

/***RECEIVE DATA FROM the APP ***/
String checkClient (WiFiClient Client_temp)
{
  String request = Client_temp.readStringUntil('\0');
  request.remove(0, 5);
  request.remove(5);
  return request;
}

void start()
{
  digitalWrite(Start_Stop,HIGH);
}

void stopp()
{
  digitalWrite(Start_Stop,LOW);
}
