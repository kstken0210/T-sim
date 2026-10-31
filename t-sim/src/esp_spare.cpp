// <!DOCTYPE html>
// <html lang="zh-Hant">
// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>山間智慧站 - 風險地理監測</title>
//     <style>
//         body { font-family: 'Segoe UI', Arial, sans-serif; margin: 0; background-color: #f4f4f9; }
//         header { background-color: #2c3e50; color: white; padding: 15px; text-align: center; }
//         #map { width: 100%; height: calc(100vh - 75px); min-height: 500px; }

//         /* 圖例樣式優化 */
//         .map-legend {
//             position: absolute;
//             top: 90px;
//             right: 20px;
//             background: rgba(255, 255, 255, 0.95);
//             padding: 12px;
//             border-radius: 8px;
//             z-index: 1000;
//             box-shadow: 0 2px 10px rgba(0,0,0,0.15);
//             font-size: 13px;
//             line-height: 22px;
//             border: 1px solid #ddd;
//         }
//         .legend-item { display: flex; align-items: center; margin-bottom: 4px; }
//         .legend-color {
//             width: 14px;
//             height: 14px;
//             border-radius: 3px;
//             margin-right: 8px;
//             border: 1px solid rgba(0,0,0,0.1);
//         }

//         .info-box { padding: 10px; max-width: 250px; }
//         .danger-badge { display: inline-block; color: white; padding: 4px 12px; border-radius: 20px; font-weight: bold; margin-bottom: 8px; }
//         .data-row { display: flex; justify-content: space-between; margin: 4px 0; font-size: 13px; border-bottom: 1px dashed #eee; }
//     </style>
// </head>
// <body>

//     <header><h1>山間智慧站：實時風險地理分佈</h1></header>

//     <div class="map-legend">
//         <div style="font-weight: bold; margin-bottom: 8px; border-bottom: 1px solid #eee;">風險等級</div>
//         <div class="legend-item"><span class="legend-color" style="background: #27ae60;"></span>安全 (0.0 - 1.0)</div>
//         <div class="legend-item"><span class="legend-color" style="background: #f39c12;"></span>中等 (1.1 - 2.5)</div>
//         <div class="legend-item"><span class="legend-color" style="background: #e74c3c;"></span>危險 (2.6 - 4.0)</div>
//     </div>

//     <div id="map"></div>

//     <script>
//         const locations = {{ locations | tojson }};

//         function initMap() {
//             if (!locations || locations.length === 0) return;

//             const map = new google.maps.Map(document.getElementById("map"), {
//                 zoom: 16,
//                 center: { lat: locations[0].lat, lng: locations[0].lng },
//                 mapTypeId: 'terrain',
//                 styles: [{ featureType: "poi", elementType: "labels", stylers: [{ visibility: "off" }] }]
//             });

//             locations.forEach(loc => {
//                 const danger = parseFloat(loc.danger) || 0;
                
//                 // 1. 顏色與動態半徑邏輯
//                 const fillColor = danger > 2.5 ? '#e74c3c' : (danger > 1.0 ? '#f39c12' : '#27ae60');
//                 const radiusInMeters = 80 + (danger * 60); 

//                 // 2. 創建地理圓圈
//                 const riskCircle = new google.maps.Circle({
//                     strokeColor: fillColor,
//                     strokeOpacity: 0.6,
//                     strokeWeight: 2,
//                     fillColor: fillColor,
//                     fillOpacity: 0.35,
//                     map: map,
//                     center: { lat: loc.lat, lng: loc.lng },
//                     radius: radiusInMeters,
//                     clickable: true
//                 });

//                 const infoWindow = new google.maps.InfoWindow({
//                     content: `
//                         <div class="info-box">
//                             <h3>環境分析報告</h3>
//                             <div class="danger-badge" style="background: ${fillColor}">危險指數: ${loc.danger} / 4.0</div>
//                             <p style="font-size:13px;"><strong>🤖 AI 建議:</strong> ${loc.advice}</p>
//                             <div class="data-row"><span>🌡️ 溫度</span><span>${loc.Temp}</span></div>
//                             <div class="data-row"><span>💧 濕度</span><span>${loc.Humid}</span></div>
//                             <div class="data-row"><span>⛰️ 高度</span><span>${loc.Altitude}</span></div>
//                             <p style="font-size:11px; color:#999; margin-top:10px;">🕒 更新: ${loc.date}</p>
//                         </div>
//                     `
//                 });

//                 riskCircle.addListener("click", (event) => {
//                     infoWindow.setPosition(event.latLng);
//                     infoWindow.open(map);
//                 });
//             });
//         }
//     </script>

//     <script async defer src="https://maps.googleapis.com/maps/api/js?key={{ api_key }}&callback=initMap"></script>
// </body>
// </html>

// import sqlite3
// import os
// from flask import Flask, request, jsonify,render_template, redirect, url_for

// app = Flask(__name__)#create WSGI #problem1
// DB_PATH = '/home/yourusername/mysite/hiking_data2.db' #mock
// DB_PATH2 = '/home/yourusername/mysite/analysis_info4.db' #mock

// @app.route('/')
// def redirect_to_map():
//     return redirect(url_for('home'))

