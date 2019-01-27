#include <time.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_SI1145.h"


#define WIFI_SSID "..."
#define WIFI_PWD "..."

ESP8266WebServer server(80);
IPAddress ip(192,168,42,100);
IPAddress subnet(255,255,255,0);

// HTML
const String HTML_HEADER = "<!doctype html>"\
  "<html><head><meta charset=\"UTF-8\"/>"\
  "<meta name=\"viewport\" content=\"width=device-width\"/>"\
  "</head><body>";
const String HTML_FOOTER = "</body></html>";

// LED
const int LED_PIN = 1;

// Software Serial
static const int RXPin = 0, TXPin = 15;
SoftwareSerial ss(RXPin, TXPin, false, 256);

// BME280 SPI
const uint8_t sclk = 14;
const uint8_t mosi =13; //Master Output Slave Input ESP8266=Master,BME280=slave 
const uint8_t miso =12; //Master Input Slave Output
const uint8_t cs_bme = 16; //CS pin
 
uint32_t hum_raw, temp_raw, pres_raw;
 int32_t t_fine;
 
uint16_t dig_T1;
 int16_t dig_T2;
 int16_t dig_T3;
  
uint16_t dig_P1;
 int16_t dig_P2;
 int16_t dig_P3;
 int16_t dig_P4;
 int16_t dig_P5;
 int16_t dig_P6;
 int16_t dig_P7;
 int16_t dig_P8;
 int16_t dig_P9;
  
uint8_t  dig_H1;
 int16_t dig_H2;
uint8_t  dig_H3;
 int16_t dig_H4;
 int16_t dig_H5;
 int8_t  dig_H6;
 
double SeaLevelPressure_hPa = 1013.25; //標準は1013.25

// PCF8523 RTC I2C
RTC_PCF8523 rtc;
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
unsigned int is_rtc_valid = 0;

// SI1145 i1145 UV/IR/Visible Light Sensor I2C
Adafruit_SI1145 uv = Adafruit_SI1145();
unsigned int is_si1145_valid = 0;


/** setup **/
void setup() {

  //- Debug Setting
  Serial.begin(115200);
  delay(20);
  Serial.println(F("Start"));

  // Wifi Setting
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip, ip, subnet);
  WiFi.softAP(WIFI_SSID, WIFI_PWD);

  //- LED Setting
  pinMode(LED_PIN, OUTPUT);

  // RTC Setting
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //while (1);
  } else {
    if (! rtc.initialized()) {
      Serial.println("RTC is NOT running!");
      // following line sets the RTC to the date & time this sketch was compiled
      rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
      // rtc.adjust(DateTime(2019, 1, 26, 0, 4, 0));
    }
    is_rtc_valid = 1;
  }

  //- si1145 Setting
  if (! uv.begin()) {
    Serial.println("Couldn't find Si1145");
    //while (1);
  } else {
    is_si1145_valid = 1;
  }
  
  //- BME280 SPI Setting
  uint8_t t_sb    = 5; //stanby 1000ms
  uint8_t filter  = 0; //filter O = off
  uint8_t spi3or4 = 0; //SPI 3wire or 4wire, 0=4wire, 1=3wire
  uint8_t osrs_t  = 4; //OverSampling Temperature x4
  uint8_t osrs_p  = 4; //OverSampling Pressure x4
  uint8_t osrs_h  = 4; //OverSampling Humidity x4
  uint8_t Mode    = 3; //Normal mode
 
  uint8_t ctrl_meas_reg = (osrs_t << 5) | (osrs_p << 2) | Mode;
  uint8_t config_reg    = (t_sb << 5) | (filter << 2) | spi3or4;
  uint8_t ctrl_hum_reg  = osrs_h;
   
  pinMode(sclk, OUTPUT);
  pinMode(miso, INPUT);
  pinMode(mosi, OUTPUT);
  pinMode(cs_bme, OUTPUT);
 
  digitalWrite(cs_bme, HIGH);
  
  writeReg(0xF2,ctrl_hum_reg);
  writeReg(0xF4,ctrl_meas_reg);
  writeReg(0xF5,config_reg);
  delay(1000);
   
  readTrim();

  //- Web Server Setting
  server.on("/", [](){
    Serial.println(F("/"));
    String html = "<h1>Hello! WebServer!</h1>";
    html += button_html();
    server.send(200, "text/html", html);
  });

  server.on("/led/on", [](){
    Serial.println(F("/led/on"));
    digitalWrite( LED_PIN, HIGH );
    String html = "<h1>LED ON</h1>";
    html += button_html();
    server.send(200, "text/html", html);
  });

  server.on("/led/off", [](){
    Serial.println(F("/led/off"));
    digitalWrite( LED_PIN, LOW );
    String html = "<h1>LED OFF</h1>";
    html += button_html();
    server.send(200, "text/html", html);
  });

  server.on("/rtc", [](){
    Serial.println(F("rtc"));  
    String html = rtc_html();
    server.send(200, "text/html", html);
  });

  server.on("/bme280", [](){
    Serial.println(F("bme280"));
    String html = bme280_html();
    server.send(200, "text/html", html);
  });

  server.on("/si1145", [](){
    Serial.println(F("si1145"));
    String html = si1145_html();
    server.send(200, "text/html", html);
  });

  server.on("/gps", [](){
    Serial.println(F("gps"));  
    String html = gps_html();
    server.send(200, "text/html", html);
  });
  
  server.begin();
}

