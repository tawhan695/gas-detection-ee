
#include <LiquidCrystal_I2C.h> //ของจอ
#include <TridentTD_LineNotify.h>

#define SSID        "3bb-wlan"  //เปลี่ยนเอานะ ชื่อไวฟาย
#define PASSWORD    "T0655577688"  // รหัส
#define LINE_TOKEN  "5DAl1HbfwbqN0INDzwgal2Kfcj2xo7nSgGCa5zZ2LOk"

///////////////////////////////////////////////////////////////////////
  //กำหนดขาสวิตซ์ที่เชื่อมต่อกับ sensor smoke
#define PIN_SENSOR 33
////////////////////////////////////////////////////////////////////////
///lcd I2C device found at address 0x27
//กำหนดขนาด จอ 16X2
int lcdColumns = 16;  
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); //ตั้งค่าที่อยู่ LCD จำนวนคอลัมน์และแถว

/////////////////////////////////////////////////////////////////////////

 //ตัวแปลจับเวลา alrm
int period = 500;
unsigned long time_now = 0;
//////////////////////////////////
//ตัวแปลจับเวลาส่งข้อความไปไลน์
int period2 = 1000*60*5;
unsigned long time_now2 = 0;
///////////////////////////////////
int pinAlarm =25;
int cudown =0;
int cudown2 =0;
///////////////////////////////////
int LED_RAD= 13;
int LED_GREEN =12;

//////////////////////////////////



void setup() {


  ///////////////////////////////////////
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight(); 
  lcd.setCursor(0, 0); // ตั้งค่าเคอร์เซอร์ไปที่คอลัมน์แรกแถวแรก
/////////////////////////////////////////
  pinMode(pinAlarm, OUTPUT);
  digitalWrite(pinAlarm,0);

//////////////////////////////////////////
//LED
  pinMode(LED_RAD, OUTPUT);
  digitalWrite(LED_RAD,0);
    pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_GREEN,0);
  
///////////////////////////////////////////
 //              เชื่อมต่อกับ wifi
///////////////////////////////////////////
  Serial.begin(115200); Serial.println();
  Serial.println(LINE.getVersion());
  
  WiFi.begin(SSID, PASSWORD);
  lcd.setCursor(0, 0);
  lcd.printf("WiFi connecting to %s\n",  SSID);
  lcd.setCursor(0, 2);
  while(WiFi.status() != WL_CONNECTED) { lcd.print("."); delay(400); }
  lcd.setCursor(0, 0);
  lcd.printf("\nWiFi connected\nIP : ");
  lcd.setCursor(0, 2);
  lcd.println(WiFi.localIP());  
  delay(2000);

  ///////////////////////////////////////////////////
  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);
  ///////////////////////////////////////////////////

  //ส่งข้อความ
  LINE.notify("ระบบออนไลน์");

}

