/**
 * @file venc-common.c
 * @author nhanh7
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>

#include <imp_log.h>
#include <imp_common.h>
#include <imp_system.h>
#include <imp_framesource.h>
#include <imp_encoder.h>
#include <imp_isp.h>
#include <imp_osd.h>

#include "venc-common.h"
#include "qrcode.h"
#include "osd.h"

#define TAG "sample-common"

static const IMPEncoderRcMode S_RC_METHOD = IMP_ENC_RC_MODE_CAPPED_QUALITY;
extern int IMP_OSD_SetPoolSize(int size);
IMPSensorInfo sensor_info;

size_picture_t size_pic[] = {
	[VENC_RESOLUTION_VGA] = {640, 480},
	[VENC_RESOLUTION_720P] = {1280, 720},
	[VENC_RESOLUTION_1080P] = {1920, 1080}};

struct chn_conf channel_num[CH_NUM] = {
	{
		.index = CHANNEL_0,
		.enable = CHN0_EN,
		.payloadType = IMP_ENC_PROFILE_HEVC_MAIN, // h264
		.fs_chn_attr = {
			.pixFmt = PIX_FMT_NV12,
			.outFrmRateNum = SENSOR_FRAME_RATE_NUM,
			.outFrmRateDen = SENSOR_FRAME_RATE_DEN,
			.nrVBs = 2,
			.type = FS_PHY_CHANNEL,

			.picWidth = SENSOR_WIDTH,
			.picHeight = SENSOR_HEIGHT,
		},
		.framesource_chn = {DEV_ID_FS, CHANNEL_0, 0},
		.imp_encoder = {DEV_ID_ENC, CHANNEL_0, 0},
	},
	{
		.index = CHANNEL_1,
		.enable = CHN1_EN,
		.payloadType = IMP_ENC_PROFILE_AVC_MAIN, // h265
		.fs_chn_attr = {
			.pixFmt = PIX_FMT_NV12,
			.outFrmRateNum = SENSOR_FRAME_RATE_NUM,
			.outFrmRateDen = SENSOR_FRAME_RATE_DEN,
			.nrVBs = 2,
			.type = FS_PHY_CHANNEL,

			.picWidth = SENSOR_WIDTH,
			.picHeight = SENSOR_HEIGHT,
		},
		.framesource_chn = {DEV_ID_FS, CHANNEL_1, 0},
		.imp_encoder = {DEV_ID_ENC, CHANNEL_1, 0},
	},
	{
		.index = CHANNEL_2,
		.enable = CHN2_EN,
		.payloadType = IMP_ENC_PROFILE_AVC_MAIN,
		.fs_chn_attr = {
			.pixFmt = PIX_FMT_NV12,
			.outFrmRateNum = SENSOR_FRAME_RATE_NUM,
			.outFrmRateDen = SENSOR_FRAME_RATE_DEN,
			.nrVBs = 2,
			.type = FS_PHY_CHANNEL,

			.picWidth = SENSOR_SNAP_WIDTH,
			.picHeight = SENSOR_SNAP_HEIGHT,
		},
		.framesource_chn = {DEV_ID_FS, CHANNEL_2, 0},
		.imp_encoder = {DEV_ID_ENC, CHANNEL_2, 0},
	},
	{
		.index = CHANNEL_3,
		.enable = CHN3_EN,
		.payloadType = IMP_ENC_PROFILE_AVC_MAIN,
		.fs_chn_attr = {
			.pixFmt = PIX_FMT_NV12,
			.outFrmRateNum = SENSOR_FRAME_RATE_NUM,
			.outFrmRateDen = SENSOR_FRAME_RATE_DEN,
			.nrVBs = 2,
			.type = FS_PHY_CHANNEL,

			.picWidth = SENSOR_WIDTH,
			.picHeight = SENSOR_HEIGHT,
		},
		.framesource_chn = {DEV_ID_FS, CHANNEL_3, 0},
		.imp_encoder = {DEV_ID_ENC, CHANNEL_3, 0},
	}};

int venc_system_init()
{
	int ret = 0;

	IMP_OSD_SetPoolSize(512 * 1024);

	memset(&sensor_info, 0, sizeof(IMPSensorInfo));
	sensor_info.cbus_type = SENSOR_CUBS_TYPE;
	memcpy(sensor_info.i2c.type, SENSOR_NAME, sizeof(SENSOR_NAME));
	sensor_info.i2c.addr = SENSOR_I2C_ADDR;

	IMP_LOG_DBG(TAG, "sample_system_init start\n");

	ret = IMP_ISP_Open();
	if (ret < 0)
	{
		printf("failed to open ISP\n");
		return -1;
	}

	ret = IMP_ISP_AddSensor(&sensor_info);
	if (ret < 0)
	{
		printf("failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_EnableSensor();
	if (ret < 0)
	{
		printf("failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_System_Init();
	if (ret < 0)
	{
		printf("IMP_System_Init failed\n");
		return -1;
	}

	/* enable turning, to debug graphics */
	ret = IMP_ISP_EnableTuning();
	if (ret < 0)
	{
		printf("IMP_ISP_EnableTuning failed\n");
		return -1;
	}
	IMP_ISP_Tuning_SetContrast(128);
	IMP_ISP_Tuning_SetSharpness(128);
	IMP_ISP_Tuning_SetSaturation(128);
	IMP_ISP_Tuning_SetBrightness(128);
