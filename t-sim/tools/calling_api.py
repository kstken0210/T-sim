import json
import os
import time
from openai import OpenAI

import requests
import math 
from collections import deque

# 填入你的 API Key
API_KEY = "Your DeepSeek API Key"  
url = "https://api.deepseek.com/chat/completions" #chat model what about analysis model
url2="https://www.hikingtrailhk.net/post_analysis"
delta_Temp3=0
delta_Humid3=0
delta_UV_index=0
history_length=5
_history = deque(maxlen=history_length) 
def get_memory():
    base_path = os.path.dirname(__file__) #check check how to use
    file_path = os.path.join(base_path, "Memory.md")
    with open(file_path, 'r', encoding='utf-8') as file:
        Memory_extract=file.read()  #what is the content format
        return Memory_extract

def get_geog():
    base_path = os.path.dirname(__file__) #check check how to use
    file_path = os.path.join(base_path, "Geography_facilities.md")
    with open(file_path, 'r', encoding='utf-8') as file:
        Geography_extract=file.read()  #what is the content format
        return Geography_extract

def calling_API(arr_dict):
    if arr_dict==None:
        print("No data received from PythonAnywhere.")
        return
    headers = {
        "Content-Type": "application/json",
        "Authorization": f"Bearer {API_KEY}"
    }

    data = {
        "model": "deepseek-reasoner",  # 指定使用 R1 模型（deepseek-reasoner）或者 V3 模型（deepseek-chat）
        "messages": [
            {"role": "system", "content": f"這是你的長期記憶文件: {get_memory()}，這是你的環境周邊設施: {get_geog()}"},
            #{"role": "assistant", "content": },
            #{"role": "user", "content": json.dumps(arr_dict)}, #must string so use json.dumps
            {"role": "user", "content": f"即時數據: {json.dumps(arr_dict)}\n\n請根據長期記憶文件、環境周邊設施和即時數據進行分析,並提供對當前山區環境的評估和建議，簡要實用。繁體字。"}
        ],
        "stream": False  # 关闭流式传输
    }

    response = requests.post(url, headers=headers, json=data)#不是post~get,why post is upload but get os download

    if response.status_code == 200:
        result = response.json()
        print(result['choices'][0]['message']['content'])
    else:
        print("请求失败，错误码：", response.status_code)

    final_data={
        "Temp": arr_dict["Temp"],
        "Humid": arr_dict["Humid"],
        "Pressure": arr_dict["Pressure"],
        "Altitude": arr_dict["Altitude"],
        "UV_index": arr_dict["UV_index"],
        "Heat_Index": arr_dict["Heat_Index"],
        "Comprehensive_index": arr_dict["Comprehensive_index"],#result['choices'][0]['message']['content']
        "advice": result['choices'][0]['message']['content'],#result['choices'][0]['message']['content'],
        "latitude": "22.31643",   #"22.33692477872468"
        "longitude": "114.18016"  # "114.17150756478935"
    }
    back_response = requests.post(url2, headers={"Content-Type": "application/json"}, json=final_data,timeout=10)
    if back_response.status_code == 200:
        print("✅ 成功！分析结果已上传。")
    else:
        print("❌ 失败！分析结果上传失败。")


def getting_data_from_pythonAnywhere():#1分鐘request一次
    # response = requests.get("http://yourusername.pythonanywhere.com/view")
    # print(type(response.text))
    # soup=bs4.BeautifulSoup(response.text, 'html.parser')
    # find_td=soup.find('td')#find_all 不支持slicing
    # print(find_td)
    # arr1=find_td.get_text() 
    # arr=arr1[1:85].split(",") 切字串is stupid
    # print(arr)
    # return arr
    #time.sleep(10)
    response = requests.get("http://www.hikingtrailhk.net/api/latest")
    if response.status_code != 200:
        return None
    data=response.json()
    return data  # 假设返回的是 JSON 数据 should be dict !!!

