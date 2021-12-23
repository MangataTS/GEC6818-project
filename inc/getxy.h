#ifndef _GETXY_H_	//如果没有定义_ANYBMP_H_，那么就定义
#define _GETXY_H_

/* 头文件 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>	//输入事件的头文件
#include <stdlib.h>


/* 宏定义 */
#define TS "/dev/input/event0"


/* 全局变量 */
extern int ts_fd;	//声明触摸屏文件描述符
extern int x;	//声明坐标x
extern int y;	//声明坐标y



/* 函数声明 */
void ts_init(void);
void ts_close(void);
void *getxy(void *arg);



#endif