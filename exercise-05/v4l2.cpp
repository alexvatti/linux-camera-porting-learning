#include "v4l2.h"

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

int V4L2::open(string port)
{
    //Turn on the camera
    fd = ::open(port.c_str(),O_RDWR | O_NONBLOCK);
    if(fd < 0) {
        qDebug()<<"open camera error!!!";
        return -1;
    }

    //stop_streaming();
    setctrl(digital_gain_red,1400);
    setctrl(digital_gain_blue,1700);

    //Query the capabilities of video equipment
    struct v4l2_capability cap;
    memset (&cap, 0, sizeof(cap));
    if(ioctl (fd, VIDIOC_QUERYCAP, &cap) == -1) {
        qDebug()<<"cap error !!!";
        return -1;
    }

    //qDebug()<<"cap.capabilities: "<<hex<<cap.capabilities;
    if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) != V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        qDebug()<<"Device: does not support V4L2_CAP_VIDEO_CAPTURE_MPLANE";
        return -1;
    }

    if ((cap.capabilities & V4L2_CAP_STREAMING) != V4L2_CAP_STREAMING)
    {
        qDebug()<<"Device: does not supports V4L2_CAP_STREAMING";
        return -1;
    }

    //Set the parameters of video capture
    //1. Set the video format
    //2. Set the video image capture window size
    //3. Set the format of the video
    set_format ();
    //4. Set the frame rate of the video
    //5. Set the rotation method of the video

    //Apply for several frame buffers, generally not less than 3
    //Query the length and offset of the frame buffer in the kernel space
    request_buffer ();
    for(m_buffers = 0; m_buffers < FRAME_NUM; ++m_buffers)
    {
        buffers[m_buffers].start = MAP_FAILED;
        query_buffer (m_buffers);
    }
    //Open video capture
    for(unsigned int i=0;i<m_buffers;i++) {
        queue_buffer (i);
    }
    start_streaming ();

    isCamOpen = true;
    return isCamOpen;
}

bool V4L2::isOpened()
{
    return isCamOpen;
}

int V4L2::queryctrl(int ctrl){
    struct v4l2_queryctrl v4l2_qctrl;
    memset (&v4l2_qctrl, 0, sizeof (v4l2_qctrl));
    v4l2_qctrl.id = ctrl;
    if (-1 == ioctl (fd, VIDIOC_QUERYCTRL, &v4l2_qctrl)) {
        qDebug()<<"queryctrl error";
        return -1;
    }
    return 0;
}

int V4L2::setctrl(int ctrl, int value)
{
    struct v4l2_control v4l2_ctrl;
    memset (&v4l2_ctrl, 0, sizeof (v4l2_ctrl));
    v4l2_ctrl.id = ctrl;
    v4l2_ctrl.value = value;
    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &v4l2_ctrl)) {
        qDebug()<<"setctrl error";
        return -1;
    }
    return 0;
}

int V4L2::getctrl(int ctrl, int *value)
{
    struct v4l2_control v4l2_ctrl;
    memset (&v4l2_ctrl, 0, sizeof (v4l2_ctrl));
    v4l2_ctrl.id = ctrl;
    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &v4l2_ctrl)) {
        qDebug()<<"getctrl error";
        return -1;
    }
    *value = v4l2_ctrl.value;
    return 0;
}

int V4L2::set_format() {
    struct v4l2_format format;
    memset(&format,0,sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix_mp.width = IMAGE_W;
    format.fmt.pix_mp.height = IMAGE_H;
    format.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SGRBG12;
    format.fmt.pix_mp.field = V4L2_FIELD_INTERLACED;
    int res = ioctl(fd, VIDIOC_S_FMT, &format);
    if(res == -1) {
        perror("Could not set format");
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
        perror("Requesting Buffer");
        return false;
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
        perror("Could not query buffer");
        return false;
    }
    buffers[buff_index].start = mmap(NULL, buffers[buff_index].plane.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffers[buff_index].plane.m.mem_offset);
    buffers[buff_index].length = buffers[buff_index].plane.length;
    if (buffers[buff_index].start == MAP_FAILED) {
        qDebug()<<"camera mmap error!!!";
        return false;
    }
    return true;
}

bool V4L2::queue_buffer(int buff_index) {
    struct v4l2_buffer buff;
    memset (&buff,0,sizeof (buff));
    buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buff.memory = V4L2_MEMORY_MMAP;
    buff.m.planes = &buffers[buff_index].plane;
    buff.field = V4L2_FIELD_INTERLACED;
    buff.index = buff_index;
    buff.length = 1;

    if(-1 == ioctl (fd, VIDIOC_QBUF, &buff))
    {
        perror("Queue Buffer Error");
        return false;
    }
    return true;
}

bool V4L2::start_streaming () {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(ioctl (fd, VIDIOC_STREAMON, &type) == -1){
        perror ("VIDIOC_STREAMON Error");
        return false;
    }
    return true;
}

bool V4L2::stop_streaming () {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(ioctl (fd, VIDIOC_STREAMOFF, &type) == -1){
        perror ("VIDIOC_STREAMOFF Error");
        return false;
    }
    return true;
}

bool V4L2::read (cv::Mat &img)
{
    if (isCamOpen == false)
        return false;
    struct v4l2_buffer buf;
    memset (&buf, 0, sizeof(buf));
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.field = V4L2_FIELD_INTERLACED;
    buf.m.planes = &buffers[buf.index].plane;
    buf.length = 1;
    if(-1 == ioctl (fd, VIDIOC_DQBUF, &buf)) {
        //qDebug()<<"camera dqbuf error!!!";
        return false;
    }
    //qDebug()<<"buf.index: "<<buf.index;
    img = BA12_to_RGB24((unsigned char *)buffers[buf.index].start);

    if(-1 == ioctl (fd, VIDIOC_QBUF, &buf)) {
        //qDebug()<<"camera qbuf error!!!";
        return false;
    }
    return true;
}

cv::Mat V4L2::BA12_to_RGB24(unsigned char *ba12)
{
    //https://answers.opencv.org/question/20748/convert-bayerbg12packed-12-bit-packed-into-rgb/
    cv::Mat Mat16Bit(IMAGE_H, IMAGE_W, CV_16UC1, ba12);
    cv::Mat Mat8Bit = Mat16Bit.clone();
    Mat16Bit.convertTo(Mat8Bit, CV_8UC3, 1.0/256);
    cv::Mat MatRgb(IMAGE_W, IMAGE_H, CV_8UC3);
    cv::cvtColor(Mat8Bit, MatRgb, cv::COLOR_BayerGR2RGB);
    return MatRgb;
}

void V4L2::release ()
{
    if (isCamOpen == true){
        //Close video capture
        stop_streaming ();

        //Release memory
        for (unsigned int i = 0;i < m_buffers;i++) {
            if (buffers[i].start != MAP_FAILED){
                if (-1 == munmap (buffers[i].start, buffers[i].length))
                    qDebug()<<"camera munmap error!!!";
            }
        }
        ::close(fd);
        isCamOpen = false;
    }
}

#endif  //TARGET_BUILD

