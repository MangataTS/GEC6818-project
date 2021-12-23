//
// Created by Mangata on 2021/12/23.
//

#ifndef UNTITLED_UARTDH11_H
#define UNTITLED_UARTDH11_H
/* 头文件 */
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


extern char WsdString[20];
int set_serial_uart(int ser_fd);
void *funcTH(void *arg);


#endif //UNTITLED_UARTDH11_H
