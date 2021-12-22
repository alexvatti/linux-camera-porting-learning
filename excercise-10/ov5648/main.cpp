#include <QCoreApplication>

#include "v4l2.h"
#include "opencv2/opencv.hpp"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    V4L2 cam;
    cam.open("/dev/video0");
    cv::Mat mat;
    if (cam.read(mat)){
        qDebug()<<"size != 0";
        if (cv::imwrite("/root/temp.jpg",mat))
            qDebug()<<"write success";
        else
            qDebug()<<"write fail";
    }else qDebug()<<"camera read error";
    cam.release();
    return 0;
    return a.exec();
}
