#include "datacollect_first.h"

// --- 蓝牙事件回调 ---
BLEServerCallbacks* MyServer_init()
{
    class MyServerCallbacks: public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) { 
            deviceConnected = true; 
            Serial.println(">>> 评委/用户已连线");
        };
        void onDisconnect(BLEServer* pServer) { 
            deviceConnected = false; 
            Serial.println(">>> 连线断开，重新开启广播...");
            pServer->getAdvertising()->start(); 
        }
    };
    return new MyServerCallbacks();
}

void initSystemPower() {
    if (!PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, PMU_I2C_SDA, PMU_I2C_SCL)) {
        Serial.println("❌ 警告：未检测到 PMU (AXP2101)");
        return;
    }

    // 1. 开启必须的电压通道
    PMU.setALDO2Voltage(3300); // 3.3V 给主板 3.3V 引脚 (LTR390 使用)
    PMU.enableALDO2();
    
    PMU.setDC3Voltage(3300);
    PMU.enableDC3();
    PMU.setALDO1Voltage(3300); // 3.3V 给 SIM7080G 模块
    PMU.enableALDO1();

    //GPS
    //PMU.setBLDO2Voltage(3300);
    //PMU.enableBLDO2(); 
    //for charging
    PMU.disableTSPinMeasure();
    // 2. 禁用所有不使用的通道 (低功耗模式 - 比赛加分项)
    PMU.disableCPUSLDO();
    PMU.disableDLDO1();
    PMU.disableDLDO2();
    PMU.disableBLDO2();
    // 注意：DCDC1 维持系统核心运行，必须保留
    // PMU.setVbusVoltageLimit(XPOWERS_AXP2101_VBUS_VOL_LIM_4V36);
    // PMU.setVbusCurrentLimit(XPOWERS_AXP2101_VBUS_CUR_LIM_1500MA);
    // PMU.setSysPowerDownVoltage(2600);

    // 3. 电池监测配置
    PMU.enableBattDetection();
    PMU.enableVbusVoltageMeasure();
    PMU.enableBattVoltageMeasure(); 
    PMU.enableSystemVoltageMeasure();

    Serial.printf("--- PMU Diagnostic Info ---\n");
    Serial.printf("System Voltage: %u mV\n", PMU.getSystemVoltage());
    Serial.printf("VBUS in: %d\n", PMU.isVbusIn());
    Serial.printf("Battery Connect: %d\n", PMU.isBatteryConnect());
    Serial.printf("ALDO2 is enabled: %s\n", PMU.isEnableALDO2() ? "YES" : "NO");
    Serial.printf("ALDO2 voltage setting: %u mV\n", PMU.getALDO2Voltage());
    Serial.printf("DCDC5 is enabled: %s\n", PMU.isEnableDC5() ? "YES" : "NO");
    Serial.printf("DCDC5 voltage setting: %u mV\n", PMU.getDC5Voltage());

    //Set PMU interrupt
    //after competition set
    Serial.println("✅ 电源管理系统配置完成 (已禁用闲置通道)");
}

uint8_t getBatteryPercent2() {
    // AXP2101 提供直接读取百分比的方法
    int percent = PMU.getBatteryPercent();
    if (percent < 0) return 0;
    if (percent > 100) return 100;
    return (uint8_t)percent;
}

void scanI2C(TwoWire &Wire) {
    Serial.println("扫描 I2C 总线...");
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("找到设备: 0x%02X\n", addr);
        }
    }
}

