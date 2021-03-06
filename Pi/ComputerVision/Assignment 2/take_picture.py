# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import cv2

if __name__ == '__main__':
 
   fileName = input("File Name:")

   # initialize the camera and grab a reference to the raw camera capture
   camera = PiCamera()
   rawCapture = PiRGBArray(camera)
 
   # allow the camera to warmup
   time.sleep(0.1)
 
   # grab an image from the camera
   print("Capturing Image...")
   try:
      camera.capture(rawCapture, format="bgr")
      image = rawCapture.array
   except:
      print("Failed to capture")
 
   # display the image on screen and wait for a keypress
   #try:
   #   cv2.imshow("Image", image)
   #except Exception, e:
   #   print e

   # save the image to the disk
   print("Saving image "+fileName)
   try:
      cv2.imwrite(fileName, image)
   except:
      print("Couldn't save "+fileName)
      pass
   
   cv2.waitKey(0)
