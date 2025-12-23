/**
 * @file speaker.c
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "speaker.h"

void speaker_mode(unsigned long MODE)
{
        int fd;
        fd = open("/dev/speaker_custom", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
        }
        ioctl(fd, MODE); 
        close(fd);
}