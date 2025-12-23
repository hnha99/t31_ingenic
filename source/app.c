/**
 * @file option.c
 * @author nhanh7
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>

//** user include **//
#include "venc-common.h"
#include "qrcode.h"
#include "audio.h"
#include "speaker.h"
#include "base64.h"
#include "osd.h"
#include "led_state.h"
#include "rtspd_server.h"

#define TAG "encoder_streamvideo"
#define WIFI_START "/tmp/start_wlan0"
#define CMD_CHECK_SDCARD "blkid | grep /dev/mmcblk0"
#define CMD_PING_GOOGLE "ping -c 1 google.com"

typedef struct
{
	int chn;
	int get_config;
	int res_size;
	bool feature_flag;
	bool qrcode_flag;
} option_t;
option_t option;

static void *_check(void *args);
static int app_checkEvtStatus();

/* scan qr code */
int app_scan_QRCode()
{
	int ret;
	char *return_data = NULL;
	char ssid[32], pass[32];
	option.qrcode_flag = true;

	memset(pass, 0, sizeof(pass));
	memset(ssid, 0, sizeof(ssid));
	while (option.qrcode_flag)
	{
		return_data = venc_get_jpeg_snap(CHANNEL_2);
		// qrcode_scanImage("/tmp/t31-ingenic/quangtv23/fcam/snap/1622130109.jpg");
		// printf("return_data__[%s]\n", return_data);
		if (return_data)
		{
			// de_data = b64_decode(return_data, strlen(return_data));
			ret = qrcode_setData(return_data, ssid, pass);
			if (ret == 0)
				break;
		}
		printf("Scaning...\n\n");
	}
	option.qrcode_flag = false;

	qrcode_saveFile(ssid, pass);
	printf("Save ID[%s] & PASS[%s] OK!\n", ssid, pass);
	free(return_data);

	return 0;
}

/* day & night mode */
int app_mode()
{
	int ret;
	pthread_t mode;
	ret = pthread_create(&mode, NULL, get_photosensitive, NULL);
	if (ret < 0)
	{
		printf("Thread mode err (%d) !\n", ret);
		return -1;
	}
	return 0;
}

/* check SDCard */
int app_checkSDCard(char *cmd)
{
	FILE *fd = NULL;
	int n = 0;
	char buf[64];
	char temp[64] = "/dev/mmcblk0: UUID=";
	memset(buf, 0, sizeof(buf));

	if ((fd = popen(cmd, "r")) == NULL)
		perror("popen");
	while ((fgets(buf, sizeof(buf), fd)) != NULL)
		pclose(fd);

	n = strncmp(buf, temp, 19);
	// printf("buf_sdcard[%s]____[%d]\n", buf, n);

	return n;
}

/* check connect internet*/
int app_checkConnect(char *cmd)
{
	FILE *fd = NULL;
	int n = 0, i = 0;
	char buf[64];
	char temp[64] = "64 bytes from ";
	memset(buf, 0, sizeof(buf));

	if ((fd = popen(cmd, "r")) == NULL)
		perror("popen");

	for (i = 0; i < 2; i++)
	{
		fgets(buf, sizeof(buf), fd);
	}
	pclose(fd);

	n = strncmp(buf, temp, 14);
	// printf("buf_internet[%s]____[%d]\n", buf, n);

	return n;
}

