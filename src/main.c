#include <stdio.h>
#include <pthread.h>
#include "picture.h"
#include "getxy.h"
#include "font.h"
#include "UARTdh11.h"
#include "RFID.h"
#include <stdbool.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define true 1
#define false 0
#define PASSLEN 4
//************素材内容************
char *number[]={//数字
        "sucai/0.bmp",
        "sucai/1.bmp",
        "sucai/2.bmp",
        "sucai/3.bmp",
        "sucai/4.bmp",
        "sucai/5.bmp",
        "sucai/6.bmp",
        "sucai/7.bmp",
        "sucai/8.bmp",
        "sucai/9.bmp",
};
char *del="sucai/del.bmp";
char *login="sucai/login.bmp";
char *background="sucai/back.bmp";
char *whiteblock="sucai/whiteblock.bmp";
char *shownumbers="sucai/shownumbers.bmp";
char *Error_login="sucai/errorlogin.bmp";
char *Login_Logo="sucai/login_logo.bmp";
char *NC[]={
        "sucai/CMWZ.bmp",
        "sucai/FMYZC.bmp",
        "sucai/KKNC.bmp",
        "sucai/LMBH.bmp"
};
char *JGB="sucai/JGB.bmp";
char *XIADAN="sucai/XDCN.bmp";
//************素材内容************
int locpass=0;
int tolid = 0;
struct Good{
    int value;
    int number;
    char name[20];
};
int IDok;	//身份识别默认失败 0失败 1成功
char a[20][40];
extern char WsdString[20];

void *Pcamera(void *arg);

#if 1
void Message_init(){
    FILE *fp_message = fopen("Message_table.txt","r");
    if(fp_message == NULL) {
        fp_message = fopen("Message_table.txt","w");
        fclose(fp_message);
        return;
    }
    char buf[40];
    while(fgets(buf,40,fp_message)){//读取这些订单信息
       strcpy(a[tolid++],buf);
       tolid %= 10;//限制读取十条
    }
    fclose(fp_message);
}

void show_Main(){//显示主界面
    Init_Font();
    Display_characterX(350,0,"全全奶茶",0x00D2691E,3);
    UnInit_Font();
    show_anybmp(background,0,0);//背景
    show_anybmp(Login_Logo,90,90);//logo
    //显示数字键盘
    for(int x = 310,i = 1;i <= 3; ++i,x += 60){
        for(int y = 240,j = 1;j <= 3; ++j,y += 60){
            show_anybmp(number[(i-1)*3+j],x,y);
        }
    }
    show_anybmp(number[0],360,420);//数字0
    show_anybmp(del,590,240);//删除键
    show_anybmp(login,590,340);//登陆键
    show_anybmp(shownumbers,280,90);//显示输入的数字
}
void show_Right(struct Good *goods){//显示密码通过后的界面
    Init_Font();
    Clean_Area(0, 0,  800, 480, 0x00FFFFFF);
    printf("YES2\n");
    show_anybmp(NC[0],250,0);
    show_anybmp(NC[1],500,0);
    show_anybmp(NC[2],250,240);
    show_anybmp(NC[3],500,240);
    show_anybmp(JGB,700,0);
    //商品的左上角显示商品的数量
    char num[10];
    Clean_Area(250,0,20,20,0x00FF0000);
    sprintf(num,"%d",goods[0].number);
    Display_characterX(250,0,num,0x00FFFFFF,1);

    Clean_Area(500,0,20,20,0x00FF0000);
    sprintf(num,"%d",goods[1].number);
    Display_characterX(500,0,num,0x00FFFFFF,1);

    Clean_Area(250,240,20,20,0x00FF0000);
    sprintf(num,"%d",goods[2].number);
    Display_characterX(250,240,num,0x00FFFFFF,1);

    Clean_Area(500,240,20,20,0x00FF0000);
    sprintf(num,"%d",goods[3].number);
    Display_characterX(500,240,num,0x00FFFFFF,1);

    //商品的左上角显示商品的数量
    //logo?
    int sum_number = 0;
    int sum_value = 0;
    for(int i = 0;i < 4; ++i){
        sum_number += goods[i].number;//总数量
        sum_value += goods[i].value * goods[i].number;//总价值
    }
//    Display_characterX(10,10,"温湿度：",0x00D2691E,2);
    Display_characterX(10,90,WsdString,0x00D2691E,2);

    Display_characterX(10,300,"数量：",0x00D2691E,2);
    sprintf(num,"%d",sum_number);
    Display_characterX(120,300,num,0x00D2691E,2);

    Display_characterX(10,350,"共计：",0x00D2691E,2);
    sprintf(num,"%d￥",sum_value);
    Display_characterX(120,350,num,0x00D2691E,2);

    Clean_Area(0,0,250,80,0x00FFD700);//返回按钮
    Display_characterX(65,20,"返回",0x00FFFFFF,3);
    Clean_Area(0,400,250,80,0x00FFD700);//确认按钮
    Display_characterX(10,420,"确定下单",0x00FFFFFF,3);

    //数量
//  左上角的商品
    Clean_Area(250, 200,  199, 40, 0x00FFF0F5);
    Display_characterX(250,180,"-",0x00D2691E,5);
    Display_characterX(350,180,"+",0x00D2691E,5);
//  右上角的商品
    Clean_Area(500, 200,  199, 40, 0x00FFF0F5);
    Display_characterX(500,180,"-",0x00D2691E,5);
    Display_characterX(600,180,"+",0x00D2691E,5);
//  左下角的商品
    Clean_Area(250, 440,  199, 40, 0x00FFF0F5);
    Display_characterX(250,420,"-",0x00D2691E,5);
    Display_characterX(350,420,"+",0x00D2691E,5);
//  右下角的商品
    Clean_Area(500, 440,  199, 40, 0x00FFF0F5);
    Display_characterX(500,420,"-",0x00D2691E,5);
    Display_characterX(600,420,"+",0x00D2691E,5);
    UnInit_Font();
}
void Faild_login(){
    show_anybmp(Error_login,100,90);
    sleep(1);//睡1s
    show_Main();//清空主界面
    locpass = 0;//将密码指针移到开头
}
//密码线程
int is_in(int x,int y,int startx,int starty,int endx,int endy){//判断触摸点是否在这个点内
    if(x >= startx && x <= endx && y >= starty && y<= endy){//如果在这个范围内返回1
        return 1;
    }
    return 0;
}

