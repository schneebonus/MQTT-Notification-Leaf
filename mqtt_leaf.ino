#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WS2812FX.h>
#include <Ticker.h>
#include <ArduinoJson.h>

#define DEBUGGING     false
#define LED_COUNT     6
#define LED_PIN       2
#define DEVICE_ID     1

const char* ssid = "";          # ToDo
const char* password = "";      # ToDo
const char* mqttServer = "";    # ToDo
const int mqttPort = 1883;      # ToDo
const char* mqttUser = "";      # ToDo
const char* mqttPassword = "";  # ToDo

String MQTT_TOPIC = "notification";
uint32_t DEFAULT_COLOUR = BLACK;

WiFiClient espClient;
PubSubClient client(espClient);
volatile int watchdogCount = 0;
Ticker secondTick;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  if(DEBUGGING)Serial.begin(115200);

  ws2812fx.init();
  ws2812fx.setBrightness(100);
  ws2812fx.setSpeed(200);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.setColor(DEFAULT_COLOUR);
  ws2812fx.start();

  // init
  connect_wifi();               // wlan verbinden
  connect_mqtt();               // mqtt verbinden
  client.subscribe((MQTT_TOPIC + "/#").c_str());

  // start the watchdog
  // restarts esp in case main loop does not finish after 10 seconds
  secondTick.attach(1, ISRWatchdog);

}

void loop() {
  client.loop();
  ws2812fx.service();

  // reset watchdog to prevent reset
  watchdogCount = 0;

  // handle wifi connection
  int wifi_retry = 0;
    while(WiFi.status() != WL_CONNECTED && wifi_retry < 5 ) {
      // reconnect wifi if not connected
      wifi_retry++;
      WiFi.disconnect();
      WiFi.mode(WIFI_OFF);
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      delay(1000);
    }
    if(wifi_retry >= 5) {
        // if reconnect failed 5 times -> restart esp
        ESP.restart();
    }

    // handle mqtt connection
    if(!client.connected())ESP.restart();
}

//---------------------- WIFI --------------------------//
void connect_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }
  if(DEBUGGING)Serial.println("Connected to the WiFi network");
}

//---------------------- MQTT --------------------------//
void connect_mqtt() {
  int mqtt_retry = 0;
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected() && mqtt_retry < 5) {
    if (client.connect(("notification_led" + String(DEVICE_ID)).c_str(), mqttUser, mqttPassword )) {
      if(DEBUGGING)Serial.println("Connected to MQTT");
    } else {
      mqtt_retry++;
      if(DEBUGGING)Serial.print("failed with state ");
      if(DEBUGGING)Serial.print(client.state());
      delay(2000);
    }
  }
}

// callback to handle incoming mqtt events
void callback(char* topic, byte* payload, unsigned int length) {
  if(DEBUGGING)Serial.println(topic);
  // on, off, reset and brightness
  if(String(topic) == MQTT_TOPIC + "/on"){ws2812fx.start();return;}
  if(String(topic) == MQTT_TOPIC + "/off"){ws2812fx.stop();return;}
  if(String(topic) == MQTT_TOPIC + "/reset"){
    for(int i = 0; i < LED_COUNT; i++){
      ws2812fx.setSegment(i, i, i, FX_MODE_STATIC, {BLACK}, 1000, false);
    }
    ws2812fx.start();
    return;}
  if(String(topic) == MQTT_TOPIC + "/brightness"){
    int brightness = int(byteToString(payload, length).c_str());
    ws2812fx.setBrightness(brightness);
    ws2812fx.start();
    return;}
  // complex patterns
  if(String(topic) == MQTT_TOPIC + "/pattern"){
    DynamicJsonDocument doc(1024);
    String json_string = byteToString(payload, length);
    deserializeJson(doc, json_string);

    int index               = doc["index"];
    int start_pos           = doc["start"];
    int stop_pos            = doc["stop"];
    String led_mode         = doc["mode"];
    uint32_t led_mode_uint  = str2mode(led_mode.c_str());
    String color            = doc["color"];
    uint32_t color_uint     = str2color(color.c_str());
    int led_speed           = doc["speed"];
    bool led_reverse        = doc["reverse"];

    ws2812fx.setSegment(index, start_pos, stop_pos, led_mode_uint, {color_uint}, led_speed, led_reverse);
    ws2812fx.start();
    return;
    }
}

//---------------------- Watchdog --------------------------//
// watchdog to reset after crashes
void ISRWatchdog(){
  watchdogCount++;
  if(watchdogCount == 10){
    ESP.reset();
  }
}

//---------------------- Utils --------------------------//
// convert a byte array with a given length to a string
String byteToString(byte* payload, unsigned int length){
  String state = "";
  for (int i = 0; i < length; i++) {
    state += (char)payload[i];
  }
  return state;
}

//---------------------- String to Color --------------------------//
/**
 * Based on
 * https://stackoverflow.com/questions/16844728/converting-from-string-to-enum-in-c
 **/
const static struct {
    uint32_t      val;
    const char    *str;
    }
    conversion_color [] = {
    {RED, "RED"},
    {GREEN, "GREEN"},
    {BLUE, "BLUE"},
    {WHITE, "WHITE"},
    {BLACK, "BLACK"},
    {YELLOW, "YELLOW"},
    {CYAN, "CYAN"},
    {MAGENTA, "MAGENTA"},
    {PURPLE, "PURPLE"},
    {ORANGE, "ORANGE"},
    {PINK, "PINK"},
    {GRAY, "GRAY"},
    {ULTRAWHITE, "ULTRAWHITE"},
    {(uint32_t)0x8B4513, "BROWN"},
  };

uint32_t str2color (const char *str)
{
     int j;
     for (j = 0;  j < sizeof (conversion_color) / sizeof (conversion_color[0]);  ++j)
         if (!strcmp (str, conversion_color[j].str))
             return conversion_color[j].val;    
     return WHITE;
}

//---------------------- String to Mode --------------------------//
/**
 * Based on
 * https://stackoverflow.com/questions/16844728/converting-from-string-to-enum-in-c
 **/
const static struct {
    uint32_t      val;
    const char    *str;
    }
    conversion_mode [] = {
    {FX_MODE_BREATH, "FX_MODE_BREATH"},
    {FX_MODE_STATIC, "FX_MODE_STATIC"},
    {FX_MODE_BLINK, "FX_MODE_BLINK"},
  };

uint32_t str2mode (const char *str)
{
     int j;
     for (j = 0;  j < sizeof (conversion_mode) / sizeof (conversion_mode[0]);  ++j)
         if (!strcmp (str, conversion_mode[j].str))
             return conversion_mode[j].val;    
     return FX_MODE_STATIC;
}
