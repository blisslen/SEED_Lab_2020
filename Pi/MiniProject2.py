# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import cv2.aruco as aruco
import numpy as np
from math import floor
from datetime import datetime as dt
import smbus
import busio
import board
import adafruit_character_lcd.character_lcd_rgb_i2c as character_lcd

#function to write a value to a certain address 
def writeNumber(value):     
	bus.write_byte(myAddr, value)
	# bus.write_byte_data(address, 0, value)     
	return -1

#function to write a value to a certain address 
def writeNumber(value):     
	bus.write_byte(myAddr, value) 
	# bus.write_byte_data(address, 0, value)     
	return -1 

def takePic(saveFile, fileLoc):
    camera = PiCamera();
    rawCapture = PiRGBArray(camera);
    time.sleep(0.1);
    try:
        camera.capture(rawCapture, format="bgr");
        image = rawCapture.array;
        camera.close();
    except:
        print("Failed to capture");
    if(saveFile):
        print("Saving image "+fileLoc);
        try:
            cv2.imwrite(fileLoc, image);
        except:
            print("Failed to save.");
    return image



def getCenters(corners):
    centers = [];
    if(not corners is None):
      for corner in corners[0]:
          xSum = 0;
          ySum = 0;
          for point in corner:
              xSum = xSum + point[0];
              ySum = ySum + point[1];
          centers.append((xSum / 4, ySum / 4));
      return centers;
    return centers;
        

"""

DEBUG PARAMETERS

"""

writePicsToFile = False;
displayPics = False;
showMarkersInDisplay = False;
printoutMarkers = True;

myAddr = 0x45;

# Modify this if you have a different sized Character LCD
lcd_columns = 16
lcd_rows = 2

# Initialise I2C bus.
i2c = busio.I2C(board.SCL, board.SDA)

# Initialise the LCD class
lcd = character_lcd.Character_LCD_RGB_I2C(i2c, lcd_columns, lcd_rows)

# Print two line message right to left
lcd.text_direction = lcd.RIGHT_TO_LEFT

"""

MAIN LOGIC LOOP

"""
aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250);
parameters = aruco.DetectorParameters_create();
while True:
    timestamp = dt.now();
    pic = takePic(writePicsToFile,"Saved/Saved_Photo"+str(timestamp)+".jpg");
    #img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY);
    #Probably don't need to greyscale it, idk why tf they made us in the assignment
    
    corners, ids, rejectedImgPoints = aruco.detectMarkers(pic, aruco_dict, parameters = parameters);
    centers = getCenters(corners);
    if(not ids is None):
    
    try:
        pass;
        print(corners[0].shape)
        #print(corners.shape);
    except:
        pass;
    if(showMarkersInDisplay):
        pic = aruco.drawDetectedMarkers(pic, rejectedImgPoints, borderColor=(100,0,240));
        pic = aruco.drawDetectedMarkers(pic, corners);
    
    if(displayPics):
        cv2.imshow("Live feed owo",pic);
        if(cv2.waitKey(1) > 0):
            break;
    if(printoutMarkers):
        #try:
        if(not ids is None):
            print(centers);
            print("Detected markers with ID's : ",ids[:,0]);
        #except:
            #print("No IDs detected.");
    (w,h) = pic.shape();
    if(centers[0][0] < w/2 and centers[0][1] < h/2):
        #TOP LEFT? IDK, near 0,0, which could be top or bottom left depending on their convention
        #set to 0 rads
        pass;
    elif(centers[0][0] < w/2 and centers[0][1] > h/2):
        #BOTTOM LEFT
        #set to pi/2 rads
        pass;
    elif(centers[0][0] > w/2 and centers[0][1] < h/2):
        #TOP RIGHT
        #set to pi rads
        pass;
    elif(centers[0][0] > w/2 and centers[0][1] > h/2):
        #BOTTOM RIGHT
        #set to 3pi/2 rads
        pass;
    #(ABOVE) SEND SETPOINT TO ARDUINO
    
    pos = 69;
    #SEND REQUEST TO ARDUINO FOR POS
    
    lcd.clear();
    lcd.message = "Current Position: " + str(pos);
    #SET LCD DISPLAY TO SHOW POS
        
cv2.destroyAllWindows();