/** main loop **/
void loop() {
  server.handleClient();
}

/** functions **/
//- button String -//
String button_html() {
    String html = "";
    html += "<button type=""button"" onclick=""location.href='/'""><h3>HOME</h3></button>";
    html += "<button type=""button"" onclick=""location.href='/led/on'""><h3>LED ON</h3></button>";
    html += "<button type=""button"" onclick=""location.href='/led/off'""><h3>LED OFF</h3></button>";
    html += "<button type=""button"" onclick=""location.href='/rtc'""><h3>RTC</h3></button>";
    html += "<button type=""button"" onclick=""location.href='/bme280'""><h3>BME280</h3></button>";
    html += "<button type=""button"" onclick=""location.href='/si1145'""><h3>SI1145</h3></button>";
    html += "<button type=""button"" onclick=""location.href='/gps'""><h3>GPS</h3></button>";
    return html;
}

//- rtc datetime -//
String get_rtc_datetime() {
//Serial.println(F("get_rtc_datetime"));
  String str_date = "None";
  String str_time = "None";

  if (is_rtc_valid == 1) {
//Serial.println(F("rtc_html.1"));
    DateTime now = rtc.now();
    int y = now.year();
    int m = now.month();
    int d = now.day();
    int h = now.hour();
    int mi = now.minute();

//Serial.println(F("rtc_html.2"));
    char cy[5], cm[3], cd[3], ch[3], cmi[3];
    sprintf(cy,  "%d", y);
    sprintf(cm,  "%d", m);
    sprintf(cd,  "%d", d);
    sprintf(ch,  "%d", h);
    sprintf(cmi, "%d", mi);

//Serial.println(F("rtc_html.3"));
    String sy = cy;
    String sm = cm;
    String sd = cd;
    String sh = ch;
    String smi = cmi;
    String dow = daysOfTheWeek[now.dayOfTheWeek()];

//Serial.println(F("rtc_html.4"));      
    str_date = sy + "/" + sm + "/" + sd + " " + dow;
    str_time = sh + ":" + smi;
      
//Serial.println(F("rtc_html.9"));
  }
  return str_date + " " + str_time;
}

//- RTC -//
String rtc_html() {
    String html = "<h1>RTC</h1>";
    html += "<p>" + get_rtc_datetime() + "</p>";
    html += button_html();
    return html;
}

