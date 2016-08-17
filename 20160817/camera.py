import picamera
import datetime
import os
 
now = datetime.datetime.now()
dir_name = now.strftime('%Y%m%d')
dir_path = '/home/pi/'+dir_name
file_name = now.strftime('%H%M%S')
  
if not os.path.exists(dir_path):
    os.makedirs(dir_path)
    os.chmod(dir_path, 0o777)
       
picamera = picamera.PiCamera()
picamera.capture(dir_path+'/'+file_name+'.jpg')
