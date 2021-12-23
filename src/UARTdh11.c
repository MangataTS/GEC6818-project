#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


//定义两个串口描述符
char WsdString[20]={0};

int set_serial_uart(int ser_fd)
{
	struct termios new_cfg,old_cfg;
		
	/*保存并测试现有串口参数设置，在这里如果串口号等出错，会有相关的出错信息*/ 
	if	(tcgetattr(ser_fd, &old_cfg) != 0)
	{
		perror("tcgetattr");
		return -1;
	}
	
	bzero( &new_cfg, sizeof(new_cfg));
	/*原始模式*/
	/* 设置字符大小*/
	new_cfg = old_cfg; 
	cfmakeraw(&new_cfg); /* 配置为原始模式 */ 

	/*波特率为115200*/
	cfsetispeed(&new_cfg, B115200); 
	cfsetospeed(&new_cfg, B115200);
	
	new_cfg.c_cflag |= CLOCAL | CREAD;
	
	/*8位数据位*/
	new_cfg.c_cflag &= ~CSIZE;
	new_cfg.c_cflag |= CS8;

	/*无奇偶校验位*/
	new_cfg.c_cflag &= ~PARENB;

	/*1位停止位*/
	new_cfg.c_cflag &= ~CSTOPB;
	/*清除串口缓冲区*/
	tcflush( ser_fd,TCIOFLUSH);
	new_cfg.c_cc[VTIME] = 0;
	new_cfg.c_cc[VMIN] = 1;
	tcflush ( ser_fd, TCIOFLUSH);
	/*串口设置使能*/
	tcsetattr( ser_fd ,TCSANOW,&new_cfg);
}
//dht11线程
void *funcTH(void *arg){
    int fd2=0;
    int ret;
    //打开串口12
    fd2 = open("/dev/ttySAC2" , O_RDWR|O_NOCTTY);
    if(fd2 < 0)
    {
        perror ("open failed 2");
        exit(0);
    }

    //初始化串口2
    set_serial_uart(fd2);
    char buf[20] = {0};
    while(1)
    {
        bzero(buf,20);
        int ret = read(fd2,buf,20);
//        printf("读到的字节数：%d\n温湿度：%s\nlen of buf[2] = %c \nbuf[3] = %c\nbuf[5] = %c\n",ret,buf,buf[2],buf[3],buf[5]);
        buf[8]='\0';
        if(buf[2]=='0' && buf[3]=='0' && buf[6] =='0' && buf[7]=='0'){
//            sleep(1);
            continue;
        }
        strcpy(WsdString,buf);
//        sleep(1);
    }
}