#if 1
	ret = IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY);
	if (ret < 0)
	{
		printf("failed to set running mode\n");
		return -1;
	}
	// IMP_ISP_Tuning_SetAntiFlickerAttr(IMPISP_ANTIFLICKER_60HZ);
	// if (ret < 0)
	// {
	// 	printf("failed to set IMP_ISP_Tuning_SetAntiFlickerAttr\n");
	// 	return -1;
	// }

#endif
#if 0
    ret = IMP_ISP_Tuning_SetSensorFPS(SENSOR_FRAME_RATE_NUM, SENSOR_FRAME_RATE_DEN);
    if (ret < 0){
        printf( "failed to set sensor fps\n");
        return -1;
    }
#endif
	IMP_LOG_DBG(TAG, "ImpSystemInit success\n");

	return 0;
}

int venc_system_exit()
{
	int ret = 0;

	IMP_LOG_DBG(TAG, "sample_system_exit start\n");

	ret = IMP_System_Exit();
	if (ret < 0)
	{
		printf("failed System exit\n");
		return -1;
	}

	ret = IMP_ISP_DisableSensor();
	if (ret < 0)
	{
		printf("failed to EnableSensor\n");
		return -1;
	}

	ret = IMP_ISP_DelSensor(&sensor_info);
	if (ret < 0)
	{
		printf("failed to AddSensor\n");
		return -1;
	}

	ret = IMP_ISP_DisableTuning();
	if (ret < 0)
	{
		printf("IMP_ISP_DisableTuning failed\n");
		return -1;
	}

	if (IMP_ISP_Close())
	{
		printf("failed to open ISP\n");
		return -1;
	}

	IMP_LOG_DBG(TAG, " sample_system_exit success\n");

	return 0;
}

int venc_framesource_init(int chn)
{
	int ret = 0;

	if (channel_num[chn].enable)
	{
		ret = IMP_FrameSource_CreateChn(channel_num[chn].index, &channel_num[chn].fs_chn_attr);
		if (ret < 0)
		{
			printf("Framesource init channel (%d) error!\n", channel_num[chn].index);
			return -1;
		}

		ret = IMP_FrameSource_SetChnAttr(channel_num[chn].index, &channel_num[chn].fs_chn_attr);
		if (ret < 0)
		{
			printf("Framesource SetChnAttr (%d) error!\n", channel_num[chn].index);
			return -1;
		}
	}

	return 0;
}

int venc_framesource_exit(int chn, bool destroy_channel)
{
	int ret = 0;

	if (destroy_channel)
	{
		/*Destroy channel*/
		ret = IMP_FrameSource_DestroyChn(channel_num[chn].index);
		if (ret < 0)
		{
			printf("FrameSource DestroyChn (%d) error!\n", chn);
			return -1;
		}
	}
	return 0;
}

int venc_encoder_init(int chn, IMPEncoderChnAttr channel_attr, bool create_group)
{
	int ret = 0, group_num = channel_num[chn].index;

	if (create_group)
	{
		ret = IMP_Encoder_CreateGroup(group_num);
		if (ret < 0)
		{
			printf("Encoder CreateGroup(%d) error !\n", group_num);
			return -1;
		}
	}
	/*create channel*/
	ret = IMP_Encoder_CreateChn(group_num, &channel_attr);
	if (ret < 0)
	{
		printf("Encoder Create(%d) error!\n", chn);
		return -1;
	}

	/*resigter channel for group*/
	ret = IMP_Encoder_RegisterChn(channel_num[chn].index, group_num);
	if (ret < 0)
	{
		printf("Encoder RegisterChn(%d, %d) error!\n", channel_num[chn].index, group_num);
		return -1;
	}

	return 0;
}

