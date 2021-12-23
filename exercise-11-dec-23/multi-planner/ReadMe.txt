
Getting Build-Errors:
=====================

gcc test-app.c -o test

In file included from test-app.c:12:0:
v4l-helpers.h: In function ‘v4l_type_is_capture’:
v4l-helpers.h:1123:15: warning: implicit declaration of function ‘V4L2_TYPE_IS_CAPTURE’; did you mean ‘V4L2_CAP_RDS_CAPTURE’? [-Wimplicit-function-declaration]
        return V4L2_TYPE_IS_CAPTURE(type);
               ^~~~~~~~~~~~~~~~~~~~
               V4L2_CAP_RDS_CAPTURE
v4l-helpers.h: In function ‘v4l_buffer_g_request_fd’:
v4l-helpers.h:1204:17: error: ‘const struct v4l2_buffer’ has no member named ‘request_fd’
  return buf->buf.request_fd;
                 ^
v4l-helpers.h: In function ‘v4l_buffer_s_request_fd’:
v4l-helpers.h:1209:10: error: ‘struct v4l2_buffer’ has no member named ‘request_fd’
  buf->buf.request_fd = request_fd;
          ^
v4l-helpers.h: At top level:
v4l-helpers.h:1520:59: error: expected ‘;’, ‘,’ or ‘)’ before ‘=’ token
   struct v4l_queue *q, unsigned count, unsigned int flags = 0)
                                                           ^
v4l-helpers.h:1553:53: error: expected ‘;’, ‘,’ or ‘)’ before ‘=’ token
   const struct v4l2_format *fmt, unsigned int flags = 0)
                                                     ^
v4l-helpers.h: In function ‘v4l_queue_free’:
v4l-helpers.h:1740:2: warning: implicit declaration of function ‘v4l_queue_reqbufs’; did you mean ‘v4l_queue_free_bufs’? [-Wimplicit-function-declaration]
  v4l_queue_reqbufs(f, q, 0, 0);
  ^~~~~~~~~~~~~~~~~
  v4l_queue_free_bufs
test-app.c: In function ‘request_query_buffers’:
test-app.c:89:46: error: ‘REQ_BUFF_SZ’ undeclared (first use in this function)
     ret = v4l_queue_reqbufs( &cam1, &vqueue, REQ_BUFF_SZ );
                                              ^~~~~~~~~~~
test-app.c:89:46: note: each undeclared identifier is reported only once for each function it appears in

