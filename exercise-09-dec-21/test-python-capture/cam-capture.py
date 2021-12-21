# Copyright (c) 2020 PHYTEC Messtechnik GmbH
# SPDX-License-Identifier: Apache-2.0

import os
import cv2
import numpy as np
import time
import subprocess
import fcntl
import ctypes

def get_camera():
    videodev = 'video0'
    buildinfo = cv2.getBuildInformation()

    if buildinfo.find('GStreamer') < 0:
        print('no GStreamer support in OpenCV')
        exit(0)

    failed = False
    # Make sure the camera is in a defined state
    cmd = 'media-ctl -V "63:0[fmt:SBGGR8_1X8/2592x1944 field:none]"'
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print('media-ctl failed: {}'.format(ret))
        failed = True
    
    cmd = 'media-ctl -V "43:0[fmt:SBGGR8_1X8/2592x1944 field:none]"'
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print('media-ctl failed: {}'.format(ret))
        failed = True
    
    cmd = 'media-ctl -V "43:4[fmt:SBGGR8_1X8/2592x1944 field:none]"'
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print('media-ctl failed: {}'.format(ret))
        failed = True

    failed = False
    cmd = 'v4l2-ctl -d0 -v width=2592,height=1944,pixelformat=BA81'
    ret = subprocess.call(cmd, shell=True)
    if ret != 0:
        print('v4l2-ctl failed: {}'.format(ret))
        failed = True

    
#    pipeline = 'v4l2src device=/dev/{video} ! appsink'.format(video=videodev)
#    return cv2.VideoCapture(pipeline, cv2.CAP_GSTREAMER)
    return cv2.VideoCapture(0)


def color_convert(frame):
    return cv2.cvtColor(frame, cv2.COLOR_BAYER_BG2BGR)


if __name__ == '__main__':
    vid = get_camera()
    count = 1
    while count < 10:
        ret, frame = vid.read()
        if ret == 0:
            print('No Frame..')
            continue
        print("frame count",count)
        count += 1

    vid.release()
    
        