//- SI1145 -//
String si1145_html() {
  int vis = uv.readVisible();
  int ir = uv.readIR();
  float uv_index = uv.readUV();
  uv_index /= 100.0;  

  char cvis[5], cir[5], cui[6];
  sprintf(cvis,  "%d", vis);
  sprintf(cir,   "%d", ir);
  dtostrf(uv_index, 3, 2, cui);

  String svis = cvis;
  String sir = cir;
  String sui = cui;
  
  String html = "<h1>SI1145</h1>";
  html += "<p>" + get_rtc_datetime() + "</p>";
  html += "<p>vis = " + svis + "</p>";
  html += "<p>ir  = " + sir  + "</p>";
  html += "<p>uv  = " + sui  + "</p>";
  html += button_html();
  return html;
}

//- GPS -//
String gps_html() {
    TinyGPSPlus gps;

    // GPSが衛星からのデータを受信しているとき
    char buf1[9], buf2[7], alt[16], lat[16], let[16];
    int  loop_exit = 0;
    int  MAX_LOOP_CNT = 5000;
    int  loop_cnt  = 0;

    while (!loop_exit) {

      while (ss.available() > 0) {
        //データを読む
        char c = (ss.read());
  
        //データを解読
        gps.encode(c);
  
        // ロケーションと日時がある場合
        if (gps.location.isValid() && gps.altitude.isValid() && gps.date.isValid() && gps.time.isValid()) {

          // 日時
          String date = String(gps.date.value());
          //Serial.println(date);
          String time = String(gps.time.value());
          //Serial.println(time);

          struct tm now; // 日時設定
          now.tm_year = date.substring(4).toInt()+100; //1900年～
          now.tm_mon = date.substring(2,4).toInt() -1;//月変換は-1
          now.tm_mday = date.substring(0,2).toInt();
          now.tm_hour = time.substring(0,2).toInt();
          now.tm_min = time.substring(2,4).toInt();
          now.tm_sec = time.substring(4,6).toInt();
          time_t tim; // 総秒時間
          tim = mktime(&now)+9*60*60; // 日本時間変換（＋9時間）
          struct tm *nowj; // 日時設定（日本時間）
          nowj = localtime(&tim); // 日時構造体 tm に変換

          // 日付
          strftime(buf1,sizeof(buf1),"%Y%m%d",nowj);

          // 時刻
          strftime(buf2,sizeof(buf2),"%H%M%S",nowj); // 時刻 

          // 高度、緯度、経度
          dtostrf(gps.altitude.meters(), 6, 2, alt);
          dtostrf(gps.location.lat(), 12, 8, lat);
          dtostrf(gps.location.lng(), 12, 8, let);

          loop_exit = 1;
          break;
          
        } else {
          loop_cnt += 1;
          if (loop_cnt > MAX_LOOP_CNT) {
            break;          
          }
        }
        //Serial.println(loop_cnt);
      }
      if (loop_exit == 0) {
        loop_cnt += 1;
        if (loop_cnt > MAX_LOOP_CNT) {
          break;          
        }
      }
    }
    String html = "<h1>GPS</h1>";
    String str_date = "None";
    String str_time = "None";
    String str_alt = "None";
    String str_lat = "None";
    String str_let = "None";
    if (loop_exit == 1) {
      str_date = buf1;
      str_time = buf2;
      str_alt = alt;
      str_lat = lat;
      str_let = let;
    }
    html += "<p>date:" + str_date + "</p>";
    html += "<p>time:" + str_time + "</p>";
    html += "<p>alt:"  + str_alt  + "m</p>";
    html += "<p>lat:"  + str_lat  + "</p>";
    html += "<p>let:"  + str_let  + "</p>";
    html += button_html();
    return html;
}

