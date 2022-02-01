#include "v4l2.h"

#ifdef TARGET_BUILD

V4L2::V4L2 (bool ov5648)
{
    fd = -1;
    is_ov5648 = ov5648;
    if (is_ov5648){
        IMAGE_W = 1280;
        IMAGE_H = 720;
    }else{
        IMAGE_W = 1280;
        IMAGE_H = 800;
    }
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
        qDebug()<<"camera open error!!!";
        return -1;
    }else qDebug()<<"camera open success";

    stop_streaming();
    if (!is_ov5648){
        setctrl(digital_gain_red,1400);
        setctrl(digital_gain_blue,1700);
    }

    //Query the capabilities of video equipment
    struct v4l2_capability cap;
    memset (&cap, 0, sizeof(cap));
    if(ioctl (fd, VIDIOC_QUERYCAP, &cap) == -1) {
        qDebug()<<"VIDIOC_QUERYCAP error";
        return -1;
    }else qDebug()<<"VIDIOC_QUERYCAP success";

    //qDebug()<<"cap.capabilities: "<<hex<<cap.capabilities;
    if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE) != V4L2_CAP_VIDEO_CAPTURE_MPLANE)
    {
        qDebug()<<"Device does not support V4L2_CAP_VIDEO_CAPTURE_MPLANE";
        return -1;
    }else qDebug()<<"Device supports V4L2_CAP_VIDEO_CAPTURE_MPLANE";

    if ((cap.capabilities & V4L2_CAP_STREAMING) != V4L2_CAP_STREAMING)
    {
        qDebug()<<"Device does not supports V4L2_CAP_STREAMING";
        return -1;
    }else qDebug()<<"Device supports V4L2_CAP_STREAMING";

    //Set the parameters of video capture
    //1. Set the video format
    //2. Set the video image capture window size
    //3. Set the format of the video
    get_format();
    set_format ();
    //4. Set the frame rate of the video
    //5. Set the rotation method of the video

    //Apply for several frame buffers, generally not less than 3
    request_buffer ();
    //Query the length and offset of the frame buffer in the kernel space
    for(m_buffers = 0; m_buffers<FRAME_NUM; ++m_buffers)
    {
        buffers[m_buffers].start = MAP_FAILED;
        query_buffer (m_buffers);
    }
    //Open video capture
    for(unsigned int i = 0; i<m_buffers; i++) {
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
        qDebug()<<"VIDIOC_QUERYCTRL error";
        return -1;
    }else qDebug()<<"VIDIOC_QUERYCTRL success";
    return 0;
}

int V4L2::setctrl(int ctrl, int value)
{
    struct v4l2_control v4l2_ctrl;
    memset (&v4l2_ctrl, 0, sizeof (v4l2_ctrl));
    v4l2_ctrl.id = ctrl;
    v4l2_ctrl.value = value;
    if (-1 == ioctl (fd, VIDIOC_S_CTRL, &v4l2_ctrl)) {
        qDebug()<<"VIDIOC_S_CTRL error";
        return -1;
    }else qDebug()<<"VIDIOC_S_CTRL success";
    return 0;
}

int V4L2::getctrl(int ctrl, int *value)
{
    struct v4l2_control v4l2_ctrl;
    memset (&v4l2_ctrl, 0, sizeof (v4l2_ctrl));
    v4l2_ctrl.id = ctrl;
    if (-1 == ioctl (fd, VIDIOC_G_CTRL, &v4l2_ctrl)) {
        qDebug()<<"VIDIOC_G_CTRL error";
        return -1;
    }else qDebug()<<"VIDIOC_G_CTRL success";
    *value = v4l2_ctrl.value;
    return 0;
}

