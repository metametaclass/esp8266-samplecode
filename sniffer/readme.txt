* install opensdk with 2.0.0 version

git clone https://github.com/pfalcon/esp-open-sdk

cd esp-open-sdk
vim Makefile

-----
VENDOR_SDK=2.0.0
-----


* build and flash:

export OPEN_SDK=/opt/esp-open-sdk/
make
make flash
