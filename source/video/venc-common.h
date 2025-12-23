/**
 * @file venc-common.h
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __SAMPLE_COMMON_H__
#define __SAMPLE_COMMON_H__


#include <imp_common.h>
#include <imp_osd.h>
#include <imp_framesource.h>
#include <imp_isp.h>
#include <imp_encoder.h>

/* usr inc */
#include "qrcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* __cplusplus */

#define SENSOR_FRAME_RATE_NUM 25
#define SENSOR_FRAME_RATE_DEN 1
#define SENSOR_SNAP_WIDTH 1920
#define SENSOR_SNAP_HEIGHT 1080


#define SENSOR_GC2053
// #define SENSOR_GC4653

#if defined SENSOR_AR0141
#define SENSOR_NAME "ar0141"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x10
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 720
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_OV7725
#define SENSOR_NAME "ov7725"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x21
#define SENSOR_WIDTH 640
#define SENSOR_HEIGHT 480
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 0
#elif defined SENSOR_OV9732
#define SENSOR_NAME "ov9732"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x36
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 720
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_OV9750
#define SENSOR_NAME "ov9750"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x36
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 720
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_OV9712
#define SENSOR_NAME "ov9712"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x30
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 720
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_GC1004
#define SENSOR_NAME "gc1004"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x3c
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 720
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_JXH42
#define SENSOR_NAME "jxh42"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x30
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 720
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_SC1035
#define SENSOR_NAME "sc1035"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x30
#define SENSOR_WIDTH 1280
#define SENSOR_HEIGHT 960
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_OV2710
#define SENSOR_NAME "ov2710"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x36
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_OV2735
#define SENSOR_NAME "ov2735"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x3c
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1

#elif defined SENSOR_OV2735B
#define SENSOR_NAME "ov2735b"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x3c
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1

#elif defined SENSOR_SC2135
#define SENSOR_NAME "sc2135"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x30
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1
#elif defined SENSOR_JXF22
#define SENSOR_NAME "jxf22"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x40
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1

#elif defined SENSOR_JXF23
#define SENSOR_NAME "jxf23"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x40
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1

#elif defined SENSOR_JXF28
#define SENSOR_NAME "jxf28"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x40
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 1
#define CROP_EN 1

#elif defined SENSOR_GC2053
#define SENSOR_NAME "gc2053"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x37
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 1
#define CHN2_EN 1
#define CHN3_EN 0
#define CROP_EN 1

#elif defined SENSOR_OV4689
#define SENSOR_NAME "ov4689"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x36
#define SENSOR_WIDTH 2048
#define SENSOR_HEIGHT 1520
#define CHN0_EN 1
#define CHN1_EN 0
#define CHN2_EN 0
#define CHN3_EN 0
#define CROP_EN 1

#elif defined SENSOR_GC4653
#define SENSOR_NAME "gc4653"
#define SENSOR_CUBS_TYPE TX_SENSOR_CONTROL_INTERFACE_I2C
#define SENSOR_I2C_ADDR 0x29
#define SENSOR_WIDTH 1920
#define SENSOR_HEIGHT 1080
#define CHN0_EN 1
#define CHN1_EN 1
#define CHN2_EN 1
#define CHN3_EN 1
#define CROP_EN 1


#endif

#define STREAM_FILE_PATH_PREFIX "/tmp/t31-ingenic/nhanh7/t31_ingenic"
// #define STREAM_FILE_PATH_PREFIX "/tmp"
#define SNAP_FILE_PATH_PREFIX "/tmp/t31-ingenic/nhanh7/t31_ingenic"

#define CHANNEL_0 0
#define CHANNEL_1 1
#define CHANNEL_2 2
#define CHANNEL_3 3
#define CHANNEL_ENABLE 1
#define CHANNEL_DISABLE 0

#define GET_VALUE_CHN_0 0
#define GET_VALUE_CHN_1 1
#define START_CHN_STREAM 2
#define STOP_CHN_STREAM 3

#define CH_NUM 	4
#define FPS 	25
#define BITRATE	1500

	struct chn_conf
	{
		unsigned int index;
		unsigned int enable;
		IMPEncoderProfile payloadType;
		IMPFSChnAttr fs_chn_attr;
		IMPCell framesource_chn;
		IMPCell imp_encoder;
	};

	struct venc_config
	{
		int fps;
		int outBitRate;
	};

	typedef struct
	{
		int width;
		int height;
	} size_picture_t;

	typedef enum resolution_type
	{
		VENC_RESOLUTION_VGA,
		VENC_RESOLUTION_720P,
		VENC_RESOLUTION_1080P,
		VENC_RESOLUTION_NONE,
	} resolution_type_t;

	typedef struct
	{
		bool b_mode;
		bool b_checkEvt;
		bool b_stream;
		bool b_osd;
		bool b_getevt;
	}pthread_config_t;
	pthread_config_t pthread;
	

	/* Variable */
	IMPEncoderChnAttr encchn_attr[CH_NUM];
	IMPFSChnAttr fschn_attr[CH_NUM];
	pthread_t thread_id[CH_NUM];
	struct venc_config venc_config_t;

	extern size_picture_t size_pic[];
	extern struct chn_conf channel_num[];

	int venc_system_init();
	int venc_system_exit();

	int venc_framesource_init(int chn);
	int venc_framesource_exit(int chn, bool destroy_channel);

	int venc_encoder_setParam_Default(IMPEncoderChnAttr *channel_attr, int chn, size_picture_t size, int fps, int BitRate);
	int venc_encoder_init(int chn, IMPEncoderChnAttr channel_attr, bool create_group);
	int venc_encoder_exit(int chn, bool destroy_group);

	int venc_jpeg_init(int chn, bool create_group);
	int venc_jpeg_exit(int chn);

	int venc_framesource_streamon(int chn);
	int venc_framesource_streamoff(int chn);

	int save_stream(FILE *fd, IMPEncoderStream *stream);
	void *get_video_stream(void *args);
	int venc_get_video_stream();
	char *venc_get_jpeg_snap(int chn);

	int venc_startStream(int chn);
	int venc_stopStream(int chn);

	int venc_SetIRCUT(int enable);
	void *get_photosensitive(void *p);

	/* Start, stop stream */
	int venc_stream_init();
	int venc_stream_exit();
	int venc_snapJPEG_init();
	int venc_snapJPEG_exit();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SAMPLE_COMMON_H__ */
