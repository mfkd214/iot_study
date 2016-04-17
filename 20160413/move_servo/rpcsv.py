from xmlrpc.server import SimpleXMLRPCServer
import RPi.GPIO as GPIO
import time
import sys

SERVER  =   "192.168.0.23"
PORT    =   8000

class SERVOFunc():

    def move(self):
        PWMOUT = 12

        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(PWMOUT, GPIO.OUT)

        servo = GPIO.PWM(PWMOUT, 50)
        servo.start(0.0)

        dc = 0.0
        for ii in range(5):

            for dc in range(2, 14, 1):
                servo.ChangeDutyCycle(dc)
                time.sleep(0.1)

            for dc in range(14, 2, -1):
                servo.ChangeDutyCycle(dc)
                time.sleep(0.1)

        servo.stop()
        GPIO.cleanup()

if __name__ == "__main__":
    server = SimpleXMLRPCServer((SERVER, PORT), allow_none=True)
    server.register_instance(SERVOFunc())
    try:
        server.serve_forever()
    finally:
        server.server_close()
        sys.exit(0)
