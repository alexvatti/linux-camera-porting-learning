#!/usr/bin/env python

'''
Sample show VideoCapture via v4l2 interface for PHYTEC phyCAM-M series.
Keys:
    q    - exit
'''

# Python 2/3 compatibility
import numpy as np
import cv2
import subprocess
import sys
import os
import time

prev_frame_time = 0
new_frame_time = 0
fps_average_count = 0
fps = 0

font                = cv2.FONT_HERSHEY_SIMPLEX
TopLeftCornerOfText = (10,30)
fontScale           = 1
fontColor           = (255,255,255)
lineType            = 2

print ("OpenCV version :  {0}".format(cv2.__version__))
face_cascade = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
eye_cascade = cv2.CascadeClassifier('haarcascade_eye.xml')


print ("")
print ("choose you camera")
print ("=================")
print ("1 = VM-016-BW,  Resolution (1280 x 720) ROI")
print ("2 = VM-016-COL, Resolution (1280 x 720) ROI")
print ("3 = VM-017-BW,  Resolution (1280 x 720) ROI")
print ("4 = VM-017-COL, Resolution (1280 x 720) ROI")

camera_version = input("Your select = ")

# for other resolutions change the "cam_width", "cam_height" and "cam_offset" parameter
if camera_version == "1":
   cam_width = 1280; cam_height = 720; cam_offset = "0,44"
   cam_typ = "'ar0144 2-0010'"; grab_res = str(cam_width)+"x"+str(cam_height); cam_col_fmt = "Y8"; grab_res_array = grab_res
   v4l2_ctl_param = "auto_exposure_max=720,dynamic_defect_pixel_correction=1"
elif camera_version == "2":
   cam_width = 1280; cam_height = 720; cam_offset = "0,44"
   cam_typ = "'ar0144 2-0010'"; grab_res = str(cam_width)+"x"+str(cam_height); cam_col_fmt = "SGRBG8_1X8"; grab_res_array = grab_res
   v4l2_ctl_param = "auto_exposure_max=720,dynamic_defect_pixel_correction=0"
elif camera_version == "3":
   cam_width = 1280; cam_height = 720; cam_offset = "16,252"
   cam_typ = "'ar052x 2-0036'"; grab_res = str(cam_width)+"x"+str(cam_height); grab_res_array = str(cam_width*2)+"x"+str(cam_height*2); cam_col_fmt = "Y8"
   v4l2_ctl_param = "exposure=2240,analogue_gain=4000,dynamic_defect_pixel_correction=1"
elif camera_version == "4":
   cam_width = 1280; cam_height = 720; cam_offset = "16,252"
   cam_typ = "'ar052x 2-0036'"; grab_res = str(cam_width)+"x"+str(cam_height); grab_res_array = str(cam_width*2)+"x"+str(cam_height*2); cam_col_fmt = "SGRBG8_1X8"
   v4l2_ctl_param = "exposure=2240,analogue_gain=5000,digital_gain_red=1400,digital_gain_blue=1400,dynamic_defect_pixel_correction=1"
else:
   print ("wrong number")
   sys.exit(0)

print ("")
print ("grab_res =",grab_res)
print ("grab_res_array =",grab_res_array)

# set display
os.environ["DISPLAY"] = ':0.0'


# read /dev/video[x] camera at CSI1/CAM1
ret = subprocess.run(["media-ctl", "-e", "mxc_isi.0.capture"], stdout=subprocess.PIPE)
CSI1_DEVICE = ret.stdout.decode()					# .decode -> convert returnvalue from byte in string
CSI1_DEVICE = CSI1_DEVICE[:-1]						# cut the "\n" at the end
CSI1_DEVICE_NUMBER = int(CSI1_DEVICE[10:].strip())	# .strip() -> cut the first 10 characters
print ("")
print (" camera device for CSI1 port = ", CSI1_DEVICE)
print (" camera device_number for CSI1 port = ", CSI1_DEVICE_NUMBER)