int venc_jpeg_init(int chn, bool create_group)
{
	int ret = 0, group_num = channel_num[chn].index;

	IMPEncoderChnAttr channel_attr;
	IMPFSChnAttr *imp_chn_attr_tmp;

	if (create_group)
	{
		ret = IMP_Encoder_CreateGroup(group_num);
		if (ret < 0)
		{
			printf("Encoder CreateGroup(%d) error !\n", group_num);
			return -1;
		}
	}

	if (channel_num[chn].enable)
	{
		imp_chn_attr_tmp = &channel_num[chn].fs_chn_attr;
		memset(&channel_attr, 0, sizeof(IMPEncoderChnAttr));
		ret = IMP_Encoder_SetDefaultParam(&channel_attr, IMP_ENC_PROFILE_JPEG, S_RC_METHOD,
										  imp_chn_attr_tmp->picWidth, imp_chn_attr_tmp->picHeight,
										  imp_chn_attr_tmp->outFrmRateNum, imp_chn_attr_tmp->outFrmRateDen, 0, 0, 25, 0);

		/* Create Channel */
		ret = IMP_Encoder_CreateChn(channel_num[chn].index, &channel_attr);
		if (ret < 0)
		{
			printf("IMP_Encoder_CreateChn(%d) error: %d\n", channel_num[chn].index, ret);
			return -1;
		}

		/* Resigter Channel */
		ret = IMP_Encoder_RegisterChn(group_num, channel_num[chn].index);
		if (ret < 0)
		{
			printf("IMP_Encoder_RegisterChn(0, %d) error: %d\n", channel_num[chn].index, ret);
			return -1;
		}
	}
	printf("Encoder jpeg init channel(%d)\n", chn);

	return 0;
}

int venc_jpeg_exit(int chn)
{
	int ret = 0, chnNum = channel_num[chn].index;
	IMPEncoderChnStat chn_stat;

	memset(&chn_stat, 0, sizeof(IMPEncoderChnStat));
	ret = IMP_Encoder_Query(chnNum, &chn_stat);
	if (ret < 0)
	{
		printf("IMP_Encoder_Query(%d) error: %d\n", chnNum, ret);
		return -1;
	}

	if (chn_stat.registered)
	{
		ret = IMP_Encoder_UnRegisterChn(chnNum);
		if (ret < 0)
		{
			printf("IMP_Encoder_UnRegisterChn(%d) error: %d\n", chnNum, ret);
			return -1;
		}

		ret = IMP_Encoder_DestroyChn(chnNum);
		if (ret < 0)
		{
			printf("IMP_Encoder_DestroyChn(%d) error: %d\n", chnNum, ret);
			return -1;
		}
	}

	printf("Encode jpeg exit channel (%d)\n", channel_num[chn].index);

	return 0;
}

int venc_encoder_setParam_Default(IMPEncoderChnAttr *channel_attr, int chn, size_picture_t size, int fps, int BitRate)
{
	int ret = 0;
	int outFramRateDen = 1;

	memset(channel_attr, 0, sizeof(IMPEncoderChnAttr));
	ret = IMP_Encoder_SetDefaultParam(channel_attr, channel_num[chn].payloadType, S_RC_METHOD, size.width, size.height, fps, outFramRateDen,
									  fps * 2 / outFramRateDen, 2,
									  (S_RC_METHOD == IMP_ENC_RC_MODE_FIXQP) ? 35 : -1,
									  BitRate);
	if (ret < 0)
	{
		printf("Encode SetParam Default error\n");
		return -1;
	}
	return 0;
}

int venc_encoder_exit(int chn, bool destroy_group)
{
	int ret = 0;
	IMPEncoderChnStat chn_stat;

	if (channel_num[chn].enable)
	{
		memset(&chn_stat, 0, sizeof(IMPEncoderChnStat));
		ret = IMP_Encoder_Query(channel_num[chn].index, &chn_stat);
		if (ret < 0)
		{
			printf("Encoder Query (%d) error!\n", channel_num[chn].index);
			return -1;
		}
		if (chn_stat.registered)
		{
			ret = IMP_Encoder_UnRegisterChn(channel_num[chn].index);
			if (ret < 0)
			{
				printf("Encoder Unregister (%d) error !\n", channel_num[chn].index);
				return -1;
			}

			if (destroy_group)
			{
				ret = IMP_Encoder_DestroyChn(channel_num[chn].index);
				if (ret < 0)
				{
					printf("Encoder DestroyChn (%d) error!\n", channel_num[chn].index);
					return -1;
				}
			}
		}
	}

	return 0;
}

int venc_framesource_streamon(int chn)
{
	int ret = 0;

	if (channel_num[chn].enable)
	{
		ret = IMP_FrameSource_EnableChn(channel_num[chn].index);
		if (ret < 0)
		{
			printf("FrameSource EnableChn (%d) error!\n", channel_num[chn].index);
			return -1;
		}
	}

	return 0;
}

int venc_framesource_streamoff(int chn)
{
	int ret = 0;

	if (channel_num[chn].enable)
	{
		ret = IMP_FrameSource_DisableChn(channel_num[chn].index);
		if (ret < 0)
		{
			printf("FrameSource DisableChn (%d) error!\n", channel_num[chn].index);
			return -1;
		}
	}

	return 0;
}

