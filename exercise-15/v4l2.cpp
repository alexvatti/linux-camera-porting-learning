#include "v4l2.h"

#include <QThread>

#ifdef TARGET_BUILD

V4L2::V4L2 ()
{
    fd = -1;
}

V4L2::~V4L2 ()
{
}

std::string V4L2::fourcc(unsigned int format)
{
    char formatArray[] = { (char)(format&0xff), (char)((format>>8)&0xff), (char)((format>>16)&0xff), (char)((format>>24)&0xff), 0 };
    return std::string(formatArray, strlen(formatArray));
}

int V4L2::open(string port, SensorType_t sensor)
{
    m_sensor = sensor;
    m_port = port;
    //Turn on the camera
    fd = ::open(port.c_str(),O_RDWR | O_NONBLOCK);
    if(fd < 0) {
        //qDebug()<<"camera open error!!!";
        return -1;
    }else{
        //qDebug()<<"camera open success";
    }

    //stop_streaming();
    //Query the capabilities of video equipment
    struct v4l2_capability cap;
    memset (&cap, 0, sizeof(cap));
    if(ioctl (fd, VIDIOC_QUERYCAP, &cap) == -1) {
        //qDebug()<<"VIDIOC_QUERYCAP error: "<<errno;
        return -1;
    }else{
        //qDebug()<<"VIDIOC_QUERYCAP success";
    }

    //qDebug()<<"cap.capabilities: "<<hex<<cap.capabilities;
    if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) != V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        //qDebug()<<"Device does not support V4L2_CAP_VIDEO_CAPTURE_MPLANE";
        return -1;
    }else{
        //qDebug()<<"Device supports V4L2_CAP_VIDEO_CAPTURE_MPLANE";
    }

    if ((cap.capabilities & V4L2_CAP_STREAMING) != V4L2_CAP_STREAMING)
    {
        //qDebug()<<"Device does not supports V4L2_CAP_STREAMING";
        return -1;
    }else{
        //qDebug()<<"Device supports V4L2_CAP_STREAMING";
    }

    //Set the parameters of video capture
    //1. Set the video format
    //2. Set the video image capture window size
    //3. Set the format of the video
    get_format();
    set_format();
    get_format();
    //4. Set the frame rate of the video
    //5. Set the rotation method of the video
    if (m_sensor == V4L2::SENSOR_AR0144){
        set_ctrl(digital_gain_red,1400);
        set_ctrl(digital_gain_blue,1700);
    }else if (m_sensor == V4L2::SENSOR_OV5648){
        set_ctrl(auto_exposure,1);
        set_ctrl(gain_automatic,1);
        set_ctrl(white_balance_automatic,1);
    }else if (m_sensor == V4L2::SENSOR_IMX219){
    }

    //Apply for several frame buffers, generally not less than 3
    request_buffer ();
    //Query the length and offset of the frame buffer in the kernel space
    for (m_buffers = 0; m_buffers < FRAME_NUM; ++m_buffers)
    {
        buffers[m_buffers].start = MAP_FAILED;
        query_buffer (m_buffers);
    }
    //Open video capture
    for(unsigned int i=0;i<m_buffers;i++) {
        queue_buffer (i);
    }
    start_streaming ();
    QThread::msleep(100);

    isCamOpen = true;
    return isCamOpen;
}

bool V4L2::isOpened()
{
    return isCamOpen;
}

int V4L2::query_ctrl(int ctrl){
    struct v4l2_queryctrl v4l2_qctrl;
    memset (&v4l2_qctrl, 0, sizeof (v4l2_qctrl));
    v4l2_qctrl.id = ctrl;
    if (-1 == ioctl (fd, VIDIOC_QUERYCTRL, &v4l2_qctrl)) {
        //qDebug()<<"VIDIOC_QUERYCTRL error: "<<errno;
        return -1;
    }else{
        //qDebug()<<"VIDIOC_QUERYCTRL success";
    }
    return 0;
}

int V4L2::set_ctrl(int ctrl, int value)
{
    struct v4l2_control v4l2_ctrl;
    memset (&v4l2_ctrl, 0, sizeof (v4l2_ctrl));
    v4l2_ctrl.id = ctrl;
    v4l2_ctrl.value = value;
    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &v4l2_ctrl)) {
        //qDebug()<<"VIDIOC_S_CTRL error: "<<errno;
        return -1;
    }else{
        //qDebug()<<"VIDIOC_S_CTRL success";
    }
    return 0;
}

