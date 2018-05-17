
#include "inttypes.h"

#include "sniffer_types.h"

#include "sniffer_functions.h"

#include "osapi.h"

#define MAX_APS_TRACKED 100
#define MAX_CLIENTS_TRACKED 200

struct beaconinfo aps_known[MAX_APS_TRACKED];                    // Array to save MACs of known APs
int aps_known_count = 0;                                  // Number of known APs
int nothing_new = 0;
struct clientinfo clients_known[MAX_CLIENTS_TRACKED];            // Array to save MACs of known CLIENTs
int clients_known_count = 0;                              // Number of known CLIENTs



int register_beacon(struct beaconinfo beacon)
{
  int known = 0;   // Clear known flag
  int u;
  for (u = 0; u < aps_known_count; u++)
  {
    if (! os_memcmp(aps_known[u].bssid, beacon.bssid, ETH_MAC_LEN)) {
      known = 1;
      break;
    }   // AP known => Set known flag
  }
  if (! known)  // AP is NEW, copy MAC to array and return it
  {
    os_memcpy(&aps_known[aps_known_count], &beacon, sizeof(beacon));
    aps_known_count++;

    if ((unsigned int) aps_known_count >=
        sizeof (aps_known) / sizeof (aps_known[0]) ) {
      os_printf("exceeded max aps_known\n");
      aps_known_count = 0;
    }
  }
  return known;
}

int register_client(struct clientinfo ci)
{
  int known = 0;   // Clear known flag
  int u;
  for (u = 0; u < clients_known_count; u++)
  {
    if (! os_memcmp(clients_known[u].station, ci.station, ETH_MAC_LEN)) {
      known = 1;
      break;
    }
  }
  if (! known)
  {
    os_memcpy(&clients_known[clients_known_count], &ci, sizeof(ci));
    clients_known_count++;

    if ((unsigned int) clients_known_count >=
        sizeof (clients_known) / sizeof (clients_known[0]) ) {
      os_printf("exceeded max clients_known\n");
      clients_known_count = 0;
    }
  }
  return known;
}

void print_beacon(struct beaconinfo beacon)
{
  if (beacon.err != 0) {
    //os_printf("BEACON ERR: (%d)  ", beacon.err);
  } else {
    os_printf("BEACON: <=============== [%32s]  ", beacon.ssid);
    int i;
    for (i = 0; i < 6; i++) os_printf("%02x", beacon.bssid[i]);
    os_printf("   %2d", beacon.channel);
    os_printf("   %4d\r\n", beacon.rssi);
  }
}

void print_client(struct clientinfo ci)
{
  int u = 0;
  int known = 0;   // Clear known flag
  if (ci.err != 0) {
    // nothing
  } else {
    os_printf("DEVICE: ");
    int i;
    for (i = 0; i < 6; i++) os_printf("%02x", ci.station[i]);
    os_printf(" ==> ");

    for (u = 0; u < aps_known_count; u++)
    {
      if (! memcmp(aps_known[u].bssid, ci.bssid, ETH_MAC_LEN)) {
        os_printf("[%32s]", aps_known[u].ssid);
        known = 1;     // AP known => Set known flag
        break;
      }
    }

    if (! known)  {
      os_printf("   Unknown/Malformed packet \r\n");
      //  for (int i = 0; i < 6; i++) os_printf("%02x", ci.bssid[i]);
    } else {
      os_printf("%2s", " ");
      int i;
      for (i = 0; i < 6; i++) os_printf("%02x", ci.ap[i]);
      os_printf("  %3d", aps_known[u].channel);
      os_printf("   %4d\r\n", ci.rssi);
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
    //print_beacon(beacon);
    if (register_beacon(beacon) == 0) {
      print_beacon(beacon);
      nothing_new = 0;
    }
  } else {
    struct sniffer_buf *sniffer = (struct sniffer_buf*) buf;
    //Is data or QOS?
    if ((sniffer->buf[0] == 0x08) || (sniffer->buf[0] == 0x88)) {
      struct clientinfo ci = parse_data(sniffer->buf, 36, sniffer->rx_ctrl.rssi, sniffer->rx_ctrl.channel);
      //print_client(ci); 
      if (memcmp(ci.bssid, ci.station, ETH_MAC_LEN)) {            
        if (register_client(ci) == 0) {
          print_client(ci);
          nothing_new = 0;
        }
      }
    }
  }
}