int V4L2::set_format() {
    struct v4l2_format format;
    memset(&format,0,sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    format.fmt.pix_mp.width = IMAGE_W;
    format.fmt.pix_mp.height = IMAGE_H;
    if (is_ov5648){
        format.fmt.pix_mp.colorspace = V4L2_COLORSPACE_JPEG;
        format.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SBGGR8;
        format.fmt.pix_mp.field = V4L2_FIELD_ANY;
    }
    else
    {
        format.fmt.pix_mp.pixelformat = V4L2_PIX_FMT_SGRBG12;
        format.fmt.pix_mp.field = V4L2_FIELD_INTERLACED;
    }
    int res = ioctl(fd, VIDIOC_S_FMT, &format);
    if(res == -1) {
        qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_S_FMT error";
    }else qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_S_FMT success";
    return res;
}

int V4L2::get_format() {
    struct v4l2_format format;
    memset(&format,0,sizeof(format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    int res = ioctl(fd, VIDIOC_G_FMT, &format);
    if(res == -1) {
        qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_G_FMT error";
    }else qDebug()<<"V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE->VIDIOC_G_FMT success";

    qDebug()<<"width: "<<format.fmt.pix_mp.width;
    qDebug()<<"height: "<<format.fmt.pix_mp.height;
    qDebug()<<"planes: "<<format.fmt.pix_mp.num_planes;
    qDebug()<<"colorspace: "<<format.fmt.pix_mp.colorspace;
    qDebug()<<"pixelformat: "<<fourcc(format.fmt.pix_mp.pixelformat).c_str();
    qDebug()<<"feild: "<<format.fmt.pix_mp.field;
    qDebug()<<"quant: "<<format.fmt.pix_mp.quantization;
    qDebug()<<"xfer: "<<format.fmt.pix_mp.xfer_func;
    qDebug()<<"ycbcr: "<<format.fmt.pix_mp.ycbcr_enc;

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
        qDebug()<<"VIDIOC_REQBUFS error";
        return false;
    }else qDebug()<<"VIDIOC_REQBUFS success";
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
        qDebug()<<"VIDIOC_QUERYBUF error";
        return false;
    }else qDebug()<<"VIDIOC_QUERYBUF success";
    buffers[buff_index].start = mmap(NULL, buffers[buff_index].plane.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buffers[buff_index].plane.m.mem_offset);
    buffers[buff_index].length = buffers[buff_index].plane.length;
    if (buffers[buff_index].start == MAP_FAILED) {
        qDebug()<<"MMAP error!!!";
        return false;
    }else qDebug()<<"MMAP success";
    return true;
}

bool V4L2::queue_buffer(int buff_index) {
    struct v4l2_buffer buff;
    memset (&buff,0,sizeof (buff));
    buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    buff.memory = V4L2_MEMORY_MMAP;
    buff.m.planes = &buffers[buff_index].plane;
    if (is_ov5648)
    {
        buff.field = V4L2_FIELD_NONE;
    }
    else
    {
        buff.field = V4L2_FIELD_INTERLACED;
    }
    buff.index = buff_index;
    buff.length = 1;

    if (-1 == ioctl (fd, VIDIOC_QBUF, &buff))
    {
        qDebug()<<"VIDIOC_QBUF Error";
        return false;
    }else qDebug()<<"VIDIOC_QBUF success";
    return true;
}

bool V4L2::start_streaming () {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(ioctl (fd, VIDIOC_STREAMON, &type) == -1){
        qDebug()<<"VIDIOC_STREAMON Error";
        return false;
    }else qDebug()<<"VIDIOC_STREAMON success";
    return true;
}

bool V4L2::stop_streaming () {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    if(ioctl (fd, VIDIOC_STREAMOFF, &type) == -1){
        qDebug()<<"VIDIOC_STREAMOFF Error";
        return false;
    }else qDebug()<<"VIDIOC_STREAMOFF success";
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
    if (is_ov5648)
    {
        buf.field = V4L2_FIELD_NONE;
    }
    else
    {
        buf.field = V4L2_FIELD_INTERLACED;
    }
    buf.m.planes = &buffers[buf.index].plane;
    buf.length = 1; //number of planes

    if(-1 == ioctl (fd, VIDIOC_DQBUF, &buf)) {
        qDebug()<<"VIDIOC_DQBUF error";
        return false;
    }
    else
        qDebug()<<"VIDIOC_DQBUF success";

    //qDebug()<<"buf.index: "<<buf.index;
    img = BA12_to_RGB24((unsigned char *)buffers[buf.index].start);

    if(-1 == ioctl (fd, VIDIOC_QBUF, &buf)) {
        qDebug()<<"VIDIOC_QBUF error";
        return false;
    }
    else
        qDebug()<<"VIDIOC_QBUF success";
    return true;
}

cv::Mat V4L2::BA12_to_RGB24(unsigned char *ba12)
{
    //https://answers.opencv.org/question/20748/convert-bayerbg12packed-12-bit-packed-into-rgb/
    cv::Mat Mat16Bit(IMAGE_H, IMAGE_W, CV_16UC1, ba12);
    cv::Mat Mat8Bit = Mat16Bit.clone();
    Mat16Bit.convertTo(Mat8Bit, CV_8UC3, 1.0/256);
    cv::Mat MatRgb(IMAGE_W, IMAGE_H, CV_8UC3);
    if (is_ov5648)
    {
        cv::cvtColor(Mat8Bit, MatRgb, cv::COLOR_BayerBG2RGB);   //ov5648
    }
    else
    {
        cv::cvtColor(Mat8Bit, MatRgb, cv::COLOR_BayerGR2RGB);   //ar0144
        /*if (pixformat == SGRBG8_1X8)
            cv::COLOR_BayerGB2BGR*/
    }
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
                    qDebug()<<"camera munmap error";
            }else qDebug()<<"camera munmap success";
        }
        ::close(fd);
        isCamOpen = false;
    }else qDebug()<<"cam not opened to release";
}

#endif  //TARGET_BUILD

