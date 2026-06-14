# 🌱 Akıllı Kompostlama Sistemi (Smart Composting System)

[![ESP32](https://img.shields.io/badge/Hardware-ESP32-blue?style=for-the-badge&logo=espressif)](https://www.espressif.com/)
[![Arduino IDE](https://img.shields.io/badge/Software-Arduino_IDE-00979D?style=for-the-badge&logo=arduino)](https://www.arduino.cc/)
[![MQTT](https://img.shields.io/badge/Protocol-MQTT-green?style=for-the-badge)](http://mqtt.org/)
[![Jamovi](https://img.shields.io/badge/Analysis-Jamovi-orange?style=for-the-badge)](https://www.jamovi.org/)

Bu proje, organik atıkların kompostlanmasını optimize etmek için akıllı sensörler, otomasyon ve nesnelerin interneti (IoT) teknolojilerini kullanan mikrodenetleyici tabanlı bir akıllı geri dönüşüm sistemidir. Sistem; sıcaklık, nem ve doluluk oranlarını gerçek zamanlı izleyerek kompostlama sürecini hızlandırır, verimliliği artırır ve çevresel sürdürülebilirliğe katkı sağlar.

---

## 🚀 Öne Çıkan Özellikler

*   **Sıcaklık ve Nem Takibi:** DHT11/DHT22 sensörü ile kompost yığınının sıcaklık ve nem durumunun anlık izlenmesi.
*   **Doluluk Seviyesi Tespiti:** HC-SR04 ultrasonik mesafe sensörü aracılığıyla kutunun doluluk oranının sürekli ölçülmesi.
*   **Otomatik Havalandırma (Fan Otomasyonu):** Sıcaklık **31.0°C** üzerine çıktığında otomatik devreye giren fan/havalandırma sistemi (röle kontrollü).
*   **Kritik Seviye Uyarı Sistemi (Buzzer):** Doluluk oranı kritik seviyeye (mesafe < 30cm) geldiğinde 10 saniyede bir 300ms öten sesli uyarı sistemi.
*   **IoT & Bulut Entegrasyonu:** Sensör verilerinin JSON formatında paketlenerek Wi-Fi üzerinden MQTT Broker'a (`esp32/veriler` topic'i ile) 500 ms aralıklarla aktarılması.
*   **Veri Analitiği & Kalibrasyon:** Jamovi programı kullanılarak farklı çevresel ortamlardan (banyo, buzdolabı, güneş ışığı vb.) toplanan verilerin korelasyon analizi ve sensör tutarlılık testleri.

---

## 📂 Proje Dizin Yapısı

Proje dizin yapısı aşağıdaki gibidir:

```directory
Akıllı-Kompostlama-Sistemi/
├── docs/                         # Proje dokümanları ve görsel şemalar
│   ├── Akilli-Kompostlama-Sistemi.pdf  # Proje tanıtım sunumu
│   ├── flowChart.png             # Yazılım akış şeması görseli
│   └── proje_tasarimi.md         # Ayrıntılı donanım/yazılım tasarım belgesi
├── src/                          # Kaynak kodlar ve deney verileri
│   ├── smartCompostSystem/       # Ana sistem kaynak kodları
│   │   └── smartCompostSystem.ino # ESP32 üzerinde çalışan ana otomasyon yazılımı
│   └── deney/                    # Deneysel ön çalışmalar ve analizler
│       ├── Deneyin-Amaci.pdf     # Deneysel metodoloji ve amaç sunumu
│       ├── readHumidityTemperature.ino # Deney aşamasında kullanılan MQTT veri toplama kodu
│       └── deneySonuclar/        # 7 farklı ortamda toplanan ham sensör çıktıları
│           └── output*.txt       # Çift bilgisayar karşılaştırmalı test verileri (40 Adet)
├── .gitignore                    # Git sürüm kontrolü dışı bırakılacak dosyalar
└── README.md                     # Proje ana tanıtım belgesi (Bu dosya)
```

---

## 🛠️ Donanım ve Bağlantı Şeması

Sistemin ana donanım tasarımı ESP32 geliştirme kartı üzerine inşa edilmiştir.

| Sensör / Aktüatör | ESP32 Pini | Açıklama |
| :--- | :---: | :--- |
| **DHT11 / DHT22** | `GPIO 4` | Kompost sıcaklığı ve nemini okur. |
| **HC-SR04 Trig** | `GPIO 5` | Ultrasonik tetik sinyalini gönderir. |
| **HC-SR04 Echo** | `GPIO 18` | Yansıyan ses dalgası süresini ölçer. |
| **Röle Modülü (Fan)** | `GPIO 17` | Havalandırma fanını kontrol eder (Aktif LOW). |
| **Buzzer** | `GPIO 16` | Kritik doluluk seviyesinde alarm verir. |
| **LDR Sensör** | `GPIO 34` | Işık yoğunluğunu ölçer (Analiz aşaması). |

Detaylı donanım mimarisi ve yazılım akış mantığı için [proje_tasarimi.md](file:///c:/Users/hp/Desktop/staj%201/Akıllı-Kompostlama-Sistemi/docs/proje_tasarimi.md) dosyasını inceleyebilirsiniz.

---

## 💻 Kurulum ve Çalıştırma

### 1. Donanım Bağlantıları
Bileşenleri yukarıdaki pin tablosuna uygun olarak ESP32 kartınıza bağlayın. Fan rölesinin aktif-LOW modunda çalıştığından ve harici besleme ihtiyacını karşıladığınızdan emin olun.

### 2. Yazılım Hazırlığı
1. Bilgisayarınıza **Arduino IDE** programını kurun.
2. `Araçlar > Kart` menüsünden **ESP32 Dev Module** kartını seçin.
3. Aşağıdaki kütüphaneleri Arduino Kütüphane Yöneticisi üzerinden yükleyin:
   * **DHT sensor library** (Adafruit)
   * **PubSubClient** (Nick O'Leary)
   * **ArduinoMqttClient** (Deney kodu için)

### 3. Kodu Yükleme
`src/smartCompostSystem/smartCompostSystem.ino` dosyasını Arduino IDE ile açın ve aşağıdaki satırları kendi ağ ve sunucu bilgilerinizle güncelleyin:

```cpp
const char* ssid = "WIFI_AG_ADINIZ";
const char* password = "WIFI_SIFRENIZ";
const char* mqttServer = "MQTT_BROKER_IP_ADRESI";
```

Ardından derleyip ESP32 kartınıza yükleyin.

---

## 🔬 Deneysel Çalışmalar ve Bulgular

Sistem tasarımı öncesinde, DHT11 ve LDR sensörlerinin farklı çevresel koşullardaki davranışını analiz etmek ve veri tutarlılığını test etmek amacıyla deneysel bir süreç yürütülmüştür:

1.  **7 Farklı Ortamda Test:** Veriler; banyo (yüksek nem), saksı (değişken nem), buzdolabı (~4°C sabit sıcaklık), klimalı oda, klimasız oda, doğrudan güneş ışığı altı ve yapay ışık altında toplanmıştır.
2.  **Çift Bilgisayar Testi:** Sensör doğruluğu ve kalibrasyon kalitesini ölçmek amacıyla aynı ortamda iki ayrı cihaz üzerinden eş zamanlı ölçümler yapılmış, verilerin genel eğilimde son derece tutarlı olduğu gözlemlenmiştir.
3.  **İstatiksel Analiz:** Toplanan ham veriler (bkz. `src/deney/deneySonuclar/`) **Jamovi** programına aktarılarak korelasyon analizleri yapılmış ve ortam sıcaklığı ile nem oranlarının kompostlamaya en uygun olduğu koşullar matematiksel olarak belirlenmiştir.

---

## 📚 Kaynakça ve Referanslar

*   **Pansari, N.B., Deosarkar, S.B., & Nandgaonkar, A.B. (2019).** *Smart compost system.* Springer.
*   **Siswoyo, R., Lu, M., Raman, V., Hanghui Then, P. (2019).** *Design and implementation of IoT-enabled compost monitoring system.* Springer.