void LTR390_UV_init()
{
    
    if (ltr.begin()!=0)
    {
        Serial.println("❌ 错误：LTR390UV 传感器未响应 (检查接线或 ALDO2)");
    }
    else{
        #ifdef UV_OPEN
            ltr.setMode(ltr.eUVSMode); // 切换到 UV 模式
            Serial.println("UV Detection Enabled");
        #endif
        #ifdef ALS_OPEN
            ltr.setMode(ltr.eALSMode);
            Serial.println("Ambient Light Detction Enabled");
        #endif
        ltr.setALSOrUVSMeasRate(ltr.e18bit, ltr.e100ms);
        ltr.setALSOrUVSGain(ltr.eGain18); //egain3,egain1,egain18...
        Serial.println("✅ LTR390UV 传感器初始化成功");
    }
}
void BME280_init()
{
    if (!bme.begin(0x76, &sensorWire)) {
        Serial.println("BME280 初始化失败，请检查接线和电源");
        while (1);   // 停机，不再执行后续代码
    }
    Serial.println("BME280 初始化成功");
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                    Adafruit_BME280::SAMPLING_X2,  // 溫度
                    Adafruit_BME280::SAMPLING_X16, // 氣壓 (提高高度精度)
                    Adafruit_BME280::SAMPLING_X1,  // 濕度
                    Adafruit_BME280::FILTER_X16);//don't know how to set
}
BME280_Data* readBME280(BME280_Data* data)
{
    //static BME280_Data data; // 使用静态变量避免频繁分配内存 may have 調度問題
    data->temperature=bme.readTemperature();
    data->pressure=bme.readPressure()/100.0F; // 转换为 hPa
    data->humidity=bme.readHumidity();
    data->altitude=bme.readAltitude(1013.25);
    return data;
}
// static void debug_print(const char *const fmt, ...) {
//     char buffer[256];
//     va_list args;
//     va_start(args, fmt);
//     vsnprintf(buffer, sizeof(buffer), fmt, args);
//     va_end(args);
//     Serial.print(buffer);
// }
// static uint8_t iic2_init(void) {
//     sensorWire.begin(SENSOR_I2C_SDA, SENSOR_I2C_SCL);

//     return 0;  // Arduino 的 Wire.begin 没有返回值，默认成功
// }

// static uint8_t iic2_deinit(void) {
//     sensorWire.end();
//     return 0;
// }

// static uint8_t iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len) {
//     //uint8_t slave_addr = addr >> 1;   // 将 8 位地址转为 7 位地址
//     //Wire.beginTransmission(slave_addr);
//     sensorWire.beginTransmission(addr);
//     sensorWire.write(buf, len);
//     uint8_t ret = sensorWire.endTransmission(true);
//     return (ret == 0) ? 0 : 1;   // 0:成功, 其他:失败
// }

// static uint8_t iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len) {
//     sensorWire.requestFrom(addr, (uint8_t)len);
    
//     // 用你自己的 debug 接口，避开 C/C++ 混合打印的问题
//     debug_print("[I2C] Reading from 0x%02X, len: %d\n", addr, len);

//     uint16_t i = 0;
//     while (sensorWire.available() && i < len) {
//         buf[i] = sensorWire.read();
//         debug_print("%02X ", buf[i]); // 打印数据
//         i++;
//     }
//     debug_print("\n");
//     return (i == len) ? 0 : 1;
// }
// static void delay_ms(uint32_t ms) {
//     delay(ms);
// }


// uint8_t SHT40_init(sht4x_handle_t *handle_sht40,uint8_t iic_addr2)
// {
//     handle_sht40->iic_init   = iic2_init; // 如果 Wire 已經初始化，這裡設為 NULL
//     handle_sht40->iic_deinit = iic2_deinit;
//     handle_sht40->iic_read_cmd   = iic_read_cmd;
//     handle_sht40->iic_write_cmd  = iic_write_cmd;
//     handle_sht40->delay_ms   = delay_ms;
//     handle_sht40->debug_print = debug_print;
//     handle_sht40->iic_addr = iic_addr2; 
//     handle_sht40->inited = 0; // 初始化标志位，sht4x_init 内部会设置为 1
//     uint8_t res = sht4x_init(handle_sht40);
//     if (res != 0) {
//         Serial.printf("❌ SHT40 初始化失败, Code: %d\n", res);
//     } else {
//         Serial.println("✅ SHT40 初始化成功");
//         // 强制标记，防止驱动内部没设好
//         handle_sht40->inited = 1; 
//     }
//     return res;
// }

