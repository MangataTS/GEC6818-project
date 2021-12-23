#include "picture.h"
#include <sys/ioctl.h>
#include <linux/videodev2.h>

// unsigned char g_color_buf[800*480*3]={0};//像素点空间缓存
// unsigned int *p;   //lcd屏映射内存

struct 
{
	void *start;
	size_t length;
} *buffers;


/* 摄像头实时显示 */
int lcd_draw_video(unsigned int x,unsigned int y, unsigned char *jpg_buf, unsigned int jpg_size)  
{
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;
	struct 	jpeg_error_mgr 			jerr;	
	
	unsigned char 	*pcolor_buf = g_color_buf;//*（pcolor_buf+0）== g_color_buf[0]
	char 	*pjpg;
	
	unsigned int 	i=0;
	unsigned int	color =0;
	
	unsigned int 	x_s = x;//x的映射起始位置
	unsigned int	y_n	= y;//x和y的映射的起始位置
	unsigned int	x_n	= x;
	
	pjpg = jpg_buf;

	/*注册出错处理*/
	cinfo.err = jpeg_std_error(&jerr);

	/*创建解码*/
	jpeg_create_decompress(&cinfo);

	/*直接解码内存数据*/		
	jpeg_mem_src(&cinfo,pjpg,jpg_size);
	
	/*读文件头*/
	jpeg_read_header(&cinfo, TRUE);

	/*开始解码*/
	jpeg_start_decompress(&cinfo);	
	
	/*读解码数据*/
	while(cinfo.output_scanline < cinfo.output_height )
	{		
		pcolor_buf = g_color_buf;		
		/* 读取jpg一行的rgb值 */
		jpeg_read_scanlines(&cinfo,&pcolor_buf,1);
		
		for(i=0; i<cinfo.output_width; i++)
		{		
			/* 获取rgb值 */
			color = 		*(pcolor_buf+2);
			color = color | *(pcolor_buf+1)<<8;
			color = color | *(pcolor_buf)<<16;				
			/* 显示像素点 */
			lcd_one_pixel(x_n,y_n,color);		
			pcolor_buf +=3;
			
			x_n++;
		}
		
		/* 换行 */
		y_n++;			
		
		x_n = x_s;		
	}		
			
	/*解码完成*/
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	return 0;
}

void *Pcamera(void *arg)
{
	int cam_fd;
	int i;//数据块循环次数
	
	//打开摄像头文件 (先开机在插摄像头，设备号为video7)
	cam_fd = open("/dev/video7", O_RDWR);
	if(cam_fd == -1)
	{
		perror("open() camera failed");
		exit(0);
	}
	
	//获取摄像头的功能信息
	struct v4l2_capability cap;
	if(ioctl(cam_fd, VIDIOC_QUERYCAP, &cap))
	{
		perror("VIDIOC_QUERYCAP");
		exit(0);
	}
	printf("driver:%s\n", cap.driver);
	printf("card:%s\n", cap.card);
	printf("bus_info:%s\n", cap.bus_info);
	printf("version:%d\n", cap.version);
	printf("capabilities:%d\n", cap.capabilities);
	// printf("device_caps:%d\n", cap.device_caps);
	printf("--------------------------------------------------\n");
	
	//设置摄像头的通道
	int index;
	index = 0;
	if(ioctl(cam_fd, VIDIOC_S_INPUT, &index))
	{
		perror("VIDIOC_S_INPUT");
		exit(0);
	}
	
	//获取摄像头的采集格式
	struct v4l2_format fmt;
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl(cam_fd, VIDIOC_G_FMT, &fmt))
	{
		perror("VIDIOC_G_FMT");
		exit(0);
	}
	printf("width: %d, height: %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
	unsigned char *p = (unsigned char *)&fmt.fmt.pix.pixelformat;
	printf("pixelformat: %c%c%c%c\n", p[0], p[1], p[2], p[3]);
	
	//设置摄像头采集格式
	bzero(&fmt, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 640; //320
	fmt.fmt.pix.height = 480;  //240
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;
	
	if(ioctl(cam_fd, VIDIOC_S_FMT, &fmt))
	{
		perror("VIDIOC_S_FMT");
		exit(0);
	}

	//申请分配缓冲区
	struct v4l2_requestbuffers reqbuf;
	bzero(&reqbuf, sizeof(reqbuf));
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_MMAP;
	reqbuf.count = 4;
	
	if(ioctl (cam_fd, VIDIOC_REQBUFS, &reqbuf))
	{
		perror("VIDIOC_REQBUFS");
		exit(0);
	}
	
	//分配四个缓冲区的内存地址
	buffers = calloc(reqbuf.count, sizeof(*buffers));
	
	struct v4l2_buffer buffer;
	
	
	for(i=0; i<reqbuf.count; i++)
	{
		bzero(&buffer, sizeof(buffer));
		buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buffer.memory = V4L2_MEMORY_MMAP;
		buffer.index = i;
		if(ioctl (cam_fd, VIDIOC_QUERYBUF, &buffer))
		{
			perror("VIDIOC_QUERYBUF");
			exit(0);
		}
		
		//将缓冲块映射到用户空间
		buffers[i].length = buffer.length;
		buffers[i].start = mmap (NULL,  buffer.length, PROT_READ|PROT_WRITE, MAP_SHARED, cam_fd, buffer.m.offset);
		
		if(buffers[i].start == NULL)
		{
			perror("buffers[i].start failed");
			exit(0);
		}		
		
		//将缓冲区数据全部入队
		if(ioctl(cam_fd , VIDIOC_QBUF, &buffer))
		{
			perror("VIDIOC_QBUF");
			exit(0);
		}
	}
	
	
	struct v4l2_buffer v4lbuf;
	memset(&v4lbuf,0,sizeof(v4lbuf));
	v4lbuf.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4lbuf.memory=V4L2_MEMORY_MMAP;
	
	//开始摄像头数据采集
	enum v4l2_buf_type vtype= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl (cam_fd, VIDIOC_STREAMON, &vtype))
	{
		perror("VIDIOC_STREAMON");
		exit(0);
	}
	
	sleep(1);
	
	while(1)
	{
		//获取帧数据缓冲
		for(i=0; i<reqbuf.count; i++)
		{
			v4lbuf.index = i;
			if(ioctl(cam_fd , VIDIOC_DQBUF, &v4lbuf))
			{
				perror("VIDIOC_DQBUF");
				exit(0);
			}
			
			// jpg_fd = open("1.jpg", O_RDWR|O_CREAT, 0777);			
			// write(jpg_fd, buffers[v4lbuf.index].start, buffers[v4lbuf.index].length);
			// close(jpg_fd);
			
			//映射的起始地址由x，y确定
			lcd_draw_video(0, 0, buffers[v4lbuf.index].start, buffers[v4lbuf.index].length);
			
			//将缓冲区重新入队尾
			if(ioctl(cam_fd , VIDIOC_QBUF, &v4lbuf))
			{
				perror("VIDIOC_QBUF");
				exit(0);
			}
		}	
	}
	
	//关闭摄像头数据采集
	vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if(ioctl (cam_fd, VIDIOC_STREAMOFF, &vtype))
	{
		perror("VIDIOC_STREAMOFF");
		exit(0);
	}
	
	//解除用户空间映射
	for(i=0; i<reqbuf.count; i++)
	{
		munmap (buffers[i].start, buffers[i].length);
	}
	
	//关闭摄像头文件
	close(cam_fd);
	close(lcd_fd);
}