def filtering_data(arr):#why 10 筆 not fewer or longer?stable,precise,less data transfer cpu good,how about data from esp to pythonAnywhere 次數？how many will use?
    #1 min 6 upload then 1min update one times
    global delta_Temp3, delta_Humid3, delta_UV_index, _history, history_length
    weighting_PAST_NEW=0.65
    weighting_MAX_AVG=0.7
    weighting_of_UV_HI=0.6

    data=arr[::-1]
    temp1_temper=data[0]["Temp1"]
    humid1_temper=data[0]["Humid1"]
    pressure_temper=data[0]["Pressure"]
    Altitude_temper=data[0]["Altitude"]
    UV_raw_temper=data[0]["UV_raw"]
    UV_index_temper=data[0]["Uv_index"]
    temp2_temper=data[0]["Temp2"]
    humid2_temper=data[0]["Humid2"]
    max_temp1_temper=data[0]["Temp1"]
    max_humid1_temper=data[0]["Humid1"]
    max_temp2_temper=data[0]["Temp2"]
    max_humid2_temper=data[0]["Humid2"]
    max_UV_index_temper=data[0]["Uv_index"]
    if len(arr)<2:
        return None #what is arr type,
    
    for i in range(len(data)-1):#bruh dumb ass can one loop do 8 filter save time
        Temp1=weighting_PAST_NEW*data[i+1]['Temp1']+(1-weighting_PAST_NEW)*temp1_temper
        temp1_temper=Temp1
        max_temp1_temper=max(max_temp1_temper,data[i+1]['Temp1'])
        Humid1=weighting_PAST_NEW*data[i+1]['Humid1']+(1-weighting_PAST_NEW)*humid1_temper
        humid1_temper=Humid1
        max_humid1_temper=max(max_humid1_temper,data[i+1]['Humid1'])
        Pressure=weighting_MAX_AVG*data[i+1]['Pressure']+(1-weighting_MAX_AVG)*pressure_temper
        pressure_temper=Pressure
        Altitude=weighting_MAX_AVG*data[i+1]['Altitude']+(1-weighting_MAX_AVG)*Altitude_temper
        Altitude_temper=Altitude
        UV_raw=weighting_of_UV_HI*data[i+1]['UV_raw']+(1-weighting_of_UV_HI)*UV_raw_temper
        UV_raw_temper=UV_raw
        UV_index=weighting_of_UV_HI*data[i+1]['Uv_index']+(1-weighting_of_UV_HI)*UV_index_temper
        UV_index_temper=UV_index
        max_UV_index_temper=max(max_UV_index_temper,data[i+1]['Uv_index'])
        Temp2=weighting_PAST_NEW*data[i+1]['Temp2']+(1-weighting_PAST_NEW)*temp2_temper
        temp2_temper=Temp2
        max_temp2_temper=max(max_temp2_temper,data[i+1]['Temp2'])
        Humid2=weighting_PAST_NEW*data[i+1]['Humid2']+(1-weighting_PAST_NEW)*humid2_temper
        humid2_temper=Humid2
        max_humid2_temper=max(max_humid2_temper,data[i+1]['Humid2'])#have another write method

    #預測+max_value+限制 !!!think more隊列 not +=
    Temp3=Temp1*0.2+Temp2*0.8 #BME 0.2 SHT40 0.8
    Humid3=Humid1*0.2+Humid2*0.8  #BME 0.2 SHT40 0.8

    _history.append((Temp3, Humid3, UV_index))
    if len(_history) == history_length:
        oldest = _history[0] 
        delta_Temp3=Temp3-oldest[0]
        delta_Humid3=Humid3-oldest[1]
        delta_UV_index=UV_index-oldest[2]
    else:
        delta_Temp3=0
        delta_Humid3=0
        delta_UV_index=0

    max_temp3=max_temp1_temper*0.2+max_temp2_temper*0.8
    max_humid3=max_humid1_temper*0.2+max_humid2_temper*0.8
    print(f"平滑後的數據 -> 溫度: {Temp3:.2f}, 濕度: {Humid3:.2f}, 氣壓: {Pressure:.2f}, 海拔: {Altitude:.2f}, UV_raw: {int(UV_raw_temper)}, UV_index: {int(UV_index_temper)}")#again review f string

    Tnw=Temp3*math.atan(0.151977*(math.sqrt(Humid3+8.313659)))+math.atan(Temp3+Humid3)-math.atan(Humid3-1.676331)+0.00391838*pow(Humid3,1.5)*math.atan(0.023101*Humid3)-4.686035
    Tg=1.184*Temp3+1.348*UV_index-0.0789*Humid3-2.73
    HI=0.8*Tnw+0.05*Tg+0.15*Temp3

    MAX_Tnw=max_temp3*math.atan(0.151977*math.sqrt((max_humid3+8.313659)))+math.atan(max_temp3+max_humid3)-math.atan(max_humid3-1.676331)+0.00391838*pow(max_humid3,1.5)*math.atan(0.023101*max_humid3)-4.686035
    MAX_Tg=1.184*max_temp3+1.348*max_UV_index_temper-0.0789*max_humid3-2.73
    MAX_HI=0.8*MAX_Tnw+0.05*MAX_Tg+0.15*max_temp3


    #ignore slippery index first, later add but no rainfall count is bad
    # 假设 UV_index 最大为 12，温度最大为 50°C，将 UV_index 映射到 0~50 范围 good method I guess?
    #統一量綱
    #經驗值
    #HI
    # 安全（绿色，< 21-25	安全 (Low)	0
    # 注意（黄色，25 – 28	注意 (Moderate)	1
    # 警戒（橙色，28 – 31	警戒 (High)	2
    # 危險 紫色，≥ 31	危险 (Extreme)	3
    # 嚴重 黑色，≥ 33	嚴重 (Extreme)	4
    #21-32

    #UV_index
    #0 – 2	低 (Low)	0
    # 3 – 5	中等 (Moderate)	1
    # 6 – 7	高 (High)	2
    # 8 - 9：强 / 很强 (Very High) 3
    # 10+：最强 / 极端 (Extreme)  4
    #0-11
    max_UV_scaled = (max_UV_index_temper / 11)*4
    UV_Scaled = (UV_index / 11)*4
    if HI<21:
        HI_Scaled=0
    else:
        HI_Scaled=(HI-21)/(32-21)*4
    if MAX_HI<21:
        max_HI_scaled=0
    else:
        max_HI_scaled=(MAX_HI-21)/(32-21)*4

    CRI=weighting_MAX_AVG*max(max_UV_scaled,max_HI_scaled)+(1-weighting_MAX_AVG)*(HI_Scaled*(weighting_of_UV_HI)+UV_Scaled*(1 - weighting_of_UV_HI))#max&avg
    payload1={#meed dict for post request
    "Temp":Temp3,
    "Humid":Humid3,
    "Pressure":pressure_temper,
    "Altitude":Altitude_temper,
    "UV_raw":UV_raw_temper,
    "UV_index":UV_index_temper,
    "Heat_Index": HI,
    "Comprehensive_index": CRI,
    "Delta_Temp3": delta_Temp3,
    "Delta_Humid3": delta_Humid3,   
    "Delta_UV_index": delta_UV_index
    }
    print(f"平滑後的數據 -> 溫度: {Temp3:.2f}, 濕度: {Humid3:.2f}, 氣壓: {Pressure:.2f}, 海拔: {Altitude:.2f}, UV_raw: {int(UV_raw_temper)}, UV_index: {int(UV_index_temper)}, 溫度變化: {delta_Temp3:.2f}, 濕度變化: {delta_Humid3:.2f}, UV變化: {delta_UV_index:.2f}")#again review f string
    return payload1


    
if __name__=="__main__":  #module 防止運行 but main can call direcrtly, c/c++ no need btw python also no need just def function variable struct... no any executing
    #while True:
    for i in range(10):
        calling_API(filtering_data(getting_data_from_pythonAnywhere()))
        time.sleep(10)





# all_temps = [item['Temp1'] for item in history]
# filtered_temp = sum(all_temps) / len(all_temps)