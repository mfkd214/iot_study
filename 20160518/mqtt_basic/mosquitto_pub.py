import time
import paho.mqtt.client as mqtt

host = '192.168.0.10'
port = 1883
topic = 'mqtt/sample'

# インスタンス作成時に protocol v3.1.1 を指定します
client = mqtt.Client(protocol=mqtt.MQTTv311)

client.connect(host, port=port, keepalive=60)

for ii in range(10):
    client.publish(topic, 'from mosquitto_pub' + str(ii + 1))
    time.sleep(0.1)

client.disconnect()
