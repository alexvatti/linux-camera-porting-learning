
#include <QCoreApplication>

#include<stdio.h>
#include<stdarg.h>
#include<string.h>
#include<linux/types.h>
#include<fcntl.h>
#include<linux/videodev2.h>
#include<linux/stat.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "linux/videodev2.h"
#include "v4l-helpers.h"

#include "opencv2/opencv.hpp"

#define REQ_BUFF_SZ     5


struct v4l_fd cam1;
char cam_devpath[] = "/dev/video0";
int realfd = 0;

int IMAGE_W=0;
int IMAGE_H=0;

//open function verified
int open_device()
{
    int real_fd;
    v4l_fd_init( &cam1 );
    real_fd = v4l_open( &cam1, cam_devpath, true );
    if ( real_fd < 0 )
    {
        printf( "Your %s cannot be open.\n", cam_devpath );
    }
    realfd = real_fd;
    return real_fd;
}

//device capabilities verified
int device_capabilities()
{
    //struct v4l2_format vfmt = {0};
    unsigned int captype = v4l_determine_type( &cam1 );

    if ( ( captype & V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE ) == 0 )
    {
        printf( "WARNING: this camera not supports Multiplane.\n" );
    }
    return 0;
}

//get format verified
int get_format()
{
    struct v4l2_format vfmt = {0};
    unsigned int captype = v4l_determine_type( &cam1 );
    int ret = 0;
    ret = v4l_g_fmt(&cam1,&vfmt,captype);
    unsigned pixelfmt = v4l_format_g_pixelformat( &vfmt );
    unsigned planes   = v4l_format_g_num_planes( &vfmt );
    IMAGE_W  = v4l_format_g_width( &vfmt );
    IMAGE_H = v4l_format_g_height( &vfmt );

    printf("width=%d\n",IMAGE_W);
    printf("height=%d\n",IMAGE_H);
    printf("planes=%d\n",planes);

    printf("Video format: %c%c%c%c (%08x) %ux%u\n",
        (pixelfmt >> 0) & 0xff,
        (pixelfmt >> 8) & 0xff,
        (pixelfmt >> 16) & 0xff,
        (pixelfmt >> 24) & 0xff,
        pixelfmt, IMAGE_W, IMAGE_H);
    return ret;
}

int request_query_buffers()
{
    int ret = 0;
    unsigned memtype = V4L2_MEMORY_MMAP;
    unsigned captype = v4l_determine_type( &cam1 );
    struct v4l_buffer vbuffer;
    struct v4l_queue  vqueue;

    v4l_buffer_init( &vbuffer, captype, memtype, 0 );
    ret = v4l_buffer_prepare_buf( &cam1, &vbuffer );
    if ( ret < 0 )
        printf( "v4l_buffer_prepare_buf()" );

    v4l_queue_init( &vqueue, captype, memtype );

    ret = v4l_queue_querybufs( &cam1, &vqueue, 0 );
    if ( ret < 0 )
        printf( "v4l_queue_querybufs()" );

    ret = v4l_queue_reqbufs( &cam1, &vqueue, REQ_BUFF_SZ );
    if ( ret < 0 )
        printf( "v4l_queue_reqbufs()" );

    ret = v4l_queue_mmap_bufs( &cam1, &vqueue, 0 );
    if ( ret < 0 )
        printf( "v4l_queue_mmap_bufs()" );
    return ret;
}

//verified
int stream_on()
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    int ret = v4l_ioctl( &cam1, VIDIOC_STREAMON, &type );
    if ( ret < 0 )
        printf( "v4l_ioctrl(VIDIOC_STREAMON)" );
    return  ret;
}

//verified
int stream_off()
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    int ret = v4l_ioctl(&cam1, VIDIOC_STREAMOFF, &type);
    if ( ret < 0 )
        printf( "v4l_ioctrl(VIDIOC_STREAMOFF)" );
    return ret;
}

cv::Mat BA12_to_RGB24(unsigned char *ba12)
{
    //https://answers.opencv.org/question/20748/convert-bayerbg12packed-12-bit-packed-into-rgb/
    cv::Mat Mat16Bit(IMAGE_H, IMAGE_W, CV_16UC1, ba12);
    cv::Mat Mat8Bit = Mat16Bit.clone();
    Mat16Bit.convertTo(Mat8Bit, CV_8UC3, 1.0/256);
    cv::Mat MatRgb(IMAGE_W, IMAGE_H, CV_8UC3);
    //if (is_ov5648)
    if (true)
    {
        cv::cvtColor(Mat8Bit, MatRgb, cv::COLOR_BayerBG2RGB);   //ov5648
    }
    else
    {
        cv::cvtColor(Mat8Bit, MatRgb, cv::COLOR_BayerGR2RGB);   //ar0144
    }
    return MatRgb;
}


int get_frame()
{
    fd_set fds;
    struct timeval tv;
    int ret = 0;
    struct v4l_buffer vbuffer;
    struct v4l_queue  vqueue;

    FD_ZERO( &fds );
    FD_SET( realfd, &fds );

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    ret = select( realfd + 1, &fds, NULL, NULL, &tv );
    if ( ret <= 0 )
        printf( "wait sync failure.\n" );

    ret = v4l_buffer_dqbuf( &cam1, &vbuffer );
    if ( ret < 0 )
            printf( "v4l_buffer_dqbuf()" );

    unsigned bufflen = 0;
    printf("bufflen = %u \n",bufflen);
    bufflen = v4l_queue_g_length( &vqueue, 0 );
    printf("bufflen = %u \n",bufflen);
    if ( bufflen > 0 )
    {
        printf("ptrData > 0\n");
        size_t bidx = 0;
        void *ptrData = v4l_queue_g_mmapping( &vqueue, bidx, 0 );
        if ( ptrData != NULL )
        {
            printf("ptrData != NULL\n");
            cv::Mat mat = BA12_to_RGB24((unsigned char *)ptrData);
            cv::imwrite("./sample.jpg",mat);
            // ptrData is RAW pixel array from camera or ISP.
        }
    }

    ret = v4l_buffer_qbuf( &cam1, &vbuffer );
    if ( ret < 0 )
        printf( "v4l_buffer_qbuf()" );
    return ret;
}

int close_device()
{
    return v4l_close(&cam1);
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int ret;
    ret = open_device();
    if(ret < 0) {
        printf("Device open failed\n");
        return -1;
    }
    ret = device_capabilities();
    if(ret < 0) {
        printf("Device does not support video capture\n");
    }
    get_format();
    printf ("get_format\n");
    request_query_buffers();
    printf ("query_buffers\n");
    stream_on();
    printf ("stream_on\n");
    get_frame();
    printf ("get_frame\n");
    stream_off();
    printf ("stream_off\n");
    //clear_buffers();
    //printf ("clear_buffers\n");
    close_device();
    printf ("close_device\n");
    return 0;
    return a.exec();
}
