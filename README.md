# MQTT-Notification-Leaf

Firmware to control a notification leaf over MQTT. Each leaf can be controlled separately.

![notification leaf](https://github.com/schneebonus/MQTT-Notification-Leaf/blob/main/leaf1.jpeg?raw=true)


### Requirenments

- ESP8266 board definition ( https://arduino-esp8266.readthedocs.io/en/latest/installing.html )
- PubSubClient.h ( https://pubsubclient.knolleary.net/ )
- ESP8266WiFi.h ( https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi )
- Ticker.h ( https://github.com/esp8266/Arduino/tree/master/libraries/Ticker )
- WS2812FX.h (https://github.com/kitesurfer1404/WS2812FX)

### Install

- Download mqtt_leaf.ino
- Set wifi settings (ssid and passsword)
- Set mqtt settings (mqttServer, mqttPort, mqttUser, mqttPassword)
- Set LED_COUNT and LED_PIN
- Deploy to your esp8266

### Control

- notification/on         -> turn on leafs (if colors are set)
- notification/off        -> turn off leafs
- notification/reset      -> reset leafs to default color (default is BLACK)
- notification/brightness -> payload 0-100 to set brightness
- notification/leaf       -> set leaf by sending a json containing:
  {
    panel: int leaf number,
    mode: MODE from conversion_mode list,
    color: COLOR from conversion_color list,
    speed: 1000,
    reverse: false
  }

### Customization

#### Add color

It is possible to add colors in mqtt_leaf.ino by adding a new uint32_t mapping to conversion_color:

![predefined colors](https://github.com/schneebonus/MQTT-Notification-Leaf/blob/main/color_enum.png?raw=true)

#### Add mode

It is possible to add modes in mqtt_leaf.ino by adding a new uint32_t mapping to conversion_mode:

![predefined modes](https://github.com/schneebonus/MQTT-Notification-Leaf/blob/main/mode_enum.png?raw=true)

### Node-RED integration

![nodered example](https://github.com/schneebonus/MQTT-Notification-Leaf/blob/main/nodered_leaf_example.png?raw=true)

### Credits

- https://github.com/kitesurfer1404/WS2812FX
- https://arduinojson.org/
- https://stackoverflow.com/questions/16844728/converting-from-string-to-enum-in-c
- https://www.thingiverse.com/thing:3307019
- https://www.thingiverse.com/thing:3230905
