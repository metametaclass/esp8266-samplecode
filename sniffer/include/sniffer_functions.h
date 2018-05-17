#ifndef SNIFFER_FUNCTIONS_H_INCLUDED
#define SNIFFER_FUNCTIONS_H_INCLUDED


#include "sniffer_types.h"

struct clientinfo parse_data(uint8_t *frame, uint16_t framelen, signed rssi, unsigned channel);

struct beaconinfo parse_beacon(uint8_t *frame, uint16_t framelen, signed rssi);

#endif