int main(int argc, char *argv[])
{
	int ret = 0;

	pthread.b_mode = true;
	pthread.b_checkEvt = true;
	pthread.b_osd = true;
	pthread.b_stream = true;
	pthread.b_getevt = true;
	option.feature_flag = true;

	/* init venc */
	ret = venc_stream_init();
	if (ret != 0)
	{
		printf("Init stream video err!!\n");
		return -1;
	}
	printf("Get video stream...\n\n");

	/* create fifo start rtsp */
	// rtspd_creatFifo();
	// rtspd_startH265();
	// rtspd_startGetStream();

	/* mode day & night */
	// ret = app_mode();
	// if (ret < 0)
	// {
	// 	printf(" Init option mode err (%d) !\n", ret);
	// 	return -1;
	// }

	/* check connect network */
	// app_checkEvtStatus();
	/* handle led */
	// led_start();

	/* signal control venc */
	while (option.feature_flag)
	{
		/* get config feature */
		printf("Get_feature = ");
		scanf("%d", &option.get_config);
		switch (option.get_config)
		{
		case 0:
		{
			printf("Channel_config = ");
			scanf("%d", &option.chn);
			printf("Change_res: ");
			scanf("%d", &option.res_size);
			ret = venc_stopStream(option.chn);
			if (ret < 0)
				break;

			venc_encoder_setParam_Default(&encchn_attr[option.chn], option.chn, size_pic[option.res_size], FPS, BITRATE);
			ret = venc_startStream(option.chn);
			if (ret < 0)
				break;
		}
		break;
		case 1:
		{
			printf("Channel_config = ");
			scanf("%d", &option.chn);
			printf("FPS = ");
			scanf("%d", &venc_config_t.fps);
			printf("BitRate = ");
			scanf("%d", &venc_config_t.outBitRate);

			ret = venc_stopStream(option.chn);
			if (ret < 0)
				break;
			ret = venc_encoder_setParam_Default(&encchn_attr[option.chn], option.chn, size_pic[2], venc_config_t.fps, venc_config_t.outBitRate);
			if (ret < 0)
				printf("Encoder setParam Default err (%d) !", ret);
			ret = venc_startStream(option.chn);
			if (ret < 0)
				break;
		}
		break;

		case 2:
			printf("APP led enable\n");
			led_appCtrl(true);
			break;

		case 3:
			printf("APP led enable\n");
			led_appCtrl(false);
			break;

		case 4:
			/* exit video encode */
			led_setEvent(RESET);
			venc_stream_exit();

			// system("killall -9 wpa_supplicant");
			// system("killall -9 udhcpc");
			printf("Exit app\n");
			option.feature_flag = false;

			break;

		case 5:
			led_setEvent(NOTSD_RECORDING);

			break;
		case 6:
			led_setEvent(SD_RECORDING);

			break;
		case 7:
			led_setEvent(NOT_INTERNET);

			break;
		case 8:
			led_setEvent(SETUP_WIFI);

			/* start scan qrcode */
			ret = venc_snapJPEG_init();
			if (ret < 0)
				break;

			app_scan_QRCode();
			led_setEvent(NOTHING);
			usleep(500000);
			venc_snapJPEG_exit();
			// audio_play(SPEAKER_CTL_MODE3, AO_QRCODE_SCAN_OK);

			led_setEvent(CONNECTING_ROUTER);

			/* start wifi */
			system(WIFI_START);
			printf("Start wifi done!\n");

			led_setEvent(NOTHING);
			break;

		case 9:
			break;

		default:
			printf("Unknow!!\n");
		}
	}

	return 0;
}

/**
 * @brief STATIC FUNCTION
 *
 */

/* check connect & sd card */
static void *_check(void *args)
{
	led_state_t *evt = (led_state_t *)args;
	bool b_audio = true;
	evt->b_appctrl = true;

	while (pthread.b_checkEvt)
	{
		if (app_checkSDCard(CMD_CHECK_SDCARD) != 0)
		{
			led_setEvent(NOT_SDCARD);
		}

		/* check connected */
		else if ((app_checkConnect(CMD_PING_GOOGLE) == 0))
		{
			led_setEvent(CONNECTED_INTERNET);
			if (b_audio == true)
			{
				printf("Wifi connected\n");
				// audio_play(SPEAKER_CTL_MODE3, AO_WIFI_CONNECT);
				b_audio = false;
			}
		}
		/* check disconnect */
		else if ((app_checkConnect(CMD_PING_GOOGLE) != 0))
		{
			led_setEvent(NOT_INTERNET);
			// audio_play(SPEAKER_CTL_MODE3, AO_WIFI_NOT_INTERNET);
			b_audio = true;
		}
		else
		{
			led_setEvent(NOTHING);
		}
	}

	return NULL;
}
static int app_checkEvtStatus()
{
	pthread_t check_id;

	pthread_create(&check_id, NULL, _check, &led_event);

	return 0;
}