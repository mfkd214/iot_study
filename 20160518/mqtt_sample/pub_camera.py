import time
import paho.mqtt.client as mqtt

HOST        = '192.168.0.21'
PORT        = 1883
TOPIC       = 'mqtt/sample'

# インスタンス作成時に protocol v3.1.1 を指定します
client = mqtt.Client(protocol=mqtt.MQTTv311)
client.connect(HOST, port=PORT, keepalive=60)

try:
    while True:
        for ii in range(1, 181, 1):
            client.publish(TOPIC, str(ii))
            time.sleep(0.1)

        for ii in range(180, -1, -1):
            client.publish(TOPIC, str(ii))
            time.sleep(0.1)
finally:
    client.disconnect()

