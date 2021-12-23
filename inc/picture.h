#ifndef _ANYBMP_H_	//如果没有定义_ANYBMP_H_，那么就定义
#define _ANYBMP_H_

/* 头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <jconfig.h>
#include <jerror.h>
// #include <jmorecfg.h>
#include <jpeglib.h>




/* 宏定义 */
#define LCD "/dev/fb0"


/* 全局变量 */
extern int lcd_fd;	//声明液晶屏文件描述符
extern int *p;	//声明显存映射指针
extern unsigned char g_color_buf[800*480*3];

/* 函数声明 */
void lcd_init(void);
void lcd_close(void);
void lcd_one_pixel(int x, int y, int color);
void show_anybmp(char *bmpname, int x0, int y0);
unsigned long file_size_get(const char *pfile_path);
int lcd_draw_jpg(unsigned int x,unsigned int y,const char *pjpg_path);


#endif