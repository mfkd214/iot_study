import time
import serial
import RPi.GPIO as GPIO

# 出力ピン
PWMOUT_PIN  =   16
# 周波数
FREQ        =   50
# 角度
DEG_MIN     =   0.0
DEG_MAX     =   180.0
# デューティ比
DC_MIN      =   2.5
DC_MAX      =   12.0

def _initial():
    """ 初期処理
    """

    # Bluetooth Serial(少し待たないとダメっぽい）
    ser = serial.Serial('/dev/serial0')
    time.sleep(5)

    #Servo
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(PWMOUT_PIN, GPIO.OUT)
    servo = GPIO.PWM(PWMOUT_PIN, FREQ)
    servo.start(0)

    return ser, servo

def _convert_dc(deg):
    """ 角度をデューティ比に換算
        in_deg 角度
    """
    return ((deg - DEG_MIN) * (DC_MAX - DC_MIN) / (DEG_MAX - DEG_MIN) + DC_MIN)

def _deg_filter(deg):
    """ 角度の範囲は0-180
    """
    if deg < DEG_MIN:
        return 0.0

    if deg > DEG_MAX:
        return 180.0

    return deg

def main():
    """
    """
    bt, servo   =   _initial()

    deg         =   90.0
    servo.ChangeDutyCycle(_convert_dc(deg))
    try:
        while True:
            c = ""
            c = bt.read()

            if c == b'1':
                deg += 1
            elif c == b'2':
                deg -= 1
            else:
                continue

            print(str(deg))

            deg = _deg_filter(deg)
            servo.ChangeDutyCycle(_convert_dc(deg))
            
    finally:
        bt.close()
        servo.stop()
        GPIO.cleanup()

if __name__ == "__main__":
    main()
