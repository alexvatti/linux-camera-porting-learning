#include <QCoreApplication>

#include "v4l2.h"
#include "opencv2/opencv.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (argc != 2){
        qDebug()<<"usage:";
        qDebug()<<"./ov5648 ov5648 or ./ov5648 ar0144";
        return 0;
    }
    V4L2 cam(!strcmp(argv[1],"ov5648"));
    cam.open("/dev/video0");
    cv::Mat mat;
    if (cam.read(mat)){
        qDebug()<<"size != 0";
        if (cv::imwrite("./temp.jpg",mat))
            qDebug()<<"write success";
        else
            qDebug()<<"write fail";
    }
    else
        qDebug()<<"camera read error";
    cam.release();
    return 0;
    return a.exec();
}
