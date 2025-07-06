#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// com8 is advertising device
void setup() {
  Serial.begin(115200);
  Serial.println("🔊 BLE Advertiser 시작");

  BLEDevice::init("ESP32_A");
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();

  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // Power 설정 관련
  pAdvertising->setMinPreferred(0x12);

  BLEAdvertisementData advData;
  advData.setName("ESP32_A");
  pAdvertising->setAdvertisementData(advData);

  pAdvertising->start();
  Serial.println("📡 Advertising 중...");
}

void loop() {
  delay(100);
}
