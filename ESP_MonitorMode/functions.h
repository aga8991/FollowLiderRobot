/*____"functions.h"___*/

// Espressif SDK functionality
extern "C" 
{
#include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);               //callback, parameter status: 0 — packet sending succeeds; otherwise — failed
  int  wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);  //register a callback for sending user-defined 802.11 packets ^
  void wifi_unregister_send_pkt_freedom_cb(void);
  int  wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);    //send user-defined 802.11 packets (802.11 - standard packet, basics of wi-fi certificates)
}

#include <ESP8266WiFi.h>
#include "./structures.h"

#define MAX_APS_TRACKED 100

beaconinfo aps_known[MAX_APS_TRACKED];                    // Array to save MACs of known APs
int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
clientinfo client_mobile_device;                       
int clients_known_count = 0;                              // Number of known CLIENTs

int register_beacon(beaconinfo beacon)
{
  int known = 0;   // Clear known flag
  for (int u = 0; u < aps_known_count; u++)
  {
    if (! memcmp(aps_known[u].bssid, beacon.bssid, ETH_MAC_LEN)) 
    {
      known = 1;
      break;
    }   // AP known => Set known flag
  }
  if (! known)  // AP is NEW, copy MAC to array and return it
  {
    memcpy(&aps_known[aps_known_count], &beacon, sizeof(beacon));
    aps_known_count++;

    if ((unsigned int) aps_known_count >=
        sizeof (aps_known) / sizeof (aps_known[0]) ) 
        {
          //Serial.printf("exceeded max aps_known\n");
          aps_known_count = 0;
        }
  }
  return known;
}

int register_client(clientinfo ci)  // clientinfo to struktura zawierająca parametry klientów
{
  char bssid[12];
  sprintf(bssid, "%02x%02x%02x%02x%02x%02x", ci.station[0], ci.station[1], ci.station[2], ci.station[3], ci.station[4], ci.station[5]);

  if (strcmp(bssid, "38fb1451a28a") != 0) return 0;   // Porównanie BSSID znalezionego klienta z BSSID śledzonego urządzenia 
  else                                                // i wyjście z funkcji w przypadku kiedy wartości nie są równe
  {
    memcpy(&client_mobile_device, &ci, sizeof(ci));   // Przypisanie wartości BSSID śledzonego urządzenia zmiennej pomocniczej
    return 1;                                         // w przypadku kiedy wartości BSSID są równe
  }
}

int print_client(clientinfo ci)
{
  if (ci.err != 0) return 0;       // Wyjście z funkcji w przypadku błędnego połączenia z klientem 
  else 
  {
    char bssid[12];
    sprintf(bssid, "%02x%02x%02x%02x%02x%02x", ci.station[0], ci.station[1], ci.station[2], ci.station[3], ci.station[4], ci.station[5]);

    if (strcmp(bssid, "38fb1451a28a") != 0) return 0;  // Wyjście z funkcji w przypadku kiedy wartości BSSID nie są równe 
    
    Serial.print(ci.rssi);          // Przesłanie wartości RSSI śledzonego klienta portem szeregowym
    return ci.channel;              // Funkcja zwraca kanał klienta
  }
}

void print_beacon(beaconinfo beacon)
{
  if (beacon.err != 0) {
    //Serial.printf("BEACON ERR: (%d)  ", beacon.err);
  } else {
    char bssid[12];
    sprintf(bssid, "%02x%02x%02x%02x%02x%02x", beacon.bssid[0], beacon.bssid[1], beacon.bssid[2], beacon.bssid[3], beacon.bssid[4], beacon.bssid[5]);
    if(strcmp(bssid, "9af4abda2aa0")==0)                //342cc4c2d61e - home, 9af4abda2aa0 - esp
    {
    //Serial.printf("BEACON: <=============== %s[%32s]\r\n", bssid, beacon.ssid);
    //Serial.printf("BEACON: <=============== [%32s]  ", beacon.ssid);
    //for (int i = 0; i < 6; i++) Serial.printf("%02x", beacon.bssid[i]);
    //Serial.printf("   %2d", beacon.channel);
    //Serial.printf("   %4d\r\n", beacon.rssi);
    //Serial.printf("\n");
    }
  }
}

void promisc_cb(uint8_t *buf, uint16_t len)
{
  int i = 0;
  uint16_t seq_n_new = 0;
  if (len == 12) {
    struct RxControl *sniffer = (struct RxControl*) buf;
  } else if (len == 128) {
    struct sniffer_buf2 *sniffer = (struct sniffer_buf2*) buf;
    struct beaconinfo beacon = parse_beacon(sniffer->buf, 112, sniffer->rx_ctrl.rssi);
    if (register_beacon(beacon) == 0) 
    {
      print_beacon(beacon);
      nothing_new = 0;
    }
  }
  else 
  {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    //Is data or QOS?
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) 
    {
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) 
      {
        if (register_client(ci) == 1) 
        {
          print_client(ci);
         // nothing_new = 0;
        }
      }
    }
  }
}