// uint8_t SHT4X_readData(sht4x_handle_t *handle_sht40, SHT4X_Data *sht4xData)
// {
//     return sht4x_read(handle_sht40, sht4xData->SHT4X_Mode, 
//                    &(sht4xData->SHT4X_temperature_raw), &(sht4xData->SHT4X_temperature_s), 
//                    &(sht4xData->SHT4X_humidity_raw), &(sht4xData->SHT4X_humidity_s));
// }
void BLE_init()
{
    BLEDevice::init("Mountain_Station_S3");
    BLEDevice::setMTU(512);
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(MyServer_init());
    
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                      );
    std::string initial_fill(150, ' '); // new stuff
    pCharacteristic->setValue(initial_fill);
    // 添加描述符，使手机 App 能正确识别 Notify 开关
    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setValue("Waiting...");

    pService->start();
    pServer->getAdvertising()->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    
    Serial.println("🚀 系统就绪，正在通过蓝牙广播...");
}

void readUVorALS(LTR390_UV_Data *uvData)
{
    #ifdef UV_OPEN
    uint32_t uvRaw = ltr.readOriginalData();
    uvData->rawData = uvRaw;
    float uvIndex= (float)(uvData->rawData) / (2300.0/4);
    if (uvIndex < 0) uvIndex = 0;
    uvData->uvIndex = uvIndex;

    #endif
    #ifdef ALS_OPEN
    uint32_t ALS_traned = ltr.readALSTransformData();
    return ALS_traned;
    #endif
}

uint8_t iic_SHT4X_read_cmd(uint8_t addr,SHT4X_Data *data) {
    // 强制输出到串口，不走任何判断
    
    // 1. 发送指令
    sensorWire.beginTransmission(addr);
    sensorWire.write(0xFD); // 高精度测量
    if (sensorWire.endTransmission() != 0) return 1;
    // 2. 强行延时
    delay(15); 
    // 3. 读取数据
    if (sensorWire.requestFrom(addr, (uint8_t)6) != 6) return 4;
    uint8_t buf[6];
    for (int i = 0; i < 6; i++) {
        buf[i] = sensorWire.read();
    }
    // 4. 计算数据 (直接复用驱动的公式)
    data->SHT4X_temperature_raw = (uint16_t)((((uint16_t)buf[0]) << 8) | buf[1]);                        /* get raw temperature */
    data->SHT4X_humidity_raw = (uint16_t)((((uint16_t)buf[3]) << 8) | buf[4]);                           /* get raw humidity */
    data->SHT4X_temperature_s = (float)(data->SHT4X_temperature_raw) / 65535.0f * 175.0f - 45.0f;                   /* convert raw temperature */
    data->SHT4X_humidity_s = (((float)(data->SHT4X_humidity_raw) / 65535.0f) * 125.0f - 6.0f);                      /* convert raw humidity */
    if ((data->SHT4X_humidity_s) > 100.0f)                                                               /* check humidity range */
    {
        data->SHT4X_humidity_s = 100.0f;                                                                 /* max humidity is 100% */
    }
    if ((data->SHT4X_humidity_s) < 0.0f)                                                                 /* check humidity range */
    {
        data->SHT4X_humidity_s = 0.0f;                                                                   /* min humidity is 0% */
    }

    return 0; // 成功！
}

void data_package(char *bleBuffer, size_t bufferSize,BME280_Data* bmeData,LTR390_UV_Data *uvData,SHT4X_Data *sht4xData)
{
    snprintf(bleBuffer, bufferSize, "UV:%.2f Raw:%u Bat:%d%% Temp:%.2f°C Pres:%.2fpa Humid:%.2f%% Alt:%.2fm Temp2:%.2f°C Humid2:%.2f%%", uvData->uvIndex, uvData->rawData, getBatteryPercent2(),bmeData->temperature,
    bmeData->pressure,bmeData->humidity,bmeData->altitude, sht4xData->SHT4X_temperature_s, sht4xData->SHT4X_humidity_s);
}
void data_package2(char *bleBuffer2, size_t bufferSize2)
{
    snprintf(bleBuffer2, bufferSize2,"Status:%s, Volt:%dmV, Vbus_volatge:%dmv, vbus_in:%s", PMU.isCharging() ? "Charging" : "Discharging", PMU.getBattVoltage(),PMU.getVbusVoltage(), PMU.isVbusIn() ? "Yes" : "No");
}