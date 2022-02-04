#!/bin/sh

if [ -r capture_H.264.mkv ]
	then
	echo ""
	echo " play capture_H.264.mkv with gstreamer, break with ctl-C"
	echo " ======================================================="
	gst-launch-1.0 \
		filesrc location="capture_H.264.mkv" ! \
		matroskademux ! h264parse ! vpudec ! \
		queue ! waylandsink sync=true
	else
	echo ""
	echo " capture_H.264.mkv not doesn't, please save it with xxcam_save_h2=6x_stream.sh"
	echo " ============================================================================="
fi

if [ -r "capture_H.265(HEVC).mkv" ]
	then
	echo ""
	echo " play capture_H.265(HEVC).mkv with gstreamer, break with ctl-C"
	echo " ============================================================="
	gst-launch-1.0 \
		filesrc location="capture_H.265(HEVC).mkv" ! \
		matroskademux ! h265parse ! vpudec ! \
		queue ! waylandsink sync=true
	else
	echo ""
	echo " capture_H.265(HEVC).mkv doesn't exist, please save it with xxcam_save_h26x_stream.sh"
	echo " ===================================================================================="
fi