int V4L2::get_ctrl(int ctrl, int *value)
{
    struct v4l2_control v4l2_ctrl;
    memset (&v4l2_ctrl, 0, sizeof (v4l2_ctrl));
    v4l2_ctrl.id = ctrl;
    if (-1 == ioctl (fd, VIDIOC_G_CTRL, &v4l2_ctrl)) {
        //qDebug()<<"VIDIOC_G_CTRL error: "<<errno;
        return -1;
    }else{
        //qDebug()<<"VIDIOC_G_CTRL success";
    }
    *value = v4l2_ctrl.value;
    return 0;
}

int V4L2::set_format() {
    int res = -1;
    struct v4l2_format format;
    memset(&format,0,sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix_mp.width = Sensors[m_sensor].imageWidth;
    format.fmt.pix_mp.height = Sensors[m_sensor].imageHeight;
    format.fmt.pix_mp.pixelformat = Sensors[m_sensor].imageFormat;
    //qDebug()<<"output image format: "<<fourcc(Sensors[m_sensor].imageFormat).c_str();
    if (m_sensor == V4L2::SENSOR_AR0144){
        format.fmt.pix_mp.field = V4L2_FIELD_NONE;
    }else if (m_sensor == V4L2::SENSOR_OV5648){
        format.fmt.pix_mp.field = V4L2_FIELD_NONE;
    }else if (m_sensor == V4L2::SENSOR_IMX219){
        format.fmt.pix_mp.field = V4L2_FIELD_NONE;
    }
    res = ioctl(fd, VIDIOC_S_FMT, &format);
    if (-1 == res) {
        //qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_S_FMT error: "<<errno;
    }else{
        //qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_S_FMT success";
    }
    return res;
}

int V4L2::get_format() {
    int res = -1;
    struct v4l2_format format;
    memset(&format,0,sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    res = ioctl(fd, VIDIOC_G_FMT, &format);
    if (-1 == res) {
        //qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_S_FMT error: "<<errno;
    }else{
        //qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_S_FMT success";
        qDebug()<<"##########";
        qDebug()<<m_port.c_str();
        qDebug()<<"width: "<<format.fmt.pix_mp.width;
        qDebug()<<"height: "<<format.fmt.pix_mp.height;
        qDebug()<<"colorspace: "<<format.fmt.pix_mp.colorspace;
        qDebug()<<"ycbcr: "<<format.fmt.pix_mp.ycbcr_enc;
        qDebug()<<"field: "<<format.fmt.pix_mp.field;
        qDebug()<<"format: "<<fourcc(format.fmt.pix_mp.pixelformat).c_str();
        qDebug()<<"xfer: "<<format.fmt.pix_mp.xfer_func;
        qDebug()<<"~~~~~~~~~~";
    }
    return res;
}

bool V4L2::request_buffer() {
    struct v4l2_requestbuffers req;
    memset(&req,0,sizeof(req));
    req.count = FRAME_NUM;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    req.memory = V4L2_MEMORY_MMAP;
    int res = ioctl(fd, VIDIOC_REQBUFS, &req);
    if (res == -1)
    {
        //qDebug()<<"VIDIOC_REQBUFS error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_REQBUFS success";
    }
    return true;
}

bool V4L2::query_buffer (unsigned int buff_index) {
    struct v4l2_buffer buff;
    memset (&buff,0,sizeof (buff));
    buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buff.memory = V4L2_MEMORY_MMAP;
    buff.index = buff_index;
    buff.m.planes = &buffers[buff_index].plane;
    buff.length = 1;
    int res = ioctl (fd, VIDIOC_QUERYBUF, &buff);
    if(res != 0) {
        //qDebug()<<"VIDIOC_QUERYBUF error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_QUERYBUF success";
    }
    buffers[buff_index].start = mmap(NULL, buffers[buff_index].plane.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffers[buff_index].plane.m.mem_offset);
    buffers[buff_index].length = buffers[buff_index].plane.length;
    if (buffers[buff_index].start == MAP_FAILED) {
        //qDebug()<<"MMAP error!!!";
        return false;
    }else{
        //qDebug()<<"MMAP success";
    }
    return true;
}

bool V4L2::queue_buffer(int buff_index) {
    struct v4l2_buffer buff;
    memset (&buff,0,sizeof (buff));
    buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buff.memory = V4L2_MEMORY_MMAP;
    buff.m.planes = &buffers[buff_index].plane;

    if (m_sensor == V4L2::SENSOR_AR0144){
        buff.field = V4L2_FIELD_NONE;
    }else if (m_sensor == V4L2::SENSOR_OV5648){
        buff.field = V4L2_FIELD_NONE;
    }else if (m_sensor == V4L2::SENSOR_IMX219){
        buff.field = V4L2_FIELD_NONE;
    }

    buff.index = buff_index;
    buff.length = 1;

    if(-1 == ioctl (fd, VIDIOC_QBUF, &buff))
    {
        //qDebug()<<"VIDIOC_QBUF Error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_QBUF success";
    }
    return true;
}

bool V4L2::start_streaming () {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(ioctl (fd, VIDIOC_STREAMON, &type) == -1){
        //qDebug()<<"VIDIOC_STREAMON Error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_STREAMON success";
    }
    return true;
}

bool V4L2::stop_streaming () {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(ioctl (fd, VIDIOC_STREAMOFF, &type) == -1){
        //qDebug()<<"VIDIOC_STREAMOFF Error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_STREAMOFF success";
    }
    return true;
}

bool V4L2::read (cv::Mat &img)
{
    if (isCamOpen == false)
        return false;
    struct v4l2_buffer buff;
    memset (&buff, 0, sizeof(buff));
    buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buff.memory = V4L2_MEMORY_MMAP;
    if (m_sensor == V4L2::SENSOR_AR0144){
        buff.field = V4L2_FIELD_NONE;
    }else if (m_sensor == V4L2::SENSOR_OV5648){
        buff.field = V4L2_FIELD_NONE;
    }else if (m_sensor == V4L2::SENSOR_IMX219){
        buff.field = V4L2_FIELD_NONE;
    }

    buff.m.planes = &buffers[buff.index].plane;
    buff.length = 1; //number of planes

    int ret = ioctl (fd, VIDIOC_DQBUF, &buff);
    if (-1 == ret){
        //qDebug()<<"VIDIOC_DQBUF error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_DQBUF success";
    }

    //qDebug()<<"buffer length: "<<buffers[buff.index].length;
    //qDebug()<<"buff.index: "<<buff.index;
    if (m_sensor == V4L2::SENSOR_AR0144){
        cv::Mat Mat(Sensors[m_sensor].imageHeight, Sensors[m_sensor].imageWidth, CV_8UC1,(unsigned char *)buffers[buff.index].start);
        cv::cvtColor(Mat, img, cv::COLOR_BayerGR2RGB);
    }else if (m_sensor == V4L2::SENSOR_OV5648){
        cv::Mat Mat(Sensors[m_sensor].imageHeight, Sensors[m_sensor].imageWidth, CV_8UC1,(unsigned char *)buffers[buff.index].start);
        cv::cvtColor(Mat, img, cv::COLOR_BayerGR2RGB);
    }else if (m_sensor == V4L2::SENSOR_IMX219){
        cv::Mat Mat(Sensors[m_sensor].imageHeight, Sensors[m_sensor].imageWidth, CV_8UC1,(unsigned char *)buffers[buff.index].start);
        cv::cvtColor(Mat, img, cv::COLOR_BayerGR2RGB);
    }

    if (-1 == ioctl (fd, VIDIOC_QBUF, &buff)) {
        //qDebug()<<"VIDIOC_QBUF error: "<<errno;
        return false;
    }else{
        //qDebug()<<"VIDIOC_QBUF success";
    }
    return true;
}

//v4l2-ctl --set-fmt-video=width=1280,height=720,pixelformat=YUYV --stream-mmap --stream-count=1 --device /dev/video0 --stream-to=/root/ov5648-1.raw
//v4l2-ctl --set-fmt-video=width=1280,height=800,pixelformat=YUYV --stream-mmap --stream-count=1 --device /dev/video0 --stream-to=/root/ar0144-1.raw

void V4L2::release ()
{
    if (isCamOpen == true){
        //Close video capture
        stop_streaming ();

        //Release memory
        for (unsigned int i = 0;i < m_buffers;i++) {
            if (buffers[i].start != MAP_FAILED){
                if (-1 == munmap (buffers[i].start, buffers[i].length)){
                    //qDebug()<<"camera munmap error";
                }
            }else{
                //qDebug()<<"camera munmap success";
            }
        }
        ::close(fd);
        isCamOpen = false;
    }else{
        //qDebug()<<"cam not opened to release";
    }
}

#endif  //TARGET_BUILD

