//
// Created by Mangata on 2021/12/23.
//
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "RFID.h"



char Get_BCC(char *SerBfr){
    char BCC = 0;
    for(int i=0; i<(SerBfr[0]-2); i++) {
        BCC ^= SerBfr[i];
    }
    return (~BCC);
}

void get_Sjz(char *SerBfr){//数据帧
    SerBfr[0] = 0x07;
    SerBfr[1] = 0x02;
    SerBfr[2] = 0x41;
    SerBfr[3] = 0x01;
    SerBfr[4] = 0x52;
    char BCC = 0;
    for(int i=0; i<(SerBfr[0]-2); i++) {
        BCC ^= SerBfr[i];
    }
    SerBfr[5] = ~BCC;
    SerBfr[6] = 0x03;
}
void get_Fpz(char *fpz){//防碰撞协议
    fpz[0] = 0x08;
    fpz[1] = 0x02;
    fpz[2] = 0x42;
    fpz[3] = 0x02;
    fpz[4] = 0x93;
    fpz[5] = 0x00;
    char BCC = 0;
    for(int i=0; i<(fpz[0]-2); i++) {
        BCC ^= fpz[i];
    }
    fpz[6] = ~BCC;
    fpz[7] = 0x03;
}
