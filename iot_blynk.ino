#include <WiFi.h>
#include <PubSubClient.h>

#define BLYNK_TEMPLATE_ID "template id"
#define BLYNK_TEMPLATE_NAME "name"
#define BLYNK_AUTH_TOKEN "token"

const char* ssid = "ssid";
const char* password = "password";

const char* mqtt_server = "blynk.cloud";
const int mqtt_port = 1883;
   


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  client.setServer(mqtt_server, mqtt_port);

  Serial.println("Connecting to Blynk MQTT...");
  // correct: username = "device", password = blynk_token
  if (client.connect("ESP32Client01", "device", BLYNK_AUTH_TOKEN)) {
    Serial.println("✅ Connected to Blynk MQTT!");
  } else {
    Serial.print("❌ Failed, rc=");
    Serial.println(client.state());
  }
}

void loop() {
  client.loop();
}