void *Ppass(void *arg)
{
    char password[4];
    char *banzipassword="4399";//内置密码
    while(!IDok)
    {
//        printf("(%d,%d)\n",x,y);
        if(x == -1 || y == -1) continue;
        if(x > 590 && x < 690 && y > 240 && y < 300){//点击删除框is_in(x,y,590,240,690,300)
            if(locpass == 0) {//表示密码框为空
                printf("密码框为空！\n");
            }
            else{//删除操作
                locpass--;
                show_anybmp(whiteblock,280+60*locpass,90);//覆盖前面一个位置
            }
            x = y = 0;
        }
        else if(x > 590 && x < 690 && y > 340 && y < 400){//点击登陆框is_in(x,y,590,340,690,400)
            if(locpass != 4){
                printf("密码框未输满！\n");
            }
            else{
                int fg = true;
                for(int i = 0;i < PASSLEN; ++i) {
                    if(password[i] != banzipassword[i]){
                        fg = false;
                    }
                }
                if(fg) {//如果对了,那么显示奖励图片
                    printf("密码正确！\n");
                    x = y = 0;
                    IDok = 1;
                    printf("通过密码认证！\n");
                }
                else {//如果错了
                    printf("密码错误！\n");
                    Faild_login();
                }
            }
            x = y = -1;
        }
        else{
            if(locpass == 4) {//密码框已满
                printf("密码框已满！\n");
            }
            else{//点击数字框
                int locnumber;
                for(int XX = 310,i = 1;i <= 3; ++i,XX += 60){
                    for(int YY = 240,j = 1;j <= 3; ++j,YY += 60){
                        if(x > XX && x < (XX + 60) && y > YY && y < (YY + 60)){//is_in(x,y,XX,YY,XX+60,YY+60)
                            locnumber = (i-1)*3+j;
                            show_anybmp(number[locnumber],280+locpass*60,90);//显示输入的数字
                            password[locpass++] = '0'+locnumber;
                        }
                    }
                }
                if(x > 360 && x < 420 && y > 420 && y < 480) {//数字0  is_in(x,y,360,420,420,480)
                    locnumber = 0;
                    show_anybmp(number[locnumber], 280 + locpass * 60, 90);
                    password[locpass++] = '0' + locnumber;
                }
            }
            x = y = -1;
        }
    }
    pthread_exit(NULL); //退出该线程
}

//RFID线程

