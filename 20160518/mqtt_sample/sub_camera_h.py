import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO

HOST = '192.168.0.21'
PORT = 1883
TOPIC = 'mqtt/sample'

# 出力ピン
PWMOUT_PIN  =   12
# 周波数
FREQ        =   50
# 角度
DEG_MIN     =   0.0
DEG_MAX     =   180.0
# デューティ比
DC_MIN      =   2.5
DC_MAX      =   12.0

GPIO.setmode(GPIO.BOARD)
GPIO.setup(PWMOUT_PIN, GPIO.OUT)
servo = GPIO.PWM(PWMOUT_PIN, FREQ)
servo.start(0)

def _convert_dc(deg):
    """ 角度をデューティ比に換算
        in_deg 角度
    """
    return ((deg - DEG_MIN) * (DC_MAX - DC_MIN) / (DEG_MAX - DEG_MIN) + DC_MIN)

def on_connect(client, userdata, flags, respons_code):
    print('status {0}'.format(respons_code))

    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    print(msg.topic + ' ' + str(msg.payload))

    dc = _convert_dc(float(msg.payload))
    servo.ChangeDutyCycle(dc)

def on_unsubscribe(client, userdata, rc):
    print('unsubscribe')

    servo.stop()
    GPIO.cleanup()
   

if __name__ == '__main__':

    # Publisherと同様に v3.1.1を利用
    client = mqtt.Client(protocol=mqtt.MQTTv311)

    client.on_connect = on_connect
    client.on_message = on_message
    client.on_unsubscribe = on_unsubscribe

    client.connect(HOST, port=PORT, keepalive=60)

    # 待ち受け状態にする
    client.loop_forever()