// @app.route('/update', methods=['POST','GET']) #problem2
// def handle_data():
//     if not request.is_json:
//         return "Request must be JSON", 400
//     # 1. 獲取 ESP32 傳來的數據 (對應你剛才拼的網址參數)
//     api_key = request.json.get('api_key')#request.args.get('api_key') what is this(previous) #problem3
//     Temp1 = request.json.get('Temp1')
//     Humid1 = request.json.get('Humid1')
//     Pressure = request.json.get('Pressure')
//     Altitude = request.json.get('Altitude')
//     UV_raw = request.json.get('UV_raw')
//     Uv_index = request.json.get('Uv_index')
//     Temp2 = request.json.get('Temp2')
//     Humid2 = request.json.get('Humid2')

//     # 2. 安全檢查
//     if api_key != "Your Server API Key":
//         return "Unauthorized", 401

//     # 3. 存入資料庫 (SQLite 範例)
//     conn = sqlite3.connect(DB_PATH)
//     c = conn.cursor()
//     #c.execute("INSERT INTO sensor_logs (Temp1, Humid1, Pressure, Altitude, UV_raw, Uv_index, Temp2, Humid2) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", (Temp1, Humid1, Pressure, Altitude, UV_raw, Uv_index, Temp2, Humid2))
//     c.execute("INSERT INTO sensor_readings (Temp1, Humid1, Pressure, Altitude, UV_raw, Uv_index, Temp2, Humid2) VALUES (?, ?, ?, ?, ?, ?, ?, ?)", (Temp1, Humid1, Pressure, Altitude, UV_raw, Uv_index, Temp2, Humid2))
//     conn.commit()
//     conn.close()

//     return "Data Saved!", 200

// @app.route('/post_analysis', methods=['POST'])
// def heatMap_Showing():
//     if not request.is_json:
//         return "Request must be JSON", 400
//     Temp = request.json.get('Temp')
//     Humid = request.json.get('Humid')
//     Pressure = request.json.get('Pressure')
//     Altitude = request.json.get('Altitude')
//     UV_index = request.json.get('UV_index')
//     Heat_Index = request.json.get('Heat_Index')
//     Comprehensive_index=request.json.get('Comprehensive_index')
//     advice = request.json.get('advice')
//     latitude = request.json.get('latitude')
//     longitude = request.json.get('longitude')

//     conn = sqlite3.connect(DB_PATH2)
//     c = conn.cursor()

//     c.execute("INSERT INTO danger_analysis (Temp, Humid, Pressure, Altitude, UV_index, Heat_Index, Comprehensive_index, advice, latitude, longitude) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", (Temp, Humid, Pressure, Altitude, UV_index, Heat_Index, Comprehensive_index, advice, latitude, longitude))
//     conn.commit()
//     conn.close()
//     return "Final_Data Saved!", 200

// @app.route('/map')
// def home():

//     conn = sqlite3.connect(DB_PATH2)
//     conn.row_factory = sqlite3.Row
//     c = conn.cursor()
//     c.execute("""SELECT Temp, Humid, Pressure,UV_index, Heat_Index, Altitude, Comprehensive_index, advice, latitude, longitude, datetime(analyzed_at, '+8 hours') as hk_time
//     FROM danger_analysis
//     ORDER BY id DESC LIMIT 3
//     """)
//     rows = c.fetchall()
//     conn.close()
//     locations = []
//     for row in rows:
//         locations.append({
//             "name": "CityU Campus Park",
//             "Temp": f"{row['Temp']:.1f}°C",
//             "Humid": f"{row['Humid']:.2f}%",
//             "UV_index": f"{row['UV_index']:.2f}",  # 修正引号和冒号位置
//             "Pressure": f"{row['Pressure']:.2f}hpa",
//             "Altitude" : f"{row['Altitude']:.2f}m",
//             "Heat_Index": f"{row['Heat_Index']:.2f}", # 補上 f-string
//             "danger" : f"{row['Comprehensive_index']:.2f}", # 補上 f-string
//             "advice": row['advice'],
//             "lat": row['latitude'],  # 經緯度建議不要用 .2f，否則地圖點會飄走
//             "lng": row['longitude'], # 經緯度建議不要用 .2f
//             #"date": row['date'] if 'date' in row.keys() else row['analyzed_at']
//             "date": row['hk_time']
//         })

//     api_key = "Your Google Maps API Key"
//     return render_template("index.html", locations=locations, api_key=api_key)

// @app.route('/view')
// def view_data():
//     conn = sqlite3.connect(DB_PATH)
//     c = conn.cursor()
//     # 查询最后 10 条记录
//     #c.execute("SELECT * FROM sensor_logs ORDER BY timestamp DESC LIMIT 10")
//     c.execute("SELECT * FROM sensor_readings ORDER BY timestamp DESC LIMIT 10")
//     rows = c.fetchall()
//     conn.close()

//     # 简单的网页显示
//     html = "<h1>山間智慧站 - 最近數據</h1><table border='1'>"
//     for row in rows:
//         html += f"<tr><td>{row}</td></tr>"
//     html += "</table>"
//     return html


// @app.route('/api/latest')
// def get_latest():
//     conn = sqlite3.connect(DB_PATH)
//     conn.row_factory = sqlite3.Row
//     c = conn.cursor()
//     # 抓取最後10筆數據
//     c.execute("SELECT * FROM sensor_readings ORDER BY id DESC LIMIT 6")
//     rows = c.fetchall()#!!! get the data from database and send out
//     conn.close()
//     if rows:
//         # 將資料庫的 tuple 轉成字典
//         data_list = [dict(row) for row in rows]
//         return jsonify(data_list)
//     return jsonify({"error": "no data"}), 404
// # o searching again or how the data format change from conn.row_factory->rows->data_list

