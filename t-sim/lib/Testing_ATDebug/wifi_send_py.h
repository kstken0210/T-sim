#ifndef WIFI_SEND_PY_H
#define WIFI_SEND_PY_H
#pragma once


#include <WiFi.h>
#include <WiFiClient.h>

#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "datacollect_first.h"

extern WiFiMulti wifiMulti2;

void wifi_init();
void http_sendData(BME280_Data* bmeData,LTR390_UV_Data *uvData,SHT4X_Data *sht4xData);

#endif