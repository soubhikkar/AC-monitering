#include "UbidotsESPMQTT.h"

#define pir1 D7
#define pir2 D8
#define sense D1

#define UBIDOTS_TOKEN "BBFF-9VOtZSnSzkJmJbNirpgvC2HyNIZHl0"
#define WIFI_SSID "beast_2-4GHz"
#define WIFI_PASS "iamthebeast"
#define DEVICE_LABEL "soet-ac-monitoring"
#define VARIABLE_LABEL_1 "3103"
#define VARIABLE_LABEL_2 "3301"

Ubidots client(UBIDOTS_TOKEN);

unsigned long long curr_time = 0;

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

int p1_val, p2_val, state = 0, prev_state = 0;

void setup() {

  Serial.begin(115200);
  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);
  pinMode(sense, INPUT);


  // client.wifiConnection(WIFI_SSID, WIFI_PASS);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(400);
  }
  Serial.println(WiFi.localIP());
  //The ESP8266 tries to reconnect automatically when the connection is lost
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  client.begin(callback);
}

void loop() {
  if (!client.connected()) {
    client.reconnect();
  }

  p1_val = digitalRead(pir1);
  p2_val = digitalRead(pir2);

  if ((p1_val == 1 || p2_val == 1) && digitalRead(sense) == 1) {
    state = 0;
  } else if ((p1_val == 0 && p2_val == 0) && digitalRead(sense) == 1) {
    state = 1;
  } else if ((p1_val == 0 && p2_val == 0) && digitalRead(sense) == 0) {
    state = 0;
  } else if ((p1_val == 1 && p2_val == 1) && digitalRead(sense) == 0) {
    state = 0;
  }

  if (millis() - curr_time >= 5000) {
    client.add(VARIABLE_LABEL_1, state);
    Serial.println(digitalRead(sense));
    client.ubidotsPublish(DEVICE_LABEL);
    client.loop();
    curr_time = millis();
  }

  // Serial.print(p1_val);
  // Serial.println(p2_val);
}