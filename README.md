# QPiUPSMon #
Reads out the PiUPS+ expansion board registers
and sends the data to an MQTT broker.

## Build ##
cd ext
mkdir lib
cd src/bcm2835-1.52
make
cp src/libbcm2835.a ../../lib/

cd ../qmqtt-master/src/mqtt
qmake mqtt.pro
make
cp libqmqtt.a ../../../../lib/

cd ../../../../..
qmake qpiupsmon.pro
make

## Dependencies ##

### bcm28355-library ###
http://www.airspayce.com/mikem/bcm2835/
Version 1.52

### QMQTT ###
https://github.com/emqtt/qmqtt/src/mqtt
commit: 3b22b35d498aeb366ef0dd3e69dc7870a1c0d2da
The project file was slightly modified to create a static library.

