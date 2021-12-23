#include "picture.h"

 
unsigned char g_color_buf[800*480*3]={0};//像素点空间缓存
int lcd_fd = -1;	//液晶屏文件描述符
int *p = NULL;	//显存映射指针


/* 液晶屏初始化 */
void lcd_init(void)
{
	lcd_fd = open(LCD, O_RDWR);	//以读写权限打开液晶屏
	if(lcd_fd == -1)
	{
		perror("open lcd failed");
		exit(0);	//结束程序
	}
	p = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, lcd_fd, 0);	//给液晶屏映射一块内存
	if(p == MAP_FAILED)	//是否映射失败
	{
		perror("mmap failed");
		exit(0);	//结束程序
	}
}


/* 资源释放 */
void lcd_close(void)
{
	munmap(p, 800*480*4);
	close(lcd_fd);
}

//给一个像素点上色
void lcd_one_pixel(int x, int y, int color)
{
	*(p+y*800+x) = color;	//给每个地址解引用赋值颜色
}


/* 显示任意大小任意位置图片 */
//x0和y0是液晶屏起始映射点
void show_anybmp(char *bmpname, int x0, int y0)
{
	if(bmpname == NULL)
	{
		printf("BMP文件名为空\n");
		exit(0);
	}
	//打开bmp图片
	int bmp_fd = open(bmpname, O_RDONLY);
	if(bmp_fd == -1)
	{
		perror("open bmp failed");
		exit(0);	//结束程序
	}
	
	//读取头信息
	char bmp_info[54] = {0};
	read(bmp_fd, bmp_info, 54);
	int w = bmp_info[18] | bmp_info[19]<<8;	//获取图片像素宽
	int h = bmp_info[22] | bmp_info[23]<<8;	//获取图片像素高
	printf("bmp:%s, w=%d, h=%d\n", bmpname, w, h);
	if(w+x0>800 || h+y0>480)
	{
		printf("超出屏幕范围\n");
		exit(0);
	}
	
	//读取RGB数据
	char bmp_buf[800*480*3] = {0};
	int color, i=0;
	read(bmp_fd, bmp_buf, 800*480*3);
	for(int y=0; y<h; y++)
	{
		for(int x=0; x<w; x++)
		{
			color = bmp_buf[i+0] | bmp_buf[i+1]<<8 | bmp_buf[i+2]<<16;	//24位转32位，从lcd的最后一行开始存储
			i += 3; //偏移下一个像素点
			lcd_one_pixel(x+x0, h-1-y+y0, color);	//映射每一个像素点
		}
	}
	
	//关闭文件
	close(bmp_fd);
}




//获取jpg文件的大小
unsigned long file_size_get(const char *pfile_path)
{
	unsigned long filesize = -1;	
	struct stat statbuff;//文件属性结构体
	
	if(stat(pfile_path, &statbuff) < 0)
	{
		return filesize;   //获取文件属性失败
	}
	else
	{
		filesize = statbuff.st_size;//获取到文件大小
	}
// printf("line:%d\n", __LINE__);
	return filesize;
}

//显示正常jpg图片
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path)  
{
	// printf("line:%d\n", __LINE__);
	/*定义解码对象，错误处理对象*/
	struct 	jpeg_decompress_struct 	cinfo;  //解压缩的参数
	struct 	jpeg_error_mgr 			jerr;	 //错误处理追踪
	
	unsigned char 	*pcolor_buf = g_color_buf;//*（pcolor_buf+0）== g_color_buf[0]
	char 	*pjpg;  //申请文件大小的缓冲区
	
	unsigned int 	i=0;
	unsigned int	color =0;
	
	unsigned int 	x_s = x;//x的映射起始位置

	unsigned int	y_n	= y;//x和y的映射的起始位置
	unsigned int	x_n	= x;
	
			 int	jpg_fd;
	unsigned int 	jpg_size;

	if(pjpg_path!=NULL)//如果传进来的是空字符串，就退出
	{
		/* 申请jpg资源，权限可读可写 */	
		jpg_fd=open(pjpg_path,O_RDWR);
		
		if(jpg_fd == -1)
		{
		   printf("open %s error\n",pjpg_path);
		   
		   return -1;	
		}	
		
		/* 获取jpg文件的大小 */
		jpg_size=file_size_get(pjpg_path);	
		if(jpg_size<3000)
			return -1;
		
		/* 为jpg文件申请内存空间 */	
		pjpg = malloc(jpg_size);
// printf("line:%d\n", __LINE__);
		/* 读取jpg文件所有内容到内存 */		
		read(jpg_fd,pjpg,jpg_size);
	}
	else
		return -1;

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

	if(pjpg_path!=NULL)
	{
		/* 关闭jpg文件 */
		close(jpg_fd);	
		
		/* 释放jpg文件内存空间 */
		free(pjpg);		
	}

	return 0;
}











