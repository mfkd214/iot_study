#-----------------------------------------------------------------------
# mosquitto.py
# DS18B20
#   https://github.com/timofurrer/w1thermsensor
#   http://qiita.com/masato/items/cf5a27af696a27b73b86
#-----------------------------------------------------------------------
from datetime import *
import time
import paho.mqtt.client as mqtt
from w1thermsensor import W1ThermSensor

ANALOG_PIN  =   0
JST         =   timezone(timedelta(hours=+9), 'JST')
HOST        = '192.168.0.21'
PORT        = 1883
TOPIC       = 'mqtt/sample'

# インスタンス作成時に protocol v3.1.1 を指定します
client = mqtt.Client(protocol=mqtt.MQTTv311)
client.connect(HOST, port=PORT, keepalive=60)

# DS18B20
sensor      =   W1ThermSensor()
try:
    while True:
        ds18b10_val     =   0.0

        # DS18B20 ------------------------------------------------------
        ds18b10_val     =   sensor.get_temperature()
        print("DS18B10:{0:.3f}".format(ds18b10_val))

        # MQTT publish -------------------------------------------------
        client.publish(TOPIC, str(ds18b10_val))
        time.sleep(0.5)

finally:
    client.disconnect()

