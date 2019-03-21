2019.03.20 菅笠くんとカメラなし菅笠くん候補の消費電力について
====

# RaspberryPi ZW(UK) 5V

- 現すげがさくん（UART：GPS、I2C：温度、湿度、気圧、照度、カメラ）
- WiFi, BLE。SD起動なので外部ストレージとしても使える。Pin40。
- Debian系Linux
- VS CODE + Pythonが主かな。所詮LinuxなんでGPIO扱えるならなんでも。
- 本体 ￥1,800くらい + 8G以上のSDカード代金。4G以上で動くけど、OS自体で4G近くくうんで大した事できなくなっちゃう。

消費電力
---
    130mA～0.3A OFF時:40mA
    https://blogs.yahoo.co.jp/bokunimowakaru/55700430.html

    キーボード+ハブ+zero(wifi, bluetooth on) -> 0.2A
    zero(wifi, bluetooth on) -> 0.1A
    zero(wifi, bluetooth on) 処理中 -> 0.2A
    カメラ稼動時、静止画 -> 0.2A、動画 -> 0.3A
    https://www.usagi1975.com/052520180823/



# ESP8266(上海) 3.3V
- 水温システムで使ってるやつ。外部ストレージなし。Pin20。フラッシュ2MB
- WiFiのみ。外部ストレージなし。
- OSなし | FreeRTOS
- VS CODE + PlatformIO(Arduino|ESP-IDF) + CやC++ | Arduino IDE + CやC++ | ESP-IDF + C++
- 本体は￥500くらいだけど、使えるようににいろいろそろえると結構高くつくんじゃないかしら？
All In Oneの開発ボードはスイッチサイエンスのやつは￥2,000くらい。秋月のは￥1,200くらいだけど
PG書き込むのにボタン操作要。ちなみに秋月の買ったけどそっこー壊れた。中華も結構あるけど試してない。
- SPIと、UARTだったかとI2Cだったかのピンが被るのでSDカードつけること考えると、現センサ類見直しいるかも。


消費電力
---
    スタートから20m秒ぐらいしたところで最大で350mAほどの電流が流れています
    Wi-Fi接続からデーター測定、送信までは73～74mA
    Deep sleepに入ってからは0.4～0.5mA程度の消費電流です。

    アクティブ時｜Deep Sleep
    ------------＋----------
    73～74mA    ｜0.4～0.5mA

    https://ambidata.io/blog/2017/06/05/esp8266-current/



# ESP32(上海) 3.3V

- ESP8266の上位版？最新版？。デュアルコア。外部ストレージなし。Pin40。フラッシュ4MB
- WiFi, BLE。外部ストレージなし。Pin40。
- OSなし | FreeRTOS
- VS CODE + PlatformIO(Arduino|ESP-IDF) + C? C++? | Arduino IDE | ESP-IDF
- 本体は\500くらいだけど、使えるようににいろいろそろえると結構高くつくんじゃないかしら？
All In Oneの開発ボードはスイッチサイエンスのやつは\2,000くらい。ただし今は在庫ないみたい。
秋月のは\1,400くらい。

消費電力
---
    ●突入電流は1A以上が２回流れる
    ●リセット電流は約600mA
    ●Wi-Fi通信時パルス電流は約600mA
    ●Wi-Fi待機時平均電流は約160mA
    https://www.mgo-tec.com/blog-entry-esp-wroom-32-current.html



結果
==

# ESP8266 < RaspberryPi ZW < ESP32

今のところ、Pi ZWが一番扱いやすいかな。


