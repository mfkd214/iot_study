# -*- coding: utf-8 -*-
import time
import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

HOST    =   setting.HOST
PORT    =   setting.PORT
UID     =   setting.UID
PWD     =   setting.PWD
TOPIC   =   UID + '/#'

def _convert_dc(deg):
    """ 角度をデューティ比に換算
        in_deg 角度
    """
    # 角度
    DEG_MIN     =   0.0
    DEG_MAX     =   180.0
    # デューティ比
    DC_MIN      =   2.5
    DC_MAX      =   12.0

    return ((deg - DEG_MIN) * (DC_MAX - DC_MIN) / (DEG_MAX - DEG_MIN) + DC_MIN)

def on_connect(client, userdata, flags, respons_code):
    print('status {0}'.format(respons_code))

    client.subscribe(TOPIC)

def on_subscribe(mqttc, obj, mid, granted_qos):
    print("Subscribed: "+str(mid)+" "+str(granted_qos))

def on_message(client, userdata, msg):
    """ topic/(v|h)/(deg)
        (v|h)   v...vertical, h...horizontal
        (deg)   0-180
    """
    print(msg.topic + ' ' + str(msg.payload))
    # 周波数
    FREQ        =   50

    # 引数展開
    #-------------------------------------------------------
    topi    =   msg.topic
    topi_l  =   topi.split('/')
    print(topi_l)

    # PIN決定
    #-------------------------------------------------------
    pwm_pin         =   0
    if topi_l[1]   ==  'v':
        pwm_pin     =   18
    elif topi_l[1] ==  'h':
        pwm_pin     =   16
    else:
        pwm_pin     =   0
    if pwm_pin  ==  0:
        return

    # 角度決定
    #-------------------------------------------------------
    deg             =   -1.0
    try:
        deg         =   float(topi_l[2])
        if deg > 0.0 and deg < 181.0:
            pass
        else:
            return
    except:
        return

    # サーボ移動
    #-------------------------------------------------------
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(pwm_pin, GPIO.OUT)

    servo       =   GPIO.PWM(pwm_pin, FREQ)
    servo.start(0)
    servo.ChangeDutyCycle(_convert_dc(deg))
    time.sleep(1.0)

    servo.stop()
    GPIO.cleanup()

if __name__ == '__main__':

    # インスタンス作成時に protocol v3.1.1 を指定します
    client = mqtt.Client(protocol=mqtt.MQTTv311)

    client.on_connect = on_connect
    client.on_subscribe = on_subscribe
    client.on_message = on_message

    # ユーザー名とパスワードを設定します
    client.username_pw_set(UID, PWD)
    client.connect(HOST, port=PORT, keepalive=60)

    # 待ち受け状態にする
    client.loop_forever()
