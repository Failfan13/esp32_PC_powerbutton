// #include <WiFiManager.h>
// #include <WiFi.h>

// // Set wifi SSID and Pass to none
// String WiFi_SSID = "Esp32_setup";
// String WiFi_Pass = "password";

// // Set deploy IP
// const char* DeployIP = "192.168.4.20";

// WiFiServer server(80);

// int WiFi_Connect() {
//   WiFi.mode(WIFI_STA);

//   Serial.begin(115200);
//   Serial.println("Hello, ESP32!");

//   WiFiManager wm;

//   bool res;
//   res = wm.autoConnect(WiFi_SSID.c_str(), WiFi_Pass.c_str());

//   if (!res) {
//     Serial.println("Failed to connect");
//   }
//   else {
//     Serial.println("Connected to WiFi");
//   }
// }