int save_stream(FILE *fd, IMPEncoderStream *stream)
{
	int ret = 0, i = 0, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++)
	{
		IMPEncoderPack *pack = &stream->pack[i];
		if (pack->length)
		{
			uint32_t remSize = stream->streamSize - pack->offset;
			if (remSize < pack->length)
			{
				ret = fwrite((void *)(stream->virAddr + pack->offset), 1, remSize, fd);
				if (ret != remSize)
				{
					// printf("stream write ret 1 (%d) != pack[%d].remSize(%d) error:%s\n", ret, i, remSize, strerror(errno));
					return -1;
				}
				ret = fwrite((void *)stream->virAddr, 1, pack->length - remSize, fd);
				if (ret != (pack->length - remSize))
				{
					// printf("stream write ret 2 (%d) != pack[%d].(length-remSize)(%d) error:%s\n", ret, i, (pack->length - remSize), strerror(errno));
					return -1;
				}
			}
			else
			{
				ret = fwrite((void *)(stream->virAddr + pack->offset), 1, pack->length, fd);
				if (ret != pack->length)
				{
					// printf("stream write ret 3 (%d) != pack[%d].length(%d) error:%s\n", ret, i, pack->length, strerror(errno));
					return -1;
				}
			}
		}
	}

	return 0;
}

void *get_video_stream(void *args)
{
	int ret = 0, val, chnNum;
	char stream_path[64];
	IMPEncoderEncType encType;

	val = (int)args;
	chnNum = val & 0xffff;
	encType = (val >> 16) & 0xffff;

	ret = IMP_Encoder_StartRecvPic(chnNum);
	if (ret < 0)
	{
		printf("IMP_Encoder_StartRecvPic(%d) failed\n", chnNum);
		return NULL;
	}

	sprintf(stream_path, "%s/videostream-%d.%s", STREAM_FILE_PATH_PREFIX, chnNum,
			(encType == IMP_ENC_TYPE_AVC) ? "h264" : ((encType == IMP_ENC_TYPE_HEVC) ? "h265" : "jpeg"));

	FILE *stream_fd = fopen(stream_path, "a+");

	if (stream_fd < 0)
	{
		printf("failed: %s\n", strerror(errno));
		return NULL;
	}

	while (pthread.b_stream)
	{
		ret = IMP_Encoder_PollingStream(chnNum, 1000);
		if (ret < 0)
		{
			printf("IMP_Encoder_PollingStream(%d) timeout\n", chnNum);
			return NULL;
		}

		IMPEncoderStream stream;
		ret = IMP_Encoder_GetStream(chnNum, &stream, 1);

		if (ret < 0)
		{
			printf("IMP_Encoder_GetStream(%d) failed\n", chnNum);
			return NULL;
		}

		ret = save_stream(stream_fd, &stream);
		if (ret < 0)
		{
			fclose(stream_fd);
			return NULL;
		}

		IMP_Encoder_ReleaseStream(chnNum, &stream);
	}

	fclose(stream_fd);

	ret = IMP_Encoder_StopRecvPic(chnNum);
	if (ret < 0)
	{
		printf("IMP_Encoder_StopRecvPic(%d) failed\n", chnNum);
		return NULL;
	}

	return NULL;
}

char *venc_get_jpeg_snap(int chn)
{
	int ret = 0;
	char *data = NULL;
	// char snap_path[64];

	ret = IMP_Encoder_StartRecvPic(channel_num[chn].index);
	if (ret < 0)
	{
		printf("IMP_Encoder_StartRecvPic(%d) failed\n", channel_num[chn].index);
		return NULL;
	}

	/* Polling JPEG Snap, set timeout as 1000msec */
	ret = IMP_Encoder_PollingStream(channel_num[chn].index, 1000);
	if (ret < 0)
	{
		printf("Polling stream channel (%d) timeout\n", channel_num[chn].index);
		return NULL;
	}

	IMPEncoderStream stream;
	/* Get stream Snap */
	ret = IMP_Encoder_GetStream(channel_num[chn].index, &stream, 1);
	if (ret < 0)
	{
		printf("IMP_Encoder_GetStream() failed\n");
		return NULL;
	}

	/* get data stream */
	IMPEncoderPack *pack = &stream.pack[0];
	if (pack->length)
	{
		data = qrcode_getData((uint8_t *)(stream.virAddr + pack->offset), pack->length);
		// if (data)
		// {
		// 	sprintf(snap_path, "%s/snap-%d.jpg", SNAP_FILE_PATH_PREFIX, channel_num[chn].index);
		// 	int snap_fd = open(snap_path, O_RDWR | O_CREAT | O_TRUNC, 0777);
		// 	if (snap_fd < 0)
		// 	{
		// 		IMP_LOG_ERR(TAG, "failed: %s\n", strerror(errno));
		// 		return NULL;
		// 	}

		// 	// ret = save_stream(snap_fd, &stream);
		// 	// if (ret < 0)
		// 	// {
		// 	// 	close(snap_fd);
		// 	// 	return NULL;
		// 	// }

		// 	ret = write(snap_fd, (void *)(stream.virAddr + pack->offset), pack->length);

		// 	close(snap_fd);
		// }
	}

	IMP_Encoder_ReleaseStream(channel_num[chn].index, &stream);
	ret = IMP_Encoder_StopRecvPic(channel_num[chn].index);
	if (ret < 0)
	{
		printf("IMP_Encoder_StopRecvPic() failed\n");
		return NULL;
	}

	return data;
}

