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
import adafruit_character_lcd.character_lcd_rgb_i2c
import busio
import board

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
        
cv2.destroyAllWindows();