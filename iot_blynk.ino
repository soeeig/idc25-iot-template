#include <WiFi.h>
#include <PubSubClient.h>

#define BLYNK_TEMPLATE_ID "template id"
#define BLYNK_TEMPLATE_NAME "name"
#define BLYNK_AUTH_TOKEN "token"

const char* ssid = "ssid";
const char* password = "password";

const char* mqtt_server = "blynk.cloud";
const int mqtt_port = 1883;
   
const char* mqtt_server = "blynk.cloud";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

String pubTopic = String("ds/") + "V0";
String subTopic = "downlink/#";

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("📩 Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String payload;
  for (unsigned int i = 0; i < length; i++) {
    payload += (char)message[i];
  }
  Serial.println(payload);
  if (payload.equalsIgnoreCase("1")) {
    Serial.println("LED turned on");
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (payload.equalsIgnoreCase("0")) {
    Serial.println("LED turned off");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32 IoT Device", "device", BLYNK_AUTH_TOKEN)) {
      Serial.println(" connected ✅");
      client.subscribe(subTopic.c_str());
      Serial.print("Subscribed to: ");
      Serial.println(subTopic);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retry in 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) { // Keep connection between ESP32 and Blynk
    reconnect();
  }
  client.loop();

  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 2000) { // Limit publish to 2s interval

    // Functional code goes here - Add code to deal with ur components here

    String payload; // value of nil - keep it nil to test components

    if (!payload.isEmpty()) { // Only upload something if payload is not empty
      client.publish(pubTopic.c_str(), payload.c_str());
      Serial.print("Published LDR value: ");
      lastPublish = millis();
    } else { 
      Serial.println("Empty payload");
      lastPublish = millis();

    }
  }
}