int venc_get_video_stream()
{
	unsigned int i = 0;
	int ret = 0;

	for (i = 0; i < 2; i++)
	{
		if (channel_num[i].enable)
		{
			int arg = 0;
			if (channel_num[i].payloadType == IMP_ENC_PROFILE_JPEG)
			{
				arg = (((channel_num[i].payloadType >> 24) << 16) | (4 + channel_num[i].index));
			}
			else
			{
				arg = (((channel_num[i].payloadType >> 24) << 16) | channel_num[i].index);
			}
			ret = pthread_create(&thread_id[i], NULL, get_video_stream, (void *)arg);
			if (ret < 0)
			{
				printf("Create ChnNum%d get_video_stream failed\n",
					   (channel_num[i].payloadType == IMP_ENC_PROFILE_JPEG) ? (4 + channel_num[i].index) : channel_num[i].index);
			}

			// pthread_join(thread_id[i], NULL);
		}
	}
	return 0;
}

int venc_startStream(int chn)
{
	int ret = 0;
	// Start rev picture
	// ret = IMP_Encoder_StartRecvPic(chn);
	// if (ret < 0)
	// {
	// 	printf("Encoder StartRecvPic channel (%d) err (%d) !\n", chn, ret);
	// 	return -1;
	// }

	ret = venc_framesource_init(chn);
	printf("FrameSource Re-init\n");
	if (ret < 0)
	{
		printf("Framsource re-init channel (%d) err (%d) !\n", chn, ret);
		return -1;
	}

	ret = venc_encoder_init(chn, encchn_attr[chn], true);
	printf("Encoder Re-init\n");
	if (ret < 0)
	{
		printf("Encode init channel (%d) err (%d) !\n", chn, ret);
		return -1;
	}

	// ret = IMP_System_Bind(&channel_num[chn].framesource_chn, &channel_num[chn].imp_encoder);
	// printf("Re-bind\n");
	// if (ret < 0)
	// {
	// 	printf("Bin framsource & encode channel (%d) err (%d) !\n", chn, ret);
	// 	return 1;
	// }

	IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
	ret = IMP_System_Bind(&channel_num[chn].framesource_chn, &osdcell);
	if (ret < 0)
	{
		printf("Bind Framesource channel & OSD err !\n");
		return -1;
	}

	ret = IMP_System_Bind(&osdcell, &channel_num[chn].imp_encoder);
	if (ret < 0)
	{
		printf("Bind OSD & channel encoder err !\n");
		return -1;
	}

	ret = venc_framesource_streamon(chn);
	printf("Framexource Re-stream-on\n");
	if (ret < 0)
	{
		printf("Framsource stream on channel (%d) err (%d) !\n", chn, ret);
		return -1;
	}

	// Start thread
	if (channel_num[chn].enable)
	{
		int ret, arg = 0;
		arg = (((channel_num[chn].payloadType >> 24) << 16) | channel_num[chn].index); // tinh argument: h265: 65536, h264: 1
		printf("Re-get stream...\n\n");
		ret = pthread_create(&thread_id[chn], NULL, get_video_stream, (void *)arg);
		if (ret < 0)
		{
			printf("Create ChnNum%d get_video_stream failed\n",
				   (channel_num[chn].payloadType == IMP_ENC_PROFILE_JPEG) ? (4 + channel_num[chn].index) : channel_num[chn].index);
		}
	}

	return 0;
}

