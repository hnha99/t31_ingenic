#ifndef __RTSPD_SERVER_H
#define __RTSPD_SERVER_H

#include <stdio.h>

#define STREAM_H264 "stream-H264"
#define STREAM_H265 "stream-H265"
#define INPUT_H264 "/tmp/h264_fifo"
#define INPUT_H265 "/tmp/h265_fifo"

int rtspd_h264(char const *streamName, char const *inputFileName);
int rtspd_h265(char const *streamName, char const *inputFileName);
int rtspd_creatFifo();
int stop_rtspd();
int rtspd_startH264();
int rtspd_startH265();
int rtspd_startGetStream();

#endif // __RTSPD_SERVER_H
