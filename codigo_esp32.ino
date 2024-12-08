#include <Wire.h>
#include <SparkFun_BNO080_Arduino_Library.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BNO080 myIMU;

BLECharacteristic *pCharacteristic;
const char* deviceID = "SPU4";

class CustomBLEServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    Serial.println("Dispositivo conectado!");
  }

  void onDisconnect(BLEServer* pServer) {
    Serial.println("Dispositivo desconectado!");
  }
};

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!myIMU.begin()) {
    Serial.println("Erro ao inicializar o BNO080.");
    while (1);
  }

  Wire.setClock(400000);
  myIMU.enableGyro(50);
  myIMU.enableAccelerometer(50);

  BLEDevice::init("SPU4_BLE_Gyroscope");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new CustomBLEServerCallbacks());

  BLEService *pService = pServer->createService(BLEUUID("181A"));
  pCharacteristic = pService->createCharacteristic(
    BLEUUID("2A5D"),
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->addDescriptor(new BLE2902());
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
  Serial.println("BLE inicializado com sucesso.");
}

void loop() {
  if (myIMU.dataAvailable()) {
    float gyroX = myIMU.getGyroX();
    float gyroY = myIMU.getGyroY();
    float gyroZ = myIMU.getGyroZ();

    float accelX = myIMU.getAccelX();
    float accelY = myIMU.getAccelY();
    float accelZ = myIMU.getAccelZ();

    String data = String(deviceID) + "," +
                  String(accelX) + "," + String(accelY) + "," + String(accelZ) + "," +
                  String(gyroX) + "," + String(gyroY) + "," + String(gyroZ);

    pCharacteristic->setValue(data.c_str());
    pCharacteristic->notify();

    Serial.println("Dados enviados: " + data);
    delay(50);
  }
}
