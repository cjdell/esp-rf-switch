#!/bin/bash

# Shell script to test the EPROM on the ESP (I was having problems with this)

ESPIP=192.168.1.116

curl -X POST "http://$ESPIP/devices.json?json=abcdefghijklmnopqrstuvwxyz0123456789"
echo

echo 'Got back:'
curl "http://$ESPIP/devices.json"
echo
