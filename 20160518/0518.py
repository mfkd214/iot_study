import serial
import time
import RPi.GPIO as GPIO
import sys

def main():
        GPIO.setmode(GPIO.BCM)
        LED1 = 5
        GPIO.setup(LED1,GPIO.OUT)

        con=serial.Serial('/dev/ttyAMA0', 9600, timeout=10)
        print con.portstr
        test = 0
        while test != 9:
#               strs=con.readline()
#               print strs
#               con.write(strs)
                test = con.read()
                print test
                if test == '1':
                        print "LED ON"
                        GPIO.output(LED1,GPIO.HIGH)
                elif test == '2':
                        print "LED OFF"
                        GPIO.output(LED1,GPIO.LOW)
                elif test == '9':
                        print "END"
                        GPIO.cleanup()
                        sys.exit()
                        
if __name__ == '__main__':
        main()