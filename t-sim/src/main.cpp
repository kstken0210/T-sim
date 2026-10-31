#include "datacollect_first.h"
#include "wifi_send_py.h"

// 硬件实例
TwoWire sensorWire = TwoWire(1);

XPowersPMU PMU; 
DFRobot_LTR390UV ltr(0x1c, &sensorWire);
Adafruit_BME280 bme;
sht4x_handle_t handle_sht40;//init function will assign value so no need to new it here

BLECharacteristic *pCharacteristic;
BLEServer *pServer;  // 保存服务器指针
bool deviceConnected = false;

BME280_Data* bmeData=new BME280_Data(); // 全局数据结构实例 also these * with new so 不怕
LTR390_UV_Data *uvData=new LTR390_UV_Data();
SHT4X_Data *sht4xData=new SHT4X_Data();

WiFiMulti wifiMulti2;

BLEServerCallbacks* MyServer_init();
//初始化放set up not before setup QAQ
void setup() {
    delay(2000);
    Serial.begin(115200);
    // 1. 启动 I2C
    Wire.begin(PMU_I2C_SDA, PMU_I2C_SCL);
    sensorWire.begin(SENSOR_I2C_SDA, SENSOR_I2C_SCL,100000);
    delay(500);
    //scanI2C(sensorWire);
    // 2. 启动电源 (必须在传感器之前)
    initSystemPower();
    delay(500); // 等待电压完全稳定
    Serial.println(PMU.getALDO2Voltage());
    Serial.println(PMU.getSystemVoltage());
    LTR390_UV_init();
    BME280_init();
    BLE_init();
    wifi_init();
}

void loop() {

    // 发送逻辑
    //char *bleBuffer = new char[64];//need delete
    scanI2C(sensorWire);
    char bleBuffer[200];
    char bleBuffer2[64];
    readBME280(bmeData);
    delay(50);
    readUVorALS(uvData);
    delay(50);//remember to delay else bomb
    iic_SHT4X_read_cmd(0x44,sht4xData);
    data_package(bleBuffer, sizeof(bleBuffer), bmeData, uvData,sht4xData);
    data_package2(bleBuffer2, sizeof(bleBuffer2));
    if (deviceConnected) {
        pCharacteristic->setValue(bleBuffer2);
        pCharacteristic->notify();
        Serial.print("[BLE Notify] ");
    } else {
        Serial.print("[Local Log] ");
    }

    // 串口同步输出 (方便 VOFA+ 观察)
    Serial.println(bleBuffer);
    Serial.println(bleBuffer2);
    http_sendData(bmeData, uvData, sht4xData);
    delay(2000); // 每 2 秒循环一次
}