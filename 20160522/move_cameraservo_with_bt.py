import time
import serial
import RPi.GPIO as GPIO
#-------------------------------------------------------------------------------
#   move_cameraservo_with_bt.py
#
#   DATE        AUTHOR      DESCRIPTION
#   ----------  ----------  ----------------------------------------------------
#   2016.05.24  mfkd        修正
#   2016.05.22  mfkd        Created
#-------------------------------------------------------------------------------

# 出力ピン
PWMOUT_PIN_H    =   16
PWMOUT_PIN_V    =   18

# 周波数
FREQ            =   50

# 角度
DEG_DEFAULT     =   90.0
DEG_MIN         =   0.0
DEG_MAX         =   180.0

# デューティ比
DC_MIN          =   2.5
DC_MAX          =   12.5

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
    """　メイン処理
    """
    
    # BT(Serial)設定
    #-------------------------------------------------------------------
    # ノンブロッキング    
    bt = serial.Serial('/dev/serial0', timeout=0.1)

    # GPIO設定
    #-------------------------------------------------------------------
    GPIO.setmode(GPIO.BOARD)
    GPIO.setup(PWMOUT_PIN_H, GPIO.OUT)
    GPIO.setup(PWMOUT_PIN_V, GPIO.OUT)

    deg_h               =   DEG_DEFAULT
    deg_v               =   DEG_DEFAULT
    try:
        while True:
            val         =   bt.read()
            if val in [b'1', b'2', b'3', b'4']:

                # サーボ START
                servo_h = GPIO.PWM(PWMOUT_PIN_H, FREQ)
                servo_v = GPIO.PWM(PWMOUT_PIN_V, FREQ)
                
                servo_h.start(_convert_dc(deg_h))
                servo_v.start(_convert_dc(deg_v))
                
                while val != b'':

                    if  (val == b'1'):
                        deg_h   =   _deg_filter(deg_h + 1)
                        print(deg_h)
                        servo_h.ChangeDutyCycle(_convert_dc(deg_h))
                    elif(val == b'2'):
                        deg_h   =   _deg_filter(deg_h - 1)
                        servo_h.ChangeDutyCycle(_convert_dc(deg_h))

                    elif(val == b'3'):
                        deg_v   =   _deg_filter(deg_v + 1)
                        servo_v.ChangeDutyCycle(_convert_dc(deg_v))

                    elif(val == b'4'):
                        deg_v   =   _deg_filter(deg_v - 1)
                        servo_v.ChangeDutyCycle(_convert_dc(deg_v))

                    else:
                        pass
                        
                    val =   bt.read()
                    
                # サーボ ENDED
                servo_h.stop()
                servo_v.stop()
    finally:
        bt.close()
        GPIO.cleanup()

if __name__ == "__main__":
    main()
