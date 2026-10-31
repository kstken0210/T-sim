// <!DOCTYPE html>
// <html lang="zh-Hant">
// <head>
//     <meta charset="UTF-8">
//     <meta name="viewport" content="width=device-width, initial-scale=1.0">
//     <title>山間智慧站 - 冒險熱力圖</title>
//     <style>
//         body {
//             font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
//             margin: 0;
//             padding: 0;
//             background-color: #f4f4f9;
//         }

//         header {
//             background-color: #2c3e50;
//             color: white;
//             padding: 15px 20px;
//             text-align: center;
//             box-shadow: 0 2px 5px rgba(0,0,0,0.2);
//         }

//         #map {
//             width: 100%;
//             height: calc(100vh - 80px); /* 減去 header 高度 */
//             min-height: 500px;
//         }

//         /* InfoWindow 樣式微調 */
//         .info-box {
//             padding: 10px;
//             max-width: 250px;
//         }
//         .info-box h3 {
//             margin: 0 0 10px 0;
//             font-size: 18px;
//             border-bottom: 2px solid #eee;
//             padding-bottom: 5px;
//         }
//         .info-box p {
//             margin: 5px 0;
//             font-size: 14px;
//             line-height: 1.4;
//         }
//         .danger-tag {
//             display: inline-block;
//             background: #e74c3c;
//             color: white;
//             padding: 2px 8px;
//             border-radius: 4px;
//             font-weight: bold;
//         }
//     </style>
// </head>
// <body>

//     <header>
//         <h1>山間智慧站：環境風險實時監測</h1>
//     </header>

//     <div id="map"></div>

//     <script>
//         // 從 Flask 接收到的數據
//         const locations = {{ locations | tojson }};

//         function initMap() {
//             if (!locations || locations.length === 0) {
//                 console.error("無數據可顯示");
//                 return;
//             }

//             // 1. 初始化地圖中心點（以第一條數據為準）
//             const mapCenter = { lat: locations[0].lat, lng: locations[0].lng };
            
//             const map = new google.maps.Map(document.getElementById("map"), {
//                 zoom: 15,
//                 center: mapCenter,
//                 mapTypeId: 'terrain', // 使用地形圖，更適合登山場景
//                 mapTypeControl: true,
//                 streetViewControl: false
//             });

//             // 2. 準備熱力圖數據
//             const heatData = locations.map(loc => {
//                 return {
//                     location: new google.maps.LatLng(loc.lat, loc.lng),
//                     // 權重直接使用你的 Comprehensive_index
//                     weight: parseFloat(loc.danger) || 1.0 
//                 };
//             });

//             // 3. 創建並配置熱力圖層
//             const heatmap = new google.maps.visualization.HeatmapLayer({
//                 data: heatData,
//                 map: map,
//                 radius: 40,      // 紅暈擴散半徑
//                 opacity: 0.8,    // 透明度
//                 // 漸變色配置：綠 (安全) -> 黃 (中等) -> 紅 (危險)
//                 gradient: [
//                     'rgba(0, 255, 255, 0)',   // 起始透明
//                     'rgba(0, 255, 0, 1)',     // 綠色
//                     'rgba(255, 255, 0, 1)',   // 黃色
//                     'rgba(255, 165, 0, 1)',   // 橙色
//                     'rgba(255, 0, 0, 1)'      // 紅色
//                 ]
//             });

//             // 4. 為每個點添加透明的 Marker 以便點擊查看 AI 建議
//             locations.forEach(loc => {
//                 const marker = new google.maps.Marker({
//                     position: { lat: loc.lat, lng: loc.lng },
//                     map: map,
//                     // 設置 icon 為透明或極小，讓用戶感覺是點擊了熱力圖區域
//                     icon: {
//                         path: google.maps.SymbolPath.CIRCLE,
//                         scale: 10,
//                         fillOpacity: 0,
//                         strokeOpacity: 0
//                     }
//                 });

//                 const infoWindow = new google.maps.InfoWindow({
//                     content: `
//                         <div class="info-box">
//                             <h3>環境分析報告</h3>
//                             <p><span class="danger-tag">危險指數: ${loc.danger}</span></p>
//                             <p><strong>🤖 AI 建議:</strong> ${loc.advice}</p>
//                             <hr>
//                             <p>🌡️ 溫度: ${loc.Temp}</p>
//                             <p>💧 濕度: ${loc.Humid}</p>
//                             <p>氣壓: ${loc.Pressure}</p>
//                             <p>高度: ${loc.Altitude}</p>
//                             <p>紫外綫: ${loc.UV_index}</p>
//                             <p>🕒 時間: ${loc.date}</p>
//                         </div>
//                     `
//                 });

//                 marker.addListener("click", () => {
//                     infoWindow.open(map, marker);
//                 });
//             });
//         }
//     </script>

//     <script async defer
//         src="https://maps.googleapis.com/maps/api/js?key={{ api_key }}&libraries=visualization&callback=initMap">
//     </script>

// </body>
// </html>


// # 📍 地理設施與路徑描述：城大山間小徑 (Demo)

// ### 1. 區域概況

// * **名稱**：城大後山智慧監測路段
  
// * **特徵**：此區域連接校園與後山，地形由低往高延伸。

// ### 2. 設施分佈與海拔關係

// * **低海拔區域 (接近起點/校園)**
  
//     * **設施**：設有**休息區**。
  
//     * **詳情**：此處設有大量**座椅 (Benches)**，環境較為開闊且有樹蔭遮蔽。
  
//     * **AI 聯動建議**：若監測到熱壓力指數 (Heat Index) 過高，應建議登山者在此多作停留。

// * **中段區域**
  
//     * **設施**：設有**涼亭 (Pavilion)**。
  
//     * **詳情**：路徑中段提供避雨及遮陽設施。

// * **高海拔區域 (接近頂端)**
  
//     * **設施**：**觀景臺 (Observatory Deck)**。
  
//     * **詳情**：無視野遮擋，適合觀察天氣與拍照。
  
//     * **風險提示**：此處暴露感強，風速可能較大且紫外綫 (UV) 遮擋較少。

// ### 3. 路徑提示

// * **上行方向**：前往觀景臺，坡度約 15 度，體力消耗較大。
// * 
// * **下行方向**：返回城大校園，路面多石階，雨後需注意防滑。