//- BME280
void readTrim(void) {
    //  初期値読み込み

    dig_T1 = read16bit(0x88);
    dig_T2 = (int16_t)read16bit(0x8A);
    dig_T3 = (int16_t)read16bit(0x8C);
 
    dig_P1 = read16bit(0x8E);
    dig_P2 = (int16_t)read16bit(0x90);
    dig_P3 = (int16_t)read16bit(0x92);
    dig_P4 = (int16_t)read16bit(0x94);
    dig_P5 = (int16_t)read16bit(0x96);
    dig_P6 = (int16_t)read16bit(0x98);
    dig_P7 = (int16_t)read16bit(0x9A);
    dig_P8 = (int16_t)read16bit(0x9C);
    dig_P9 = (int16_t)read16bit(0x9E);
 
    dig_H1 = read8bit(0xA1);
    dig_H2 = (int16_t)read16bit(0xE1);
    dig_H3 = read8bit(0xE3);
    dig_H4 = (int16_t)((read8bit(0xE4) << 4) | (read8bit(0xE5) & 0x0F));
    dig_H5 = (int16_t)((read8bit(0xE6) << 4) | (read8bit(0xE5) >> 4));
    dig_H6 = (int8_t)read8bit(0xE7);
}
void writeReg(uint8_t reg_address, uint8_t data) {
    //  BME280へ初期レジスタ書き込み関数

    digitalWrite(cs_bme, LOW);
    SpiWrite(reg_address & B01111111); // write, bit 7 low
    SpiWrite(data);
    digitalWrite(cs_bme, HIGH);
}
void readData() {
    //  BME280からの温度、湿度、気圧データ読み込み関数

    uint32_t data[8];
    uint8_t i;
   
    digitalWrite(cs_bme, LOW);
    SpiWrite(0xF7 | B10000000); //0xF7 pressure msb read, bit 7 high
    for(i=0; i<8; i++){
      data[i] = SpiRead();
    }
    digitalWrite(cs_bme, HIGH);
    pres_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4); //0xF7, msb+lsb+xlsb=19bit
    temp_raw = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4); //0xFA, msb+lsb+xlsb=19bit
    hum_raw  = (data[6] << 8) | data[7];  //0xFD, msb+lsb=19bit(16:0)
}
int32_t calibration_T(int32_t adc_T) {
    //  温度キャリブレーション関数

    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
     
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return T; 
}
uint32_t calibration_P(int32_t adc_P) {
    //  気圧キャリブレーション関数
    int32_t var1, var2;
    uint32_t P;
    var1 = (((int32_t)t_fine)>>1) - (int32_t)64000;
    var2 = (((var1>>2) * (var1>>2)) >> 11) * ((int32_t)dig_P6);
    var2 = var2 + ((var1*((int32_t)dig_P5))<<1);
    var2 = (var2>>2)+(((int32_t)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((int32_t)dig_P2) * var1)>>1))>>18;
    var1 = ((((32768+var1))*((int32_t)dig_P1))>>15);
    if (var1 == 0) {
        return 0;
    }    
    P = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;
    if(P<0x80000000) {
       P = (P << 1) / ((uint32_t) var1);   
    }else{
        P = (P / (uint32_t)var1) * 2;    
    }
    var1 = (((int32_t)dig_P9) * ((int32_t)(((P>>3) * (P>>3))>>13)))>>12;
    var2 = (((int32_t)(P>>2)) * ((int32_t)dig_P8))>>13;
    P = (uint32_t)((int32_t)P + ((var1 + var2 + dig_P7) >> 4));
    return P;
}
uint32_t calibration_H(int32_t adc_H) {
    //  湿度キャリブレーション関数
    int32_t v_x1;
     
    v_x1 = (t_fine - ((int32_t)76800));
    v_x1 = (((((adc_H << 14) -(((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1)) + 
              ((int32_t)16384)) >> 15) * (((((((v_x1 * ((int32_t)dig_H6)) >> 10) * 
              (((v_x1 * ((int32_t)dig_H3)) >> 11) + ((int32_t) 32768))) >> 10) + ((int32_t)2097152)) * 
              ((int32_t) dig_H2) + 8192) >> 14));
   v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
   v_x1 = (v_x1 < 0 ? 0 : v_x1);
   v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);
   return (uint32_t)(v_x1 >> 12);   
}
double ReadAltitude(double SeaLevel_Pres, double pressure) {
    //  標高計算関数
  
    double altitude = 44330.0 * (1.0 - pow(pressure / SeaLevel_Pres, (1.0/5.255)));  
    return altitude;
}
uint16_t read16bit(uint8_t reg) {
    //  BME280から16bitデータ読み込み関数
  
    uint16_t d1, d2;
    uint16_t data;
    digitalWrite(cs_bme, LOW);
    SpiWrite(reg | B10000000); // read, bit 7 high
    d1 = SpiRead();
    d2 = SpiRead();
    data = (d2 << 8) | d1;
    digitalWrite(cs_bme, HIGH);
    return data;
}
uint8_t read8bit(uint8_t reg) {
    //  BME280から8bitデータ読み込み関数
  
    uint8_t data;
    digitalWrite(cs_bme, LOW);
    SpiWrite(reg | B10000000); // read, bit 7 high
    data = SpiRead();
    digitalWrite(cs_bme, HIGH);
    return data;
}
void SpiWrite(uint8_t data) {
    //  BME280へSPI信号データ送信関数
  
    for (int i=7; i>=0; i--) {
      digitalWrite(sclk, LOW);
      digitalWrite(mosi, data & (1<<i));
      digitalWrite(sclk, HIGH);
    }
}
uint8_t SpiRead() {
  //  BME280からのSPI信号データ読み込み関数
  uint8_t r_data = 0;
  for (int i=7; i>=0; i--) {
    r_data <<= 1;
    digitalWrite(sclk, LOW);
    digitalWrite(mosi, LOW);
    digitalWrite(sclk, HIGH);
    if(digitalRead(miso)){
      r_data |= 1;
    }
  }
  return r_data;
}
String bme280_html() {
    //  BME280
    double temp_act = 0.0, press_act = 0.0, hum_act = 0.0, altitude_act = 0.0;
    int32_t temp_cal;
    uint32_t press_cal,hum_cal;
   
    readData();
   
    temp_cal = calibration_T(temp_raw);
    press_cal = calibration_P(pres_raw);
    hum_cal = calibration_H(hum_raw);
    temp_act = (double)temp_cal / 100.0;
    press_act = (double)press_cal / 100.0;
    hum_act = (double)hum_cal / 1024.0;
    altitude_act = ReadAltitude(SeaLevelPressure_hPa, press_act);

    delay(2000);
    /*
    Serial.println("-----------------------");
    Serial.print("Temperature = "); Serial.print(temp_act); Serial.println(" *C");
    Serial.print("Humidity = "); Serial.print(hum_act); Serial.println(" %");
    Serial.print("Pressure = "); Serial.print(press_act); Serial.println(" hPa");
    Serial.print("Altitude = "); Serial.print(altitude_act); Serial.println(" m");
    */
    char  char_temp[16];
    char  char_hum[16];
    char  char_press[16];
    char  char_alt[16];
    dtostrf(temp_act,     6,  2, char_temp);
    dtostrf(hum_act,      6,  2, char_hum);
    dtostrf(press_act,    6,  2, char_press);
    dtostrf(altitude_act, 6,  2, char_alt);
    
    String  str_temp  = char_temp;
    String  str_hum   = char_hum;
    String  str_press = char_press;
    String  str_alt   = char_alt;
    String  html = "<h1>BME280</h1>";
    html += "<p>" + get_rtc_datetime() + "</p>";
    html += "<p>Temperature = " + str_temp  + " *C</p>";
    html += "<p>Humidity    = " + str_hum   + " %</p>";
    html += "<p>Pressure    = " + str_press + " hPa</p>";
    html += "<p>Altitude    = " + str_alt   + " m</p>";
    html += button_html();

    return html;
}
