import pyfirmata
from datetime import *
from urllib import request

HTTP_SERVER_URL = "http://localhost:8888/lm35dz/"
ANALOG_PIN = 0
JST = timezone(timedelta(hours=+9), 'JST')

if __name__ == "__main__":
    # Creates a new board 
    board = pyfirmata.Arduino("/dev/ttyACM0")

    print("Setting up the connection to the board ...")
    it = pyfirmata.util.Iterator(board)
    it.start()

    # Start reporting for defined pins
    board.analog[ANALOG_PIN].enable_reporting()
    try:

        while(1):
               
            val = board.analog[ANALOG_PIN].read()
            if val is None:
                continue 

            out_val = val * 5.0 * 100
            now = datetime.now()

            url = HTTP_SERVER_URL + now.strftime("%Y%m%d%H%M") + "/" + str(out_val) + "/"
            print(url)
            request.urlopen(url)

            board.pass_time(10)

    finally:
        board.exit()
