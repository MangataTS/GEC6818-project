//
// Created by Mangata on 2021/12/23.
//

#ifndef UNTITLED_RFID_H
#define UNTITLED_RFID_H
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
//#include "UARTdh11.h"

char Get_BCC(char *SerBfr);
void get_Fpz(char *fpz);
void get_Sjz(char *SerBfr);

#endif //UNTITLED_RFID_H
