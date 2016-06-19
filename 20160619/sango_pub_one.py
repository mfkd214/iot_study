# -*- coding: utf-8 -*-
import sys
import time
import paho.mqtt.client as mqtt
import setting

UID     =   setting.UID
PWD     =   setting.PWD
HOST    =   setting.HOST
PORT    =   setting.PORT

# インスタンス作成時に protocol v3.1.1 を指定します
client = mqtt.Client(protocol=mqtt.MQTTv311)
# ユーザー名とパスワードを設定します
client.username_pw_set(UID, PWD)
client.connect(HOST, port=PORT, keepalive=60)

while True:
    # v:縦軸、h:横軸
    sys.stdout.write("(v)irtical | (h)orizontal | (q)uit = ")
    sys.stdout.flush()
    vh  = sys.stdin.readline().strip()
    if  vh == 'q':
        break

    # 角度 0-180
    sys.stdout.write("(0-180)角度 | (q)uit = ")
    sys.stdout.flush()
    deg = sys.stdin.readline().strip()
    if  deg == 'q':
        break

    topic = UID + '/%s/%s' % (vh, deg)
    print(topic)

    client.publish(topic, "message from sango_pub_one")
    time.sleep(0.5)

