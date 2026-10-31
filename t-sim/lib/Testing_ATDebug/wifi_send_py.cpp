#include "wifi_send_py.h"

const char *ssid = "Your WiFi SSID";
const char *password = "Your WiFi Password";

//PythonAnywhere 本身沒有預設這些 ID。 but can try
const String channelID = "Your ThingSpeak Channel ID";
const String writeApiKey = "Your ThingSpeak Write API Key";
const String readApiKey = "Your ThingSpeak Read API Key";
const String ServerURL = "http://www.hikingtrailhk.net/update"; 

unsigned long lastUploadTime = 0;           // 記錄上次上傳的時間
const unsigned long uploadInterval = 10000; // 設定間隔為 10000 毫秒 (10秒)

void wifi_init()
{
    Serial.print("Connecting");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

//back chwck each line of code for what
void http_sendData(BME280_Data* bmeData,LTR390_UV_Data *uvData,SHT4X_Data *sht4xData)
{
    unsigned long currentTime = millis();
    if (currentTime - lastUploadTime >= uploadInterval)
    {
        lastUploadTime = currentTime;
        if ((WiFi.status() == WL_CONNECTED)) //if ((wifiMulti2.run() == WL_CONNECTED))
        {
            HTTPClient http;
            Serial.print("[HTTP] begin...\n");
            http.begin(ServerURL);
            http.addHeader("Content-Type", "application/json");

            // 1. 直接打包所有感測器數據到 JSON
            JsonDocument doc;
            doc["api_key"]="Your Server API Key"; // 請改成伺服器端驗證用的 API key
            doc["Temp1"] = bmeData->temperature;
            doc["Humid1"] = bmeData->humidity;
            doc["Pressure"] = bmeData->pressure;
            doc["Altitude"] = bmeData->altitude;
            doc["UV_raw"] = uvData->rawData;
            doc["Uv_index"] = uvData->uvIndex;
            doc["Temp2"] = sht4xData->SHT4X_temperature_s;
            doc["Humid2"] = sht4xData->SHT4X_humidity_s;

            String jsonString;
            serializeJson(doc, jsonString);

            int httpCode = http.POST(jsonString);
            if (httpCode > 0) 
            {
                Serial.printf("[HTTP] 回應碼: %d\n", httpCode);
                if (httpCode==200)
                {
                    String response = http.getString();
                    Serial.print("伺服器回覆說：");
                    Serial.println(response);
                }
            } 
            else 
            {
                Serial.printf("[HTTP] 失敗: %s\n", http.errorToString(httpCode).c_str());
            }
            http.end();
        }
    }//no use delay(10000)
}
// these are some old tcp/socket things can find from arduino wificlient 
// void wifi_response(WiFiClient *client)
// {
//     unsigned long timeout = millis();
//     while (client->available() == 0)
//     {
//         if (millis() - timeout > 5000) 
//         {
//             Serial.println(">>> Client Timeout !");
//             client->stop();
//             return;
//         }
//     }
//     while (client->available())
//     {
//         String line = client->readStringUntil('\r');
//         Serial.print(line);
//     }
//     Serial.printf("\nClosing connection\n\n");
// }

// void wifi_send(WiFiClient *client,BME280_Data* bmeData,LTR390_UV_Data *uvData,SHT4X_Data *sht4xData)
// {
//     String footer = String(" HTTP/1.1\r\n") + "Host: " + String(host) + "\r\n" + "Connection: close\r\n\r\n";
//     if (!client->connect(host, httpPort))
//     {
//         return;
//     }
//     client->print("GET /update?api_key=" + writeApiKey + "&BME_temp=" + bmeData->temperature +
//         "&BME_humid=" + bmeData->humidity + "&BME_Press=" + bmeData->pressure + "&BME_Altitude="  + bmeData->altitude +
//         "&UV_Raw=" + uvData->rawData + "&Uv_index=" + uvData->uvIndex + "&SHT4X_Temp=" + sht4xData->SHT4X_temperature_s + "&SHT4X_Humid=" + 
//         sht4xData->SHT4X_humidity_s + footer);
//     wifi_response(client);
// }

// void wifi_read(WiFiClient *client,BME280_Data* bmeData,LTR390_UV_Data *uvData,SHT4X_Data *sht4xData)
// {
//     String readRequest = "GET /channels/" + channelID + "/fields/" + fieldNumber + ".json?results=" + numberOfResults + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";
//     if (!client->connect(host, httpPort))
//     {
//         return;
//     }
//     client->print(readRequest);
//     wifi_response(client);

// }

// void wifi_delay()
// {
//     delay(10000); // Delay between each send (10 seconds)
// }