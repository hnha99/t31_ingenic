/**
 * @file audio.h
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "speaker.h"

#define AO_TEST_SAMPLE_RATE 8000
#define AO_TEST_SAMPLE_TIME 20
#define AO_TEST_BUF_SIZE (AO_TEST_SAMPLE_RATE * sizeof(short) * AO_TEST_SAMPLE_TIME / 1000)
#define AO_WIFI_CONNECT             "/usr/share/notify/CN/connect_wifi_ok.wav"
#define AO_QRCODE_SCAN_OK           "/usr/share/notify/binbin.wav"
#define AO_WIFI_DISCONNECT          "/usr/share/notify/CN/user_need_check.wav"
// #define AO_WIFI_NOT_INTERNET        "/tmp/t31-ingenic/nhanh7/can_notconnect.wav"
#define AO_WIFI_NOT_INTERNET        "/tmp/can_notconnect.wav"
#define AO_NOT_SDCARD               "/tmp/check_sdcard.wav"


int audio_play(unsigned long mode, const char* audio_file);

#endif // __AUDIO_H__