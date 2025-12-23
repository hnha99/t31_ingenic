/**
 * @file speaker.h
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __SPEAKER_MODE_H__
#define __SPEAKER_MODE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>

#define SPEAKER_CTL_MODE0 _IOW('a','0',int32_t*)
#define SPEAKER_CTL_MODE1 _IOW('a','1',int32_t*)
#define SPEAKER_CTL_MODE2 _IOW('a','2',int32_t*)
#define SPEAKER_CTL_MODE3 _IOW('a','3',int32_t*)
#define SPEAKER_CTL_MODE4 _IOW('a','4',int32_t*)

void speaker_mode(unsigned long MODE);

#endif /* __SPEAKER_MODE_H__ */