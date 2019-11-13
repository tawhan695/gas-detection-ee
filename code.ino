#include <WiFi.h>
#include <WiFiClientSecure.h>
//########## CONFIGURATION : MODIFY HERE ##########

char ssid[] = "myssid";  //ขื่อวายฟาย SSID
char pass[] = "mypassword"; //รหัสผ่านวายฟาย

String txt = "Switch pressed";  //ข้อความที่จะแสดงใน Line

//รหัสเชื่อมต่อ Line TOKEN
#define TokenLine "Ynzzovc8dway85pRSJKNvtUIAzlkPnvBPs9MUpaBWRA"
//กำหนดขาสวิตซ์ที่เชื่อมต่อกับ sensor smoke
#define PIN_SENSOR D1

//#################################################
boolean oldState;
boolean data;
void setup() {
//กำหนดให้ขาดิจิตอล SWPIN รับข้อมูลจากสวิตซ์
pinMode(PIN_SENSOR, INPUT);
//ตั้งค่าคอนโซล
Serial.begin(9600);
//เริ่มการเชื่อมต่อกับวายฟายแม่ข่าย
WiFi.begin(ssid, pass);
//แสดง "WiFi Connecting" ในคอนโซล
Serial.print("WiFi Connecting");
//ตรวจเช็คสถานะการเขื่อมต่อวายฟาย
//ถ้าไม่สำเร็จให้แสดง "." ในคอนโซลจนกว่าจะเขื่อมต่อได้
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
//แสดงสถานะการวายฟายเชื่อต่อแล้ว และแสดงหมายเลขไอพีที่ได้รับในคอนโซล
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
}
void loop() {
  data = digitalRead(SWPIN);
  if (data == 0 and oldState == 1) {
    delay(10);
      if(digitalRead(SWPIN) == 0) {
        Serial.println(txt);
        NotifyLine(txt);
      }
  }
 oldState = data;
}
void NotifyLine(String t) {
WiFiClientSecure client;
if (!client.connect("notify-api.line.me", 443)) {  // เชื่อมต่อและตรวจสอบสถานะ การเชื่อมต่อ
 Serial.println("Connection failed");
 return;
}


/*
  ส่งข้อความผ่าน โปรโตคอล Http 1.1  ส่งไปยัง server ของไลน์  url : notify-api.line.me ตามด้วยกุญแจ คือ TokenLine "Ynzzovc8dway85pRSJKNvtUIAzlkPnvBPs9MUpaBWRA"
  หรือเป็น เส้นทางที่จะส่งข้อมูลไป
*/
String req = "";
req += "POST /api/notify HTTP/1.1\r\n";
req += "Host: notify-api.line.me\r\n";
req += "Authorization: Bearer " + String(TokenLine) + "\r\n";
req += "Cache-Control: no-cache\r\n";
req += "User-Agent: ESP32\r\n";
req += "Content-Type: application/x-www-form-urlencoded\r\n";
req += "Content-Length: " + String(String("message=" + t).length()) + "\r\n";
req += "\r\n";
req += "message=" + t;
Serial.println(req); // debug serial port
client.print(req);  // ส่งข้อความไปยังไลน์
delay(20);
Serial.println("-------------");
 
while(client.connected()) {  //เชื่อมต่อกับ Line 
  String line = client.readStringUntil('\n');
  if (line == "\r") {
    break;
  }
 } //while
} //loop