void *Prfid(void *arg)
{
    //1、打开串口设备
    int fd = open("/dev/ttySAC1" , O_RDWR|O_NOCTTY);
    if(fd < 0)
    {
        perror ("open failed 1");
        exit(0);
    }
    printf("fd = %d\n",fd);
    //2、初始化串口
    set_serial_uart(fd);
    char request[8] = {0};
    get_Sjz(request);
    char fpz[9] = {0};
    get_Fpz(fpz);
    char buf[10] = {0};
    int ret;
    printf("[%d]\n", __LINE__);
    while(!IDok)
    {
        printf("[%d]\n", __LINE__);
        ret = write(fd, request, 7);
        printf("write ret = %d\n",ret);
        for(int i = 0;i < 7; ++i) {
            printf("%x",request[i]);
        }
        putchar('\n');
        ret = read(fd, buf, 10);
        printf("read ret = %d\n",ret);
        printf("buf[2] = %x\n",buf[2]);
        printf("[%d]\n", __LINE__);
        if(ret > 0)
        {
            if(buf[2] == 0)
            {
                printf("请求成功\n");
                write(fd, fpz, 8);
                ret = read(fd, buf, 10);
                if(ret > 0)
                {
                    printf("防碰撞\n");
                    if(buf[0] == 0x0a && buf[2] == 0x00){
                        int cardid = (buf[7]<<24) | (buf[6] << 16) | (buf[5] << 8) | buf[4];
                        if(cardid == 0x3D458719){
                            IDok = 1;
                            break;
                        }
                        printf("OX%X\n",cardid);
                        printf("CARDID = 0x%x 0x%x 0x%x 0x%x\n",buf[7],buf[6],buf[5],buf[4]);
                        sleep(2);
                    }
                }
            }
            else
            {
                printf("请求失败\n");
            }
        }
    }
    printf("exit RFID*****************\n");
    pthread_exit(NULL);	//退出该线程
}

void submit_table(struct Good *goods){//提交页面
    show_anybmp(XIADAN,200,40);//显示下单成功的图片
    FILE* fd_table = fopen("Message_table.txt","w");
    char buf[40];
    memset(buf,0,sizeof buf);
    int value = 0;
    for(int i = 0;i < 4; ++i) value += goods[i].value * goods[i].number;
    time_t timep;
    struct tm *p;
    time(&timep);
    p = gmtime(&timep);
    char TIM[20];
    sprintf(TIM,"date:%d-%d-%d ",(int)(1900+p->tm_year),(int)(1+p->tm_mon),(int)p->tm_mday);
    strcat(buf,TIM);
    char HF[20];
    sprintf(HF,"cost: %d",value);
    strcat(buf,HF);
    printf("buf = %s \ntolid = %d\n",buf,tolid);
    strcpy(a[tolid++],buf);
    tolid%=10;
    for(int i = 0;i < 10; ++i) {
        if(strcmp(a[i],""))
            fprintf(fd_table,"%s\n",a[i]);
    }
    fclose(fd_table);
    sleep(1);//暂停1s
}

void histoy_submit_table() {
    Init_Font();
    Display_characterX(10,30,WsdString,0x00D2691E,2);
    Clean_Area(0, 0,  800, 480, 0x00FFE4E1);//刷新背景
    Display_characterX(280,0,"历史订单",0x00D2691E,4);
    Clean_Area(700, 0, 100, 80, 0x00DCDCDC);//返回框
    Display_characterX(690,15,"返回",0x00D2691E,3);
    //这个订单是左侧的五个
    int cnt = 0;
    for(int i = 0;i < 5; ++i) {
        if(strcmp(a[i],"\n") == 0 || strcmp(a[i],"") == 0) continue;
        a[i][strlen(a[i])-1] ='\0';
        Display_characterX(0,80 + cnt * 80,a[i],0x00FFFFFF,2);
        cnt++;
    }
    for(int i = 5;i < 10; ++i) {
        a[i][strlen(a[i])-1] ='\0';
        if(strcmp(a[i],"\n") == 0 || strcmp(a[i],"") == 0) continue;
        if(cnt < 5) {
            Display_characterX(0,80 + cnt * 80,a[i],0x00FFFFFF,2);
        }
        else {
            Display_characterX(400,80 + (cnt-5) * 80,a[i],0x00FFFFFF,2);
        }
        cnt++;
    }
    while(1){
        Clean_Area(0, 0,  200, 80, 0x00FFE4E1);//刷新背景
        Display_characterX(10,30,WsdString,0x00D2691E,2);
        if(x > 700 && x < 800 && y > 0 && y < 80) {//返回框
            UnInit_Font();
            return;
        }
    }
}