# set video path, camera resolution and ROI position via media-ctl
print ("")
print (" configure CSI1_DEVICE (camera_1 port) with media_ctl")
print (" Note: For use other resolutions,")
print (" see the scripts in path ...\\gstreamer-examples\\...")
print (" ===============================================================")
subprocess.run(["media-ctl", "-r"])
subprocess.run(["media-ctl", "-l", cam_typ+":0->'mxc-mipi-csi2.0':0[1]"])
subprocess.run(["media-ctl", "-l", "'mxc-mipi-csi2.0':4->'mxc_isi.0':0[1]"])
subprocess.run(["media-ctl", "-l", "'mxc_isi.0':12->'mxc_isi.0.capture':0[1]"])

subprocess.run(["media-ctl", "-V", cam_typ+":0 [fmt:"+cam_col_fmt+"/"+grab_res+" ("+cam_offset+")/"+grab_res_array+"]"])
subprocess.run(["media-ctl", "-V", "'mxc-mipi-csi2.0':4 [fmt:"+cam_col_fmt+"/"+grab_res+" field:none]"])
subprocess.run(["media-ctl", "-V", "'mxc_isi.0':12 [fmt:"+cam_col_fmt+"/"+grab_res+" field:none]"])


# set camera parameters via v4l2-ctl
subprocess.run(["v4l2-ctl", "-d", str(CSI1_DEVICE), "-c", str(v4l2_ctl_param)])	# set v4l2 controls 


# open videodevice via GStreamer
cap = cv2.VideoCapture("v4l2src device="+str(CSI1_DEVICE)+" ! appsink", cv2.CAP_GSTREAMER)
# open videodevice via GStreamer with bayer preprocessing, Note: only for col-cam "cv2.COLOR_BAYER_GB2BGR" is not neccesary
#cap = cv2.VideoCapture("v4l2src device=/dev/video0 ! video/x-bayer, format=grbg, width="+str(cam_width)+", height="+str(cam_height)+" ! bayer2rgbneon ! videoconvert ! appsink", cv2.CAP_GSTREAMER)
# open videodevice via V4l Interface
#cap = cv2.VideoCapture(int(CSI1_DEVICE_NUMBER), cv2.CAP_V4L)

if not cap.isOpened():
    print(" Cannot open camera")
    exit()


# set fullscreen
cv2.namedWindow('phyCAM - Face detection', cv2.WND_PROP_FULLSCREEN)
cv2.setWindowProperty('phyCAM - Face detection',cv2.WND_PROP_FULLSCREEN,cv2.WINDOW_FULLSCREEN)


while(True):
    ret, frame = cap.read()

    if cam_col_fmt == "SGRBG8_1X8":
       frame = cv2.cvtColor(frame, cv2.COLOR_BAYER_GB2BGR)

    if fps_average_count == 5:
       new_frame_time = time.time()
       fps = 5/(new_frame_time-prev_frame_time)
       prev_frame_time = new_frame_time 
       print ("fps = %3.1f"% fps, end='\r')
       fps_average_count = 0
    fps_average_count += 1
    cv2.putText(frame, "FPS: {:03.1f}".format(fps), TopLeftCornerOfText, font, fontScale, fontColor, lineType)


    if cam_col_fmt == "SGRBG8_1X8":
       gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    else:
       gray = frame

    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.3, minNeighbors=3)
    for (x,y,w,h) in faces:
        frame = cv2.rectangle(frame,(x,y),(x+w,y+h),(255,0,0),2)
        roi_gray = gray[y:y+h, x:x+w]
        roi_color = frame[y:y+h, x:x+w]
#        eyes = eye_cascade.detectMultiScale(roi_gray)
#        for (ex,ey,ew,eh) in eyes:
#            cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(0,255,0),2)


    # Draw a rectangle around the faces
    for (x, y, w, h) in faces:
        cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 2)

    # Write Number of Faces
    cv2.putText(frame,'/ Number of Faces : ' + str(len(faces)),(180, 30), font, fontScale, fontColor, lineType)     

    cv2.imshow('phyCAM - Face detection',frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
 
cap.release()
cv2.destroyAllWindows()