int venc_stopStream(int chn)
{
	int ret = 0;

	// Stop receive picture
	ret = IMP_Encoder_StopRecvPic(chn);
	if (ret < 0)
	{
		printf("Encode StopRecvPic channel (%d) err (%d) !\n", chn, ret);
		return -1;
	}

	ret = venc_framesource_streamoff(chn);
	printf("Framesource streamoff channel (%d)\n", chn);
	if (ret < 0)
	{
		printf("Framesource streamoff channel (%d) err (%d)\n", chn, ret);
		return -1;
	}

	// Stop stream;
	ret = pthread_cancel(thread_id[chn]);
	printf("Stop thread channel (%d)\n", chn);
	if (ret < 0)
	{
		printf("Stop thread channel err (%d)!\n", ret);
		return -1;
	}

	// ret = IMP_System_UnBind(&channel_num[chn].framesource_chn, &channel_num[chn].imp_encoder);
	// if (ret < 0)
	// {
	// 	printf("Ubin framsource & channel encode (%d) err (%d) !\n", chn, ret);
	// 	return -1;
	// }

	IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
	ret = IMP_System_UnBind(&osdcell, &channel_num[chn].imp_encoder);
	if (ret < 0)
	{
		printf("UnBind OSD & channel encoder err !\n");
		return -1;
	}

	ret = IMP_System_UnBind(&channel_num[chn].framesource_chn, &osdcell);
	if (ret < 0)
	{
		printf("UnBind Framesource & OSD err !\n");
		return -1;
	}

	ret = venc_encoder_exit(chn, true);
	printf("Encoder exit channel (%d)\n", chn);
	if (ret < 0)
	{
		printf("Encoder exit channel (%d) err (%d) !\n", chn, ret);
		return -1;
	}

	ret = venc_framesource_exit(chn, false);
	if (ret < 0)
	{
		printf("Framsource exit channel (%d) err (%d) !\n", chn, ret);
		return -1;
	}

	printf("Stop Stream channel(%d)!\n\n", chn);

	return 0;
}

int venc_SetIRCUT(int enable)
{
	int fd, fd79, fd80;
	char on[4], off[4];

	if (!access("/tmp/setir", 0))
	{
		if (enable)
		{
			system("/tmp/setir 0 1");
		}
		else
		{
			system("/tmp/setir 1 0");
		}
		return 0;
	}

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/export error !");
		return -1;
	}

	write(fd, "79", 2);
	write(fd, "80", 2);

	close(fd);

	fd79 = open("/sys/class/gpio/gpio79/direction", O_RDWR);
	if (fd79 < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio79/direction error !");
		return -1;
	}

	fd80 = open("/sys/class/gpio/gpio80/direction", O_RDWR);
	if (fd80 < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio80/direction error !");
		return -1;
	}

	write(fd79, "out", 3);
	write(fd80, "out", 3);

	close(fd79);
	close(fd80);

	fd79 = open("/sys/class/gpio/gpio79/active_low", O_RDWR);
	if (fd79 < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio79/active_low error !");
		return -1;
	}

	fd80 = open("/sys/class/gpio/gpio80/active_low", O_RDWR);
	if (fd80 < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio80/active_low error !");
		return -1;
	}

	write(fd79, "0", 1);
	write(fd80, "0", 1);

	close(fd79);
	close(fd80);

	fd79 = open("/sys/class/gpio/gpio79/value", O_RDWR);
	if (fd79 < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio79/value error !");
		return -1;
	}

	fd80 = open("/sys/class/gpio/gpio80/value", O_RDWR);
	if (fd80 < 0)
	{
		IMP_LOG_DBG(TAG, "open /sys/class/gpio/gpio80/value error !");
		return -1;
	}

	sprintf(on, "%d", enable);
	sprintf(off, "%d", !enable);

	write(fd79, "0", 1);
	usleep(10 * 1000);

	write(fd79, on, strlen(on));
	write(fd80, off, strlen(off));

	if (!enable)
	{
		usleep(10 * 1000);
		write(fd79, off, strlen(off));
	}

	close(fd79);
	close(fd80);

	return 0;
}

void *get_photosensitive(void *p)
{
	int i = 0, ret = 0;
	float gb_gain, gr_gain;
	float iso_buf;
	bool ircut_status = true;
	int night_count = 0;
	int day_count = 0;
	// int day_oth_count = 0;
	// bayer域的 (g分量/b分量) 统计值
	float gb_gain_record = 200;
	float gr_gain_record = 200;
	float gb_gain_buf = 200, gr_gain_buf = 200;
	IMPISPRunningMode pmode;
	IMPISPEVAttr ExpAttr;
	IMPISPWB wb;
	IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY);
	venc_SetIRCUT(1);
	while (pthread.b_mode)
	{
		// 获取曝光AE信息
		ret = IMP_ISP_Tuning_GetEVAttr(&ExpAttr);
		if (ret == 0)
		{
			// printf("u32ExposureTime: %d\n", ExpAttr.ev);
			// printf("u32AnalogGain: %d\n", ExpAttr.again);
			// printf("u32DGain: %d\n", ExpAttr.dgain);
		}
		else
		{
			return NULL;
		}
		iso_buf = ExpAttr.ev;
		// printf(" iso buf ==%f\n", iso_buf);
		ret = IMP_ISP_Tuning_GetWB_Statis(&wb);
		if (ret == 0)
		{
			gr_gain = wb.rgain;
			gb_gain = wb.bgain;
			// printf("gb_gain: %f\n", gb_gain);
			// printf("gr_gain: %f\n", gr_gain);
			printf("gr_gain_record: %f\n", gr_gain_record);
		}
		else
		{
			return NULL;
		}

		// 平均亮度小于20，则切到夜视模式
		if (iso_buf > 300000)
		{
			night_count++;
			printf("night_count==%d\n", night_count);
			if (night_count > 3)
			{
				IMP_ISP_Tuning_GetISPRunningMode(&pmode);
				if (pmode != IMPISP_RUNNING_MODE_NIGHT)
				{
					printf("### entry night mode ###\n");
					IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_NIGHT);
					venc_SetIRCUT(0);
					ircut_status = true;
				}
				// 切夜视后，取20个gb_gain的的最小值，作为切换白天的参考数值gb_gain_record ，gb_gain为bayer的G/B
				for (i = 0; i < 10; i++)
				{
					IMP_ISP_Tuning_GetWB_Statis(&wb);
					gr_gain = wb.rgain;
					gb_gain = wb.bgain;
					if (i == 0)
					{
						gb_gain_buf = gb_gain;
						gr_gain_buf = gr_gain;
					}
					gb_gain_buf = ((gb_gain_buf > gb_gain) ? gb_gain : gb_gain_buf);
					gr_gain_buf = ((gr_gain_buf > gr_gain) ? gr_gain : gr_gain_buf);
					usleep(50000);
					gb_gain_record = gb_gain_buf;
					gr_gain_record = gr_gain_buf;
					// printf("gb_gain == %f,iso_buf=%f",gb_gain,iso_buf);
					// printf("gr_gain_record == %f\n ",gr_gain_record);
				}
			}
		}
		else
		{
			night_count = 0;
		}
		// 满足这3个条件进入白天切换判断条件
		if (((int)iso_buf < 300000) && (ircut_status == true) && (gb_gain > gb_gain_record + 15))
		{
			if ((iso_buf < 200000) || (gb_gain > 50))
			{
				day_count++;
			}
			else
			{
				day_count = 0;
			}
			// printf("gr_gain_record == %f gr_gain =%f line=%d\n",gr_gain_record,gr_gain,__LINE__);
			// printf("day_count == %d\n",day_count);
			if (day_count > 3)
			{
				printf("### entry day mode ###\n");
				IMP_ISP_Tuning_GetISPRunningMode(&pmode);
				if (pmode != IMPISP_RUNNING_MODE_DAY)
				{
					IMP_ISP_Tuning_SetISPRunningMode(IMPISP_RUNNING_MODE_DAY);
					venc_SetIRCUT(1);
					ircut_status = false;
				}
			}
		}
		else
		{
			day_count = 0;
		}
		sleep(1);
	}
	return NULL;
}

/**
 * @brief
 *
 * @return init stream
 */

int venc_stream_init()
{
	int i = 0, ret = 0;

	/* System init */
	ret = venc_system_init();
	if (ret < 0)
	{
		printf("System init (%d) err !\n", ret);
		return -1;
	}

	/* FrameSource init */
	for (i = 0; i < 2; i++)
	{
		ret = venc_framesource_init(i);
		if (ret < 0)
		{
			printf("Framsource init channel (%d) err !\n", ret);
			return -1;
		}
	}

	/* Set encoder param channel */
	for (i = 0; i < 2; i++)
	{
		ret = venc_encoder_setParam_Default(&encchn_attr[i], i, size_pic[0], FPS, BITRATE);
		if (ret < 0)
		{
			printf("Encode SetParam channel (%d) err !\n", ret);
			return -1;
		}
	}

	/* Encoder init */
	for (i = 0; i < 2; i++)
	{
		ret = venc_encoder_init(i, encchn_attr[i], true);
		if (ret < 0)
		{
			printf("Encode init channel (%d) err !\n", ret);
			return -1;
		}
	}

	/* Framsource stream on*/
	for (i = 0; i < 2; i++)
	{
		ret = venc_framesource_streamon(i);
		if (ret < 0)
		{
			printf("Framsource stream on channel (%d) err !\n", ret);
			return -1;
		}
	}

	/* get stream */
	ret = venc_get_video_stream();
	if (ret < 0)
	{
		printf("Get video stream (%d) err !\n", ret);
		return -1;
	}

	/* bind */
	// for (i = 0; i < 2; i++)
	// {
	// 	ret = IMP_System_Bind(&channel_num[i].framesource_chn, &channel_num[i].imp_encoder);
	// 	if (ret < 0)
	// 	{
	// 		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n", i);
	// 		return -1;
	// 	}
	// }

	/* Start OSD channel 0 */
	ret = venc_startOSD(&osd, grpNum);
	if (ret < 0)
	{
		printf("OSD start channel err (%d) !\n", ret);
		return -1;
	}

	return 0;
}

