#include "getxy.h"


int ts_fd = -1;
int x, y;	//坐标全局变量

void ts_init(void)
{
	ts_fd = open(TS, O_RDONLY);
	if(ts_fd == -1)
	{
		perror("open ts failed");
		exit(0);
	}
}

void ts_close(void)
{
	close(ts_fd);
}


/* 实时获取坐标 */
void *getxy(void *arg)
{
	//2、读取触摸屏数据、判断事件类型及动作
	struct input_event buf;	//输入事件结构体变量
	int tmpx, tmpy;
	// int x1=-1, y1=-1; //在屏幕范围之外
	while(1)
	{
		read(ts_fd, &buf, sizeof(buf));
		if(buf.type == EV_ABS)	//绝对事件
		{
			if(buf.code == ABS_X)	//x轴事件
			{
				// tmpx = buf.value;	//蓝色边框	
				tmpx = buf.value*800/1024;	//黑色边框	
			}
			else if(buf.code == ABS_Y)	//y轴事件
			{
				// tmpy = buf.value;	//蓝色边框	
				tmpy = buf.value*480/600;	//黑色边框	
			}
		}
		
		/* 保证坐标值在屏幕范围之内 */
		if(tmpx<0 || tmpx>800 )	//坐标值跑飞重读一次
			continue;
		else if(tmpy<0 || tmpy>480 )
			continue;
		if(buf.type == EV_KEY && buf.code == BTN_TOUCH)	//按压事件
		{
			if(buf.value == 0)
			{
				printf("松开\n");
				/* x 和 y 为全局变量*/
				x = tmpx;	
				y = tmpy;
				// break;
			}
			else
			{
				printf("按下\n");
			}
		}
		printf("getxy --- (%d, %d)\n", x, y);
	}
}
