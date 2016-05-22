import paho.mqtt.client as mqtt

HOST = '192.168.0.21'
PORT = 1883
TOPIC = 'mqtt/sample'

def on_connect(client, userdata, flags, respons_code):
    print('status {0}'.format(respons_code))

    client.subscribe(TOPIC)

def on_message(client, userdata, msg):
    print(msg.topic + ' ' + str(msg.payload))

if __name__ == '__main__':

    # Publisherと同様に v3.1.1を利用
    client = mqtt.Client(protocol=mqtt.MQTTv311)

    client.on_connect = on_connect
    client.on_message = on_message

    client.connect(HOST, port=PORT, keepalive=60)

    # 待ち受け状態にする
    client.loop_forever()
