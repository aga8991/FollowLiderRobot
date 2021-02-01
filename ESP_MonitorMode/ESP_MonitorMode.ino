#include <ESP8266WiFi.h>
#include "./functions.h"

#define disable 0
#define enable  1
unsigned int channel = 1;
int client_channel = 0;
int time1 = 0;

void setup()
{
  Serial.begin(115200);
  Serial.printf("\n\nSDK version:%s\n\r", system_get_sdk_version());

  wifi_set_opmode(STATION_MODE);            // Promiscuous works only with station mode
  wifi_set_channel(channel);     
  wifi_promiscuous_enable(disable);
  wifi_set_promiscuous_rx_cb(promisc_cb);   // Set up an Rx callback function in promiscuous mode, which will be called when data packet is received
  wifi_station_disconnect();                // needed before wifi_promiscuous_enable
  wifi_promiscuous_enable(enable);          // Enable promiscuous mode for sniffer, only for atation mode
}

void loop() {
  while (client_channel == 0)            // Pętla wykonywana dopóki klient nie zostanie znaleziony
  {
    channel = 0;
    while (true) 
    {
       channel++;
       if (channel == 14) break;         // Skanowanie kanałów 1-14
       wifi_set_channel(channel);        // Ustawianie kanału
    }
      delay(1);
    }
    if ((Serial.available() > 0) && (Serial.read() == '\n')) 
    {
       client_channel = print_client(client_mobile_device);       // Funkcja print_client zwraca wartość kanału, 
       if (client_channel != 0) wifi_set_channel(client_channel); // o ile klientem jest wybrane urządzenie mobilne 
       delay(1);                                                  // lub 0 w przypadku znalezienia innego klienta
    }
    //for (int u = 0; u < aps_known_count; u++) print_beacon(aps_known[u]);
}
