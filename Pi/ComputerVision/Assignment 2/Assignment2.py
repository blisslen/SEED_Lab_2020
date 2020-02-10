# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2
import cv2.aruco as aruco
import numpy as np
from math import floor

def takePic(cameraWorks, saveFile, fileLoc):
  if(cameraWorks):
    camera = PiCamera();
    rawCapture = PiRGBArray(camera);
    time.sleep(0.1);
    print("Capturing Image...")
    try:
      try:
        #rawCapture.truncate(0);
          pass;
      except:
        pass;
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
  else:
    print("Reading cached image...");
    #try:
    image = cv2.imread(fileLoc, cv2.IMREAD_UNCHANGED);
    #except:
      #print("Failed to read file.");
    return image;

clickPoint = [];
pressed = False;

def clickCap(event, x, y, flags, params):
  global clickPoint;
  global pressed;
  if event == cv2.EVENT_LBUTTONDOWN:
    clickPoint = (x,y);
    pressed = True;

def part1(cameraWorks):
  #print("You just called function 1!");
  img = takePic(cameraWorks, True, "Live/shrek.png");
  print("Original dimensions : ",img.shape);
  scale_percent = 50 # percent of original size
  width = int(img.shape[1] * scale_percent / 100);
  height = int(img.shape[0] * scale_percent / 100);
  dim = (width, height);
  scaled = cv2.resize(img,dim, interpolation=cv2.INTER_AREA);
  print("Resized dimensions : ",scaled.shape);
  cv2.imshow("Resized", scaled);
  cv2.waitKey(60000);
  cv2.destroyAllWindows();
  return scaled;

def part2(cameraWorks):
  #print("You just called function 2!");
  img = takePic(cameraWorks, False, "Cached/shrek.png");
  ref = img;
  while True:
    useBGR = input("Would you like to use BGR or HSV colors?");
    if(useBGR == "bgr" or useBGR == "BGR"):
      break;
    elif(useBGR == "HSV" or useBGR == "hsv"):
      ref = cv2.cvtColor(img,cv2.COLOR_BGR2HSV);
      break;
    else:
      print("Invalid input.");
  cv2.namedWindow("image");
  cv2.setMouseCallback("image", clickCap); 
  print("click on a pixel of the image shown to see its color values.");
  while True:
    cv2.imshow("image",img);
    cv2.waitKey(1);
    if(pressed):
      print(ref[clickPoint[1],clickPoint[0]]);
      break;
  cv2.destroyAllWindows();
  return;
    
  
def part3(cameraWorks):
  img = takePic(cameraWorks, False, "Cached/shrek.png");
  newColor = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY);
  cv2.imshow("Grayscale", newColor);
  cv2.waitKey(60000);
  cv2.destroyAllWindows();
  return;
  
def part4(cameraWorks):
  img = takePic(cameraWorks, False, "Cached/yellow_slide2.png");
  if(not cameraWorks):
    scale_percent = 20 # percent of original size
    width = int(img.shape[1] * scale_percent / 100);
    height = int(img.shape[0] * scale_percent / 100);
    dim = (width, height);
    img = cv2.resize(img,dim, interpolation=cv2.INTER_AREA);
  
  yellow = np.uint8([[[0,255,255]]]);
  hsv_yellow = cv2.cvtColor(yellow,cv2.COLOR_BGR2HSV);
  hsv_yellow_hue = hsv_yellow[0,0,0];
  boundaries = [([hsv_yellow_hue - 15,100,100],[hsv_yellow_hue + 15,255,255])];
  hsv_img = cv2.cvtColor(img,cv2.COLOR_BGR2HSV);
  for (lower, upper) in boundaries:
    lower = np.array(lower, dtype = "uint8");
    upper = np.array(upper, dtype = "uint8");
    mask = cv2.inRange(hsv_img, lower, upper);
    output = cv2.bitwise_and(img, img, mask = mask);
    cv2.imshow("Only Yellow", np.hstack([img, output]));
    cv2.waitKey(60000);
    cv2.destroyAllWindows();
  return;
  
def part5(cameraWorks):
  print("Testing image for markers...");
  img = takePic(cameraWorks, False, "Cached/Both.jpg");
  if(not cameraWorks):
    scale_percent = 20 # percent of original size
    width = int(img.shape[1] * scale_percent / 100);
    height = int(img.shape[0] * scale_percent / 100);
    dim = (width, height);
    img = cv2.resize(img,dim, interpolation=cv2.INTER_AREA);
    
  img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY);
  aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250);
  parameters = aruco.DetectorParameters_create();
  corners, ids, rejectedImgPoints = aruco.detectMarkers(img, aruco_dict, parameters = parameters);
  print(corners);
  img = aruco.drawDetectedMarkers(img, rejectedImgPoints, borderColor=(100,0,240));
  img = aruco.drawDetectedMarkers(img, corners);
  cv2.imshow("Markers detected",img);
  try:
        print("Detected markers with ID's : ",ids[:,0]);
  except:
        print("No IDs detected.");
  cv2.waitKey(60000);
  cv2.destroyAllWindows();
  return;

  
  
def part6(cameraWorks):
  print("Testing feed for markers...");
  print("Press any key to stop.");
  frame = 0;
  files = ["Cached/Just1.jpg","Cached/Just2.jpg","Cached/Both.jpg"];
  aruco_dict = aruco.Dictionary_get(aruco.DICT_6X6_250);
  parameters = aruco.DetectorParameters_create();
  while True:
    img = takePic(cameraWorks, False, files[floor(frame / 5.0) % 3]);
    if(not cameraWorks):
      scale_percent = 20 # percent of original size
      width = int(img.shape[1] * scale_percent / 100);
      height = int(img.shape[0] * scale_percent / 100);
      dim = (width, height);
      img = cv2.resize(img,dim, interpolation=cv2.INTER_AREA);
      frame = frame + 1;
      
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY);
    corners, ids, rejectedImgPoints = aruco.detectMarkers(img, aruco_dict, parameters = parameters);
    try:
      pass;
      print(corners[0].shape)
      #print(corners.shape);
    except:
      pass;
    #img = aruco.drawDetectedMarkers(img, rejectedImgPoints, borderColor=(100,0,240));
    
    #img = aruco.drawDetectedMarkers(img, corners);
    #cv2.imshow("Markers detected",img);
    try:
        print(ids.shape);
        print("Detected markers with ID's : ",ids[:,0]);
    except:
        print("No IDs detected.");
    cv2.imshow("current img",img);
    if(cv2.waitKey(1) > 0):
      break;
  cv2.destroyAllWindows();
  return;

funcDict = {"1":part1, "2":part2, "3":part3, "4":part4, "5":part5, "6":part6};

while True:
  userInp = input("Please enter the part of this assignment you'd like to demo. (1-6), or exit to exit.");
  if userInp == "exit":
    break;
  elif(funcDict.get(userInp) != None):
    cameraWorks = False;
    while True:
      cameraState = input("Does your camera work yet? (y/n)");
      if(cameraState == "y" or cameraState == "Y"):
        cameraWorks = True;
        break;
      elif(cameraState == "n" or cameraState == "N"):
        cameraWorks = False;
        break;
      else:
        printf("Invalid input. Try again.");
    funcDict[userInp](cameraWorks);
  else:
    print("Not a valid input. Try again.");