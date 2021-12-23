
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

struct v4l_fd cam1;
char cam_devpath[] = "/dev/video0";
int realfd=0;
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
    realfd=real_fd;
    return real_fd;
}

//device capabilities verified
int device_capabilities()
{

    struct v4l2_format vfmt = {0};
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
    v4l_g_fmt(&cam1,&vfmt,captype);
    unsigned pixelfmt = v4l_format_g_pixelformat( &vfmt );
    unsigned planes   = v4l_format_g_num_planes( &vfmt );
    unsigned width  = v4l_format_g_width( &vfmt );
    unsigned height = v4l_format_g_height( &vfmt );


    printf("width=%d\n",width);
    printf("height=%d\n",height);
    printf("planes=%d\n",planes);

    printf("Video format: %c%c%c%c (%08x) %ux%u\n",
        (pixelfmt >> 0) & 0xff,
        (pixelfmt >> 8) & 0xff,
        (pixelfmt >> 16) & 0xff,
        (pixelfmt >> 24) & 0xff,
        pixelfmt, width, height);
}

int request_query_buffers()
{
    int ret=0;
    unsigned memtype = V4L2_MEMORY_MMAP;
    unsigned captype = v4l_determine_type( &cam1 );
    struct v4l_buffer vbuffer;
    struct v4l_queue  vqueue;

    v4l_buffer_init( &vbuffer, captype, memtype, 0 );
    ret = v4l_buffer_prepare_buf( &cam1, &vbuffer );
    if ( ret < 0 ) 
        perror( "v4l_buffer_prepare_buf()" );

    v4l_queue_init( &vqueue, captype, memtype );

    ret = v4l_queue_querybufs( &cam1, &vqueue, 0 );
    if ( ret < 0 ) 
        perror( "v4l_queue_querybufs()" );

    ret = v4l_queue_reqbufs( &cam1, &vqueue, REQ_BUFF_SZ );
    if ( ret < 0 ) 
        perror( "v4l_queue_reqbufs()" );

    ret = v4l_queue_mmap_bufs( &cam1, &vqueue, 0 );
    if ( ret < 0 ) 
        perror( "v4l_queue_mmap_bufs()" );
}

//verified
int stream_on()
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    int ret = v4l_ioctl( &cam1, VIDIOC_STREAMON, &type );
    if ( ret < 0 ) 
        perror( "v4l_ioctrl(VIDIOC_STREAMON)" );
}

//verified
int stream_off()
{
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    int ret = v4l_ioctl(&cam1, VIDIOC_STREAMOFF, &type);
    if ( ret < 0 ) 
        perror( "v4l_ioctrl(VIDIOC_STREAMOFF)" );
}

int get_frame()
{
    fd_set fds;
    struct timeval tv;
    int ret;
    struct v4l_buffer vbuffer;
    struct v4l_queue  vqueue;

    FD_ZERO( &fds );
    FD_SET( realfd, &fds );

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    ret = select( realfd + 1, &fds, NULL, NULL, &tv );
    if ( ret <= 0 ) 
        perror( "wait sync failure.\n" );

    ret = v4l_buffer_dqbuf( &cam1, &vbuffer );
    if ( ret < 0 ) 
            perror( "v4l_buffer_dqbuf()" );

    unsigned bufflen = v4l_queue_g_length( &vqueue, 0 );
    if ( bufflen > 0 )
    {
        size_t bidx = 0;
        void* ptrData = v4l_queue_g_mmapping( &vqueue, bidx, 0 );
        if ( ptrData != NULL )
        {
            // ptrData is RAW pixel array from camera or ISP.
        }
    }

    ret = v4l_buffer_qbuf( &cam1, &vbuffer );
    if ( ret < 0 ) 
        perror( "v4l_buffer_qbuf()" );
}

int close_device()
{
    int ret = v4l_close(&cam1);
}

int main()
{
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
    request_query_buffers();
    stream_on();
    get_frame();
    stream_off();
    //clear_buffers();
    close_device();
}