void loop() {
 
  int mq2 =analogRead(PIN_SENSOR);
  int mq2Map = map(mq2,200,1000,0,100);

  //เงื่อนไขไม่ให้ค่าติดลบ
  if(mq2Map<0){
    mq2Map=0;
    digitalWrite(pinAlarm,0);  
  }

  String value;
  lcd.setCursor(0, 0);
  value += "Value : ";
  value +=String(mq2Map);
  value +=" %";
  lcd.print(value);
  
///////////////////////////////////////////////////////////////
//         ส่วนนี้ จะเป็นเงื่อนไข  เมื่อ ค่าจาก sensor เกินกำหนดที่เรากำหนดไว้ คือ if(mq2Map>=20){ (20 สามารถแก้ไขได้ตามที่เราต้องการ)
//         มันจะส่ง ข้อความไปยังไลน์
//
///////////////////////////////////////////////////////////////  
     if(mq2Map >= 20){   //เมื่อค่าจาก sensor จากตัวแปล mq2Map  มากกว่าหรือเท่ากับ ให้ เงื่อนไขนี้ทำงาน 
          
           lcd.setCursor(0, 1);
           lcd.println("Status : danger");       
           cudown2+=1; //ตัวแปลนี้ เอาไว้นำเมื่อถึง ห้าวิ ก็จะให้แจ้งเตือนไปยังไลน์ หนึ่งครั้ง   ๑๑๑ มันจะนับเริ่มจาก 1 2 3 4 5 6 ไปเรื่อยๆจนว่าค่าจาก เซ็นเชอร์ จะน้อยกว่า 20 เมื่อค่าน้อยกว่า 20  ค่าที่นับจะเป็น 0 
          if(cudown2==5){  //เมื่อค่าที่นับเท่ากับ ๕ จะส่งข้อความไปยังไลน์
              String value2;
              value2 +="###แจ้งเตือน !####\n"; // += คือ  value3 = value3 + "ข้อความ" เอาตัวมันเองเท่ากับตัวมันเองแล้วบวกด้วยข้อความ คือการเอาข้อความมาต่อกัน
              value2 +="----แก๊ส&ควันเกินขีดจำกัด----\n";
              value2 += "   Value : ";
              value2 +=String(mq2Map); // mq2Map มีค่าเป็น int จึงทำการแปลงค่า ให้เป็น String ตัวคำสั่งนี String(mq2Map);้ 
              value2 +=" %";
              LINE.notify(value2);   // ฟังก์ชันนี้ เรียกใช้จาก libary TridentTD_LineNotify เอาค่า value2 ที่ต่อกัน ส่งไปยังไลน์
          }
          
/////////////////////////////////////////////////////////////////
//             ส่วนนี้ เป็นของ ตัวส่งเสียง
//             เมื่อ ค่าจาก sensor มากกว่า 20 มันจะส่งเสียง 
//             แบบ ติดดับ ติดดับ ความถี่ติดดับ สามารถตั้งค่าได้ จากตัวแปล period 
/////////////////////////////////////////////////////////////////
        if(millis() > time_now + period){  //
            time_now = millis();
             // สั่งให้ หลอดไปติดดับ
             digitalWrite(LED_RAD,1);    // จะใช้ (LED_RAD,1); หรือ (LED_RAD,HIGHT); ความหมาเดียวกัน
             digitalWrite(LED_GREEN,0);    // จะใช้ (LED_RAD,0); หรือ (LED_RAD,LOW); ความหมาเดียวกัน
             digitalWrite(pinAlarm,1);      // สั่งให้ส่งเสียงแจ้งเตือน                 

       }
       }else{
                 lcd.setCursor(0, 1);
                 lcd.print("Status : safe");
                 // สั่งให้ หลอดไปติดดับ
                 digitalWrite(LED_RAD,0);
                 digitalWrite(LED_GREEN,1);
                 cudown2=0;
       }


///////////////////////////////////////////////////////////////////
//                 ส่วนนี้จะดีเล ส่งข้อความไปยังไลน์
//                   period2 =1000*60*5  มีค่าเท่ากับ 5 นาที แปลงมาจาก มิลลิวินาที ตัวแปลนี้จะตั้งค่าการหน่วงเวลาจากด้านบน 
////////////////////////////////////////////////////////////////////                   
    if(millis() > time_now2 + period2){  //จับเวลาได้ครบ ห้านาที ก็จะเข้าฟังชันนี้
       String value3; //ตัวเเปลเก็บข้อความ
       if(mq2Map>=20){
              time_now2 = millis();
              value3 +="###แจ้งเตือน !####\n";   // += คือ  value3 = value3 + "ข้อความ" เอาตัวมันเองเท่ากับตัวมันเองแล้วบวกด้วยข้อความ คือการเอาข้อความมาต่อกัน
              value3 +="----แก๊ส&ควันเกินขีดจำกัด----\n";
              value3 += "   Value : ";
              value3 +=String(mq2Map);  // mq2Map มีค่าเป็น int จึงทำการแปลงค่า ให้เป็น String ตัวคำสั่งนี String(mq2Map);้ 
              value3 +=" %";
              LINE.notify(value3); // ฟังก์ชันนี้ เรียกใช้จาก libary TridentTD_LineNotify เอาค่า value3 ที่ต่อกัน ส่งไปยังไลน์
       }else{
              time_now2 = millis();
              value3 +="    แจ้งเตือน !\n"; // // += คือ  value3 = value3 + "ข้อความ" เอาตัวมันเองเท่ากับตัวมันเองแล้วบวกด้วยข้อความ คือการเอาข้อความมาต่อกัน
              value3 +="----แก๊ส&ควันเกินขีดจำกัด----\n";
              value3 += "   Value : ";
              value3 +=String(mq2Map); // mq2Map มีค่าเป็น int จึงทำการแปลงค่า ให้เป็น String ตัวคำสั่งนี String(mq2Map);้ 
              value3 +=" %";
              LINE.notify(value3); // ฟังก์ชันนี้ เรียกใช้จาก libary TridentTD_LineNotify เอาค่า value3 ที่ต่อกัน ส่งไปยังไลน์
       }
 
    }
               // end 
 /////////////////////////////////////////////////////////////////   
  delay(1000);

     digitalWrite(pinAlarm,0);       // สั่งให้ปิดส่งเสียงแจ้งเตือน  
  lcd.clear();
}
