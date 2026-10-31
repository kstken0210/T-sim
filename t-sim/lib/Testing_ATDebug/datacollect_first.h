#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "DFRobot_LTR390UV.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "driver_sht4x.h"
#include "utilities.h"

#define XPOWERS_CHIP_AXP2101
#include "XPowersLib.h" // 核心电源管理库
// #ifdef __cplusplus
// extern "C" {
// #endif
// --- 引脚与定义 ---
// PMU 专用 I2C（固定，不可改）
#define PMU_I2C_SDA 15
#define PMU_I2C_SCL 7

// 传感器 I2C（可以改成任意空闲 GPIO）
#define SENSOR_I2C_SDA 13
#define SENSOR_I2C_SCL 21

// BLE UUID (建议保持默认或自行随机生成)
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// --- 功能开关 (用户可以在这里注释掉不需要的功能) ---
#define UV_OPEN    // 开启紫外线检测
//#define ALS_OPEN    // 开启环境光 (Ambient Light) 检测

extern TwoWire sensorWire;
extern XPowersPMU PMU;
extern DFRobot_LTR390UV ltr;
extern Adafruit_BME280 bme;
extern sht4x_handle_t handle_sht40;
extern BLECharacteristic *pCharacteristic;
extern BLEServer *pServer;  // 保存服务器指针
extern bool deviceConnected;
struct BME280_Data {
    float temperature;   // 摄氏度
    float pressure;      // 百帕 (hPa)
    float humidity;      // 相对湿度 (%)
    float altitude;      // 海拔 (米)
};

struct LTR390_UV_Data{
    uint32_t rawData;
    float uvIndex;
};

struct SHT4X_Data{
    sht4x_mode_t SHT4X_Mode;
    uint16_t SHT4X_temperature_raw;
    float SHT4X_temperature_s;
    uint16_t SHT4X_humidity_raw;
    float SHT4X_humidity_s;
};

BLEServerCallbacks* MyServer_init();
void initSystemPower();
uint8_t getBatteryPercent2();
void scanI2C(TwoWire &Wire);
void LTR390_UV_init();
void BME280_init();
BME280_Data* readBME280(BME280_Data* data);
void BLE_init();
uint8_t iic_SHT4X_read_cmd(uint8_t addr,SHT4X_Data *data);
void readUVorALS(LTR390_UV_Data *uvData);
void data_package(char *bleBuffer, size_t bufferSize,BME280_Data* bmeData,LTR390_UV_Data *uvData,SHT4X_Data *sht4xData);
void data_package2(char *bleBuffer2, size_t bufferSize2);
// #ifdef __cplusplus
// }
// #endif
// // ------------------------------------------
