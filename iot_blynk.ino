#include <WiFi.h>
#include <PubSubClient.h>

#define BLYNK_TEMPLATE_ID "template_id" // replace
#define BLYNK_TEMPLATE_NAME "Device Name" // replace
#define BLYNK_AUTH_TOKEN "Auth Token" // replace

#define LDR_PIN 10
#define LED_PIN 11  

const char* ssid = "WiFI SSID"; // replace
const char* password = "p@ssw3rd"; // replace

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
    digitalWrite(LED_PIN, HIGH);
  } else if (payload.equalsIgnoreCase("0")) {
    Serial.println("LED turned off");
    digitalWrite(LED_PIN, LOW);
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

  pinMode(LDR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

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
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 2000) {
    int ldrValue = analogRead(LDR_PIN);
    String payload = String(ldrValue);
    client.publish(pubTopic.c_str(), payload.c_str());
    Serial.print("Published LDR value: ");
    Serial.println(ldrValue);
    lastPublish = millis();
  }
}
