to capture raw YUV image ov5648 camera sensor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v4l2-ctl --set-fmt-video=width=1280,height=720,pixelformat=YUYV --stream-mmap --stream-count=1 --device /dev/video1 --stream-to=/root/ov5648-1.raw


to capture raw YUV image ar0144 camera sensor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
v4l2-ctl --set-fmt-video=width=1280,height=800,pixelformat=YUYV --stream-mmap --stream-count=1 --device /dev/video1 --stream-to=/root/ar0144-1.raw