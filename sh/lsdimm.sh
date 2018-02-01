#!/bin/sh
for dev in /sys/bus/i2c/devices/0-00*;
do
        dimm=$(cut -b 129-145 ${dev}/eeprom);
        echo ${dimm};
done
