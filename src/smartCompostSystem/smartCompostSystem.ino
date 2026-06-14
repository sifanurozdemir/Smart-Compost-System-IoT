#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

const char* ssid = "Technonet-Misafir";
const char* password = "Tech2024!";
const char* mqttServer = "10.200.44.21";
const int mqttPort = 1883;
const char* mqttTopic = "esp32/veriler";

WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int trigPin = 5;
const int echoPin = 18;
const int rolePin = 17;      // Fan (aktif LOW)
const int buzzerPin = 16;    // Buzzer

bool buzzerOn = false;
unsigned long buzzerStartMillis = 0;
unsigned long lastBuzzTriggerMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(rolePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  digitalWrite(rolePin, LOW);   // Fan kapalı (aktif LOW)
  digitalWrite(buzzerPin, LOW);  // Buzzer kapalı

  dht.begin();
  WiFi.begin(ssid, password);
  Serial.println("Wi-Fi bağlantısı kuruluyor...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi bağlantısı kuruldu.");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!client.connected()) reconnect();
  client.loop();

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  float distance = measureDistance();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT verileri okunamadı!");
    return;
  }

  Serial.println("----- Ölçüm -----");
  Serial.print("Sıcaklık: "); Serial.print(temperature); Serial.println(" °C");
  Serial.print("Nem: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("Mesafe: "); Serial.print(distance); Serial.println(" cm");

  // 🔥 FAN KONTROLÜ
  if (temperature > 31.0) {
    digitalWrite(rolePin, LOW);  // Fan Açık
  } else {
    digitalWrite(rolePin, HIGH); // Fan Kapalı
  }

  // 🔔 BUZZER KONTROLÜ (5s'de bir 1s ötsün, sadece mesafe < 30cm ise)
  unsigned long currentMillis = millis();
  if (distance < 30.0) {
    if (!buzzerOn && currentMillis - lastBuzzTriggerMillis >= 10000) {
      digitalWrite(buzzerPin, HIGH);
      buzzerOn = true;
      buzzerStartMillis = currentMillis;
      lastBuzzTriggerMillis = currentMillis;
    }

    if (buzzerOn && currentMillis - buzzerStartMillis >= 300) {
      digitalWrite(buzzerPin, LOW);
      buzzerOn = false;
    }
  } else {
    digitalWrite(buzzerPin, LOW);
    buzzerOn = false;
  }

  // 🔄 MQTT JSON GÖNDERİMİ
  String payload = "{";
  payload += "\"sicaklik\": " + String(temperature) + ",";
  payload += "\"nem\": " + String(humidity) + ",";
  payload += "\"mesafe\": " + String(distance);
  payload += "}";

  client.publish(mqttTopic, payload.c_str());

  delay(500);  // çok uzun gecikme kullanma
}

float measureDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // 30ms = 5m
  return duration * 0.0343 / 2;
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT bağlantısı deneniyor...");
    if (client.connect("ESP32_Client")) {
      Serial.println("bağlandı.");
    } else {
      Serial.print("Hata: ");
      Serial.print(client.state());
      Serial.println(" - 5 sn sonra yeniden denenecek...");
      delay(5000);
    }
  }
}