int venc_stream_exit()
{
	int i = 0, ret = 0;

	/* Ecxit flag stream video*/

	/* Exit thread */
	pthread.b_mode = false;
	pthread.b_checkEvt = false;
	pthread.b_osd = false;
	pthread.b_stream = false;

	/* framsource streamoff */
	for (i = 0; i < 2; i++)
	{
		ret = venc_framesource_streamoff(i);
		if (ret < 0)
		{
			printf("Framsource streamoff channel (%d) err!\n", ret);
			return -1;
		}
	}

	/* exit OSD */
	ret = venc_stopOSD(&osd, grpNum);
	if (ret < 0)
	{
		printf("Stop OSD err !\n");
		return -1;
	}

	/* Unbind */
	// for (i = 0; i < 2; i++)
	// {
	// 	ret = IMP_System_UnBind(&channel_num[i].framesource_chn, &channel_num[i].imp_encoder);
	// 	printf("UnBind FrameSource channel (%d) and Encoder done\n", i);
	// 	if (ret < 0)
	// 	{
	// 		printf("UnBind FrameSource channel (%d) and Encoder err !\n", ret);
	// 		return -1;
	// 	}
	// }

	/* encoder exit */
	for (i = 0; i < 2; i++)
	{
		ret = venc_encoder_exit(i, true);
		if (ret < 0)
		{
			printf("Encode exit channel (%d) err!\n", ret);
			return -1;
		}
	}

	/* framsource exit */
	for (i = 0; i < 2; i++)
	{
		ret = venc_framesource_exit(i, true);
		if (ret < 0)
		{
			printf("Framsource exit channel (%d) err!\n", ret);
			return -1;
		}
	}

	/* system exit */
	ret = venc_system_exit();
	if (ret < 0)
	{
		printf("System exit (%d) err !\n", ret);
		return -1;
	}

	return 0;
}

/* init snap JPEG */
int venc_snapJPEG_init()
{
	int ret = 0;

	/* snap framsource init */
	ret = venc_framesource_init(CHANNEL_2);
	if (ret < 0)
	{
		printf("Framesource JPEG init error!\n");
		return -1;
	}
	/* jpeg encode init */
	ret = venc_jpeg_init(CHANNEL_2, true);
	if (ret < 0)
	{
		printf("JPEG init error!\n");
		return -1;
	}
	/* bind */
	IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
	ret = IMP_System_Bind(&channel_num[CHANNEL_2].framesource_chn, &osdcell);
	if (ret < 0)
	{
		printf("Bind Framesource channel & OSD err !\n");
		return -1;
	}

	ret = IMP_System_Bind(&osdcell, &channel_num[CHANNEL_2].imp_encoder);
	if (ret < 0)
	{
		printf("Bind OSD & channel encoder err !\n");
		return -1;
	}
	// ret = IMP_System_Bind(&channel_num[CHANNEL_2].framesource_chn, &channel_num[CHANNEL_2].imp_encoder);
	// if (ret < 0)
	// {
	// 	printf("Bind channel JPEG (%d) err !\n", CHANNEL_2);
	// 	return -1;
	// }
	/* snap jpeg stream on */
	ret = venc_framesource_streamon(CHANNEL_2);

	if (ret < 0)
	{
		printf("Framesource JPEG streamon error!\n");
		return -1;
	}

	return 0;
}

/* exit snap JPEG */
int venc_snapJPEG_exit()
{
	int ret = 0;
	/* snap stream off */
	ret = venc_framesource_streamoff(CHANNEL_2);
	if (ret < 0)
	{
		printf("Framesource  stream off channel err (%d)!\n", ret);
		return -1;
	}
	/* Unbin */
	IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
	ret = IMP_System_UnBind(&osdcell, &channel_num[CHANNEL_2].imp_encoder);
	if (ret < 0)
	{
		printf("UnBind OSD & channel encoder err !\n");
		return -1;
	}

	ret = IMP_System_UnBind(&channel_num[CHANNEL_2].framesource_chn, &osdcell);
	if (ret < 0)
	{
		printf("UnBind Framesource & OSD err !\n");
		return -1;
	}
	// ret = IMP_System_UnBind(&channel_num[CHANNEL_2].framesource_chn, &channel_num[CHANNEL_2].imp_encoder);
	// if (ret < 0)
	// {
	// 	printf("Unbin framesource & channel encoder err (%d)!\n", ret);
	// 	return -1;
	// }
	/* JPEG exit */
	ret = venc_jpeg_exit(CHANNEL_2);
	if (ret < 0)
	{
		printf("JPEG exit channel err (%d)!\n", ret);
		return -1;
	}

	/* snap framsource exit */
	ret = venc_framesource_exit(CHANNEL_2, true);
	if (ret < 0)
	{
		printf("Framesource exit channel err (%d)!\n", ret);
		return -1;
	}

	return 0;
}
