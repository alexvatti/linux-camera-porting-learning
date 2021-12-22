#ifndef V4L2_H
#define V4L2_H

#ifdef TARGET_BUILD

#include <QObject>
#include <QDateTime>
#include <QDebug>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

using namespace std;

//User Controls
#define exposure                        0x00980911 //(int)   //min=0 max=65535 step=1 default=800 value=800
#define horizontal_flip                 0x00980914 //(bool)  //default=0 value=0
#define vertical_flip                   0x00980915 //(bool)  //default=0 value=0
#define alpha_component                 0x00980929 //(int)   //min=0 max=255 step=1 default=0 value=0
#define exposure_fine                   0x00982e01 //(int)   //min=0 max=65535 step=1 default=10 value=10
#define auto_exposure_min               0x00982e02 //(int)   //min=0 max=65535 step=1 default=1 value=1
#define auto_exposure_max               0x00982e03 //(int)   //min=0 max=65535 step=1 default=800 value=800
#define auto_exposure_target            0x00982e04 //(int)   //min=0 max=65535 step=1 default=20480 value=20480
#define auto_exposure_cur               0x00982e05 //(int)   //min=0 max=65535 step=1 default=0 value=800 flags=read-only, volatile
#define autogain_analogue               0x00982e06 //(bool)  //default=1 value=1
#define autogain_digital                0x00982e07 //(bool)  //default=0 value=0
#define analogue_gain_auto_min          0x00982e08 //(menu)  //min=0 max=3 default=0 value=0
#define row_binning                     0x00982e09 //(menu)  //min=0 max=1 default=0 value=0
#define col_binning                     0x00982e0a //(menu)  //min=0 max=2 default=0 value=0
#define compading                       0x00982e0b //(bool)  //default=0 value=0
#define digital_gain_red                0x00982e0c //(int)   //min=1000 max=15999 step=1 default=1300 value=1300
#define digital_gain_green_red          0x00982e0d //(int)   //min=1000 max=15999 step=1 default=1000 value=1000
#define digital_gain_blue               0x00982e0e //(int)   //min=1000 max=15999 step=1 default=1500 value=1500
#define digital_gain_green_blue         0x00982e0f //(int)   //min=1000 max=15999 step=1 default=1000 value=1000
#define black_level_correction          0x00982e11 //(bool)  //default=1 value=1
#define flash_delay                     0x00982e12 //(int)   //min=-128 max=127 step=1 default=0 value=0
#define dynamic_defect_pixel_correction 0x00982e13 //(bool)  //default=0 value=0

//Camera Controls

#define auto_exposure                   0x009a0901 //(menu)   //min=0 max=1 default=0 value=0

//Flash Controls

#define led_mode                        0x009c0901 //(menu)   //min=0 max=1 default=0 value=0

//Image Source Controls

#define vertical_blanking               0x009e0901 //(int)    //min=22 max=65535 step=1 default=22 value=22
#define horizontal_blanking             0x009e0902 //(int)    //min=208 max=65535 step=1 default=208 value=208
#define analogue_gain                   0x009e0903 //(int)    //min=1000 max=16000 step=1 default=2000 value=2000
#define red_pixel_value                 0x009e0904 //(int)    //min=0 max=4095 step=1 default=0 value=0
#define green_red_pixel_value           0x009e0905 //(int)    //min=0 max=4095 step=1 default=0 value=0
#define blue_pixel_value                0x009e0906 //(int)    //min=0 max=4095 step=1 default=0 value=0
#define green_blue_pixel_value          0x009e0907 //(int)    //min=0 max=4095 step=1 default=0 value=0

//Image Processing Controls

#define link_frequency                  0x009f0901 //(intmenu) //min=0 max=5 default=0 value=2
#define test_pattern                    0x009f0903 //(menu)    //min=0 max=4 default=0 value=0
#define digital_gain                    0x009f0905 //(int)     //min=1000 max=15999 step=1 default=1000 value=1000

#define IMAGE_W 1280
#define IMAGE_H 720

#ifdef CAP_RESOLUTION_1280x800
#define IMAGE_W 1280
#define IMAGE_H 720
#endif
#ifdef CAP_RESOLUTION_640x400
#define IMAGE_W 640
#define IMAGE_H 400
#endif
#ifdef CAP_RESOLUTION_320x200
#define IMAGE_W 320
#define IMAGE_H 200
#endif
#ifdef CAP_RESOLUTION_160x100
#define IMAGE_W 160
#define IMAGE_H 100
#endif

#define FRAME_NUM 1

//Map each buffer frame
struct buffer
{
    void* start;
    struct v4l2_plane plane;
    size_t length;
};

class V4L2 : public QObject
{
    Q_OBJECT

public:
    V4L2 ();
    ~V4L2 ();

    int open (string port);
    bool isOpened();
    bool read(cv::Mat &img);
    void release ();

private:
    int set_format ();
    int queryctrl(int ctrl);
    int setctrl(int ctrl, int value);
    int getctrl(int ctrl,int *value);
    bool request_buffer();
    bool query_buffer(unsigned int buff_index);
    bool queue_buffer(int buff_index);
    bool start_streaming();
    bool stop_streaming();
    std::string fourcc(unsigned int format);
    cv::Mat BA12_to_RGB24(unsigned char *ba12);
    int queryctrl();

    int fd;
    bool isCamOpen{false};
    struct buffer buffers[FRAME_NUM];
    unsigned int m_buffers; //Number of frame buffers
};

#endif // V4L2_H

#endif
