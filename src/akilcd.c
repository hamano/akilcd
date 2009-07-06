/*
 * akicd - Akizuki Graphic LCD Writer
 * Copyright (C) 2005 Tsukasa Hamano <hamano@klab.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define BUF_SIZE 256

static char lcdbuf[4][21];
static int lcdpos = 0;

void lcd_write(int fd, char *buf, size_t len)
{
    size_t i;
    for(i=0; i<len; i++){
        write(fd, buf + i, 1);
        usleep(1000);
    }
}

void lcd_clear(int fd)
{
    char c;
    int ret;
    lcd_write(fd, "\033E00\r", 5);
    ret = read(fd, &c, 1);
    //printf("clear ret: %d, c: %c\n", ret, c);
}

void lcd_print(int fd, const char *buf)
{
    int i;
    size_t len;

    len = strlen(buf);
    for(i=0; i<len; i+=20){
        strncpy(lcdbuf[lcdpos++], buf + i, 20);
        if(lcdpos >= 4){
            lcdpos = 0;
        }
    }
}

void lcd_pos(int fd, int y)
{
    char c;
    int ret;
    char cmd[8];
    snprintf(cmd, 8, "\033N000%c\r", '0' + y);
    lcd_write(fd, cmd, 7);
    ret = read(fd, &c, 1);
    //printf("pos ret: %d, c: %c\n", ret, c);
}

void lcd_flush(int fd)
{
    int i;
    for(i=0; i<4; i++){
        lcd_pos(fd, i);
        lcd_write(fd, lcdbuf[(lcdpos+i)%4], 20);
        //printf("%s\n", lcdbuf[(lcdpos+i)%4]);
    }
}

int main(int argc, char *argv[]){
    char *dev;
    int fd;
    struct termios term;
    char buf[BUF_SIZE];

    if(argc < 2){
        fprintf(stderr, "too few arguments\n");
        return EXIT_FAILURE;
    }
    dev = argv[1];
    //printf("opening %s\n", dev);
    fd = open(dev, O_RDWR | O_SYNC);
    if(fd < 0){
        fprintf(stderr, "cannot open device\n");
        return EXIT_FAILURE;
    }

    tcgetattr(fd, &term);
    cfsetspeed(&term, B38400);
    cfmakeraw(&term);
    tcsetattr(fd, TCSANOW, &term);
    tcflush(fd, TCIOFLUSH);
    lcd_clear(fd);

    while(fgets(buf, BUF_SIZE - 1, stdin)){
        lcd_print(fd, buf);
        lcd_flush(fd);
    }
    close(fd);
    return EXIT_SUCCESS;
}