void show_check(){
    Init_Font();
    Clean_Area(0, 0,  400, 480, 0x00FFA500);//左侧点单
    Display_characterX(50,200,"即刻点单",0x00FF4500,4);
    Clean_Area(400, 0,  400, 480, 0x00AFEEEE);//右侧历史订单
    Display_characterX(450,200,"历史订单",0x00FF4500,4);
    UnInit_Font();
}

#endif

int main(int argc, char *argv[])
{
    IDok = 0;
    //初始化设备文件
    Message_init();//历史订单
    lcd_init();		//液晶屏
    ts_init();		//触摸屏
    //开启坐标获取线程
    pthread_t coordTid;
    pthread_create(&coordTid, NULL, getxy, NULL);	//实时获取坐标
    show_Main();
    //身份认证：密码识别或者RFID
    pthread_t passTid, rfidTid, cameraTid, dht11Tid;
    pthread_create(&passTid, NULL, Ppass, NULL);		//创建密码线程
    pthread_create(&rfidTid, NULL, Prfid, NULL);		//创建rfid线程
    pthread_create(&dht11Tid, NULL, funcTH, NULL);		//创建dht11线程->温湿度
    while(!IDok);	//直到身份认证成功
    show_check();
    while(1) {
        Init_Font();
        Clean_Area(0, 0,  200, 80, 0x00FFA500);//温湿度
        Display_characterX(10,10,WsdString,0x00D2691E,2);
        UnInit_Font();
        if(x > 0 && x < 400 && y > 0 && y < 480) {//左侧进入点单
            x = y = -1;
            struct Good G[4];
            memset(G,0,sizeof G);
            for(int i = 0;i < 4; ++i) G[i].number = 0;
            G[0].value = 13;//草莓丸子
            G[1].value = 10;//蜂蜜柚子
            G[2].value = 12;//可可奶茶
            G[3].value = 11;//柠檬薄荷
            show_Right(G);
            while(1) {
                Init_Font();
                Clean_Area(0, 80,  200, 80, 0x00FFFFFF);//左侧点单
                Display_characterX(10,90,WsdString,0x00D2691E,2);
                UnInit_Font();
                if(x > 250 && x < 350 && y > 200 && y < 240) {//左上角的删除
                    printf("LEFT_UP_DEL\n");
                    if(G[0].number)
                        G[0].number--;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 350 && x < 450 && y > 200 && y < 240) {//左上角的增加
                    printf("LEFT_UP_ADD\n");
                    G[0].number++;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 500 && x < 600 && y > 200 && y < 240) {//右上角的删除
                    printf("RIGHT_UP_DEL\n");
                    if(G[1].number)
                        G[1].number--;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 600 && x < 700 && y > 200 && y < 240) {//右上角的增加
                    printf("RIGHT_UP_ADD\n");
                    G[1].number++;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 250 && x < 350 && y > 440 && y < 480) {//左下角的删除
                    printf("LEFT_DOWN_DEL\n");
                    if(G[2].number)
                        G[2].number--;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 350 && x < 450 && y > 440 && y < 480) {//左下脚的增加
                    printf("LEFT_DOWN_ADD\n");
                    G[2].number++;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 500 && x < 600 && y > 440 && y  < 480) {//右下角的删除
                    printf("LEFT_DOWN_DEL\n");
                    if(G[3].number)
                        G[3].number--;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 600 && x < 700 && y > 440 && y < 480) {//右下角的增加
                    printf("RIGHT_DOWN_ADD\n");
                    G[3].number++;
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 0 && x < 250 && y > 400 && y < 480) {//提交订单的需求
                    printf("SUBMIT_S\n");
                    if(G[0].number + G[1].number + G[2].number + G[3].number == 0){//如果没有选中，那么就不能提交
                        x = y = -1;
                        continue;
                    }
                    submit_table(G);
                    show_Right(G);
                    x = y = -1;
                }
                else if(x > 0 && x < 250 && y > 0 && y < 80) {//返回上一个界面
                    x = y = -1;
                    show_check();
                    break;
                }
            }
            x = y = -1;
        }
        else if(x > 400 && x < 800 && y > 0 && y < 480){//进入历史订单
            x = y = -1;
            histoy_submit_table();
            x = y = -1;
            show_check();
        }

    }
    //资源释放
    lcd_close();	//液晶屏
    ts_close();		//触摸屏
    return 0;
}