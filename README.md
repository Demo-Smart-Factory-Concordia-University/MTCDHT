# MTCDHT
MTConnect adapter for a DHT sensor

# Usage
Connect the DHT sensor to your ESP8266.

Compile and upload the code to your ESP8266 using the Ardunio IDE. In order the ESP8266 can connect to your WiFi network add 
a file `secret.h` in the [Arduino project folder](arduino/MTCDHT) with the following content:
```
#define SECRET_SSID "your Wifi ssid"
#define SECRET_PASS "your Wifi password"
```
Once running, check if SHDR messages are emitted by the adapter by running:
```
telnet IP_ADRESS 7878
```
where `IP_ADRESS` is the IP address of your ESP8266 running the adapter. The IP address can be found on the serial monitor attached to your ESP8266.
