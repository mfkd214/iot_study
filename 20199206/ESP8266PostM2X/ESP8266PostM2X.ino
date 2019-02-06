#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

const char* ssid      = "<your ssid>";
const char* pwd       = "<your wifi passphrase>";

WiFiClient m2x_client;
const char* m2x_host  = "api-m2x.att.com";
String m2x_device_id = "<your m2x device id>";
String m2x_stream_id = "<your m2x stream id>";
String m2x_api_key = "<your m2x api key>";
/***********************************************************************
 *  START
 ***********************************************************************/
void setup() {
  Serial.begin(115200);
  delay(10);

  // WiFi Connect
  start_wifi();
  delay(1000);
}
/***********************************************************************
 *  LOOP
 ***********************************************************************/
void loop() {

  //  データを取得
  int val = get_data();
  if (val != 0) {

    //  データをPOSTする
    post_m2x(val);

  }

  //  5分待つ
  delay(300000);
}

//======================================================================
//  Wifiに接続する
//======================================================================
void start_wifi() {

  // We start by connecting to a WiFi network
  Serial.println("starting wifi");
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, pwd);

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("wifi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//======================================================================
//  データセンシング
//======================================================================
int get_data() {
  // とりあえず
  return 20;
}
//======================================================================
//  m2xにデータをPOSTする
//======================================================================
void post_m2x(int val) {
  Serial.println("chalenge connection m2x");
  if (!m2x_client.connect(m2x_host, 80)) {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  //M2xにデータを送信する
  m2x_client.print(
      "PUT /v2/devices/" + m2x_device_id + "/streams/" + m2x_stream_id + "/value HTTP/1.1\r\n" +
      "Host: " + m2x_host + "\r\n" +
      "User-Agent: curl/7.43.0\r\n" +
      "Accept: */*\r\n" +
      "X-M2X-KEY: " + m2x_api_key + "\r\n" +
      "Content-Type: application/json\r\n" +
      // "Connection: close\r\n" +
      "Content-Length: 17\r\n\r\n" +
      "{ \"value\": \"" + val + "\" }\r\n");
  Serial.printf("sent %d to the m2x server\n", val);

  String response = m2x_client.readString();
  Serial.println(response);

  Serial.println("closing m2x connection");
  m2x_client.stop();
}
