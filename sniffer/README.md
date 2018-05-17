esp8266 wifi sniffer example
------------


WiFi MAC and RSSI sniffer example, based on https://www.hackster.io/rayburne/esp8266-mini-sniff-f6b93a


Installing esp-open-sdk
=======================

* install opensdk with 2.0.0 version

```
$ git clone --recursive https://github.com/pfalcon/esp-open-sdk.git
$ cd esp-open-sdk
$ vim Makefile
-----
VENDOR_SDK=2.0.0
-----
make STANDALONE=y

```

Building
========

```
$ git clone https://github.com/metametaclass/esp8266-samplecode.git
$ cd esp8266-samplecode/sniffer/
$ export OPEN_SDK=/opt/esp-open-sdk/
$ make
```

Flashing
========

Ensure that your user has access to /dev/ttyUSB0

Connect USB-serial to auto-select boot mode:
https://github.com/espressif/esptool/wiki/ESP8266-Boot-Mode-Selection


esp8266 | FTDI usb-serial
--- | --- | 
GND | GND
VCC | VCC
RX | TX
TX | RX
CH_PD | RTS
GPIO0 | DTR

```
$ make flash

```
