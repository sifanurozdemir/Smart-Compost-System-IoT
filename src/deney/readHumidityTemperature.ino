#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include "DHT.h"

const char* ssid = "HONOR";
const char* pass = "123456789";

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

const char* broker = "test.mosquitto.org";
const int port = 1883;
const char* topic = "test_sifa_146544";

const long interval = 36000; // 30 dakikada 50 veri için interval süresi (1800000/50)
unsigned long previousMillis = 0;

const int dataLimit = 50;    // Veri limiti
const bool isUnlimited = false;
int dataIndex = 0;

#define DHTPIN 4     // DHT11 Data pin bağlı olduğu ESP32 pin numarası
#define DHTTYPE DHT11   // Kullanılan DHT sensör tipi
#define LDRPIN 34      // LDR sensörünün bağlı olduğu analog pin numarası

DHT dht(DHTPIN, DHTTYPE);

void setupWifi() {
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print("-");
  }
  Serial.print("\nConnected to: ");
  Serial.println(ssid);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setupWifi();
  
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
}

void loop() {
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    dataIndex++;
    if(dataIndex <= dataLimit || isUnlimited){
      // Sıcaklık ve nemi okuma
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();
      
      // LDR verisini okuma
      int ldrValue = analogRead(LDRPIN);

      // Verilerin geçerliliğini kontrol etme
      if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Error: Failed to read from DHT sensor!");
        return;
      }

      // Verileri seri monitöre yazdırma
      Serial.print("Sending message to topic: ");
      Serial.println(topic);
      Serial.print("Humidity: ");
      Serial.print(humidity);
      Serial.print(" %\t");
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print(" *C\t");
      Serial.print("LDR Value: ");
      Serial.println(ldrValue);

      // MQTT mesajı gönderme
      mqttClient.beginMessage(topic);
      mqttClient.print("Humidity: ");
      mqttClient.print(humidity);
      mqttClient.print(" %\tTemperature: ");
      mqttClient.print(temperature);
      mqttClient.print(" *C\tLDR Value: ");
      mqttClient.println(ldrValue);
      mqttClient.print("Index of data is: ");
      mqttClient.println(dataIndex+1);
      mqttClient.endMessage();

      Serial.println();
    }    
  }
}