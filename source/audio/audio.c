/**
 * @file audio.c
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <math.h>
#include <sys/prctl.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include <imp/imp_audio.h>
#include <imp/imp_audio.h>

#include "audio.h"

static int audio_getfile(const char* audio_file);

int audio_play(unsigned long mode, const char* audio_file)
{
	speaker_mode(mode);
	audio_getfile(audio_file);

	return 0;
}

//************* STATIC FUNC ****************//
/////////////////////////////////////////////
static int audio_getfile(const char* audio_file)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;

	buf = (unsigned char *)malloc(AO_TEST_BUF_SIZE);
	if (buf == NULL)
	{
		printf("[ERROR] %s: malloc audio buf error\n", __func__);
		return -1;
	}

	FILE *play_file = fopen(audio_file, "rb");
	if (play_file == NULL)
	{
		printf("[ERROR] %s: fopen %s failed\n", __func__, audio_file);
		return -1;
	}

	/* Step 1: set public attribute of AO device. */
	int devID = 0;
	IMPAudioIOAttr attr;
	attr.samplerate = AUDIO_SAMPLE_RATE_16000;
	attr.bitwidth = AUDIO_BIT_WIDTH_16;
	attr.soundmode = AUDIO_SOUND_MODE_MONO;
	attr.frmNum = 20;
	attr.numPerFrm = 640;
	attr.chnCnt = 1;
	ret = IMP_AO_SetPubAttr(devID, &attr);
	if (ret != 0)
	{
		printf("set ao %d attr err: %d\n", devID, ret);
		return -1;
	}

	memset(&attr, 0x0, sizeof(attr));
	ret = IMP_AO_GetPubAttr(devID, &attr);
	if (ret != 0)
	{
		printf("get ao %d attr err: %d\n", devID, ret);
		return -1;
	}

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(devID);
	if (ret != 0)
	{
		printf("enable ao %d err\n", devID);
		return -1;
	}

	/* Step 3: enable AI channel. */
	int chnID = 0;
	ret = IMP_AO_EnableChn(devID, chnID);
	if (ret != 0)
	{
		printf("Audio play enable channel failed\n");
		return -1;
	}

	/* Step 4: Set audio channel volume. */
	int chnVol = 80;
	ret = IMP_AO_SetVol(devID, chnID, chnVol);
	if (ret != 0)
	{
		printf("Audio Play set volume failed\n");
		return -1;
	}

	ret = IMP_AO_GetVol(devID, chnID, &chnVol);
	if (ret != 0)
	{
		printf("Audio Play get volume failed\n");
		return -1;
	}

	int aogain = 28;
	ret = IMP_AO_SetGain(devID, chnID, aogain);
	if (ret != 0)
	{
		printf("Audio Record Set Gain failed\n");
		return -1;
	}

	ret = IMP_AO_GetGain(devID, chnID, &aogain);
	if (ret != 0)
	{
		printf("Audio Record Get Gain failed\n");
		return -1;
	}

	while (1)
	{
		size = fread(buf, 1, AO_TEST_BUF_SIZE, play_file);
		if (size < AO_TEST_BUF_SIZE)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(devID, chnID, &frm, BLOCK);
		if (ret != 0)
		{
			printf("send Frame Data error\n");
			return -1;
		}

		IMPAudioOChnState play_status;
		ret = IMP_AO_QueryChnStat(devID, chnID, &play_status);
		if (ret != 0)
		{
			printf("IMP_AO_QueryChnStat error\n");
			return -1;
		}
	}

	ret = IMP_AO_FlushChnBuf(devID, chnID);
	if (ret != 0)
	{
		printf("IMP_AO_FlushChnBuf error\n");
		return -1;
	}

	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(devID, chnID);
	if (ret != 0)
	{
		printf("Audio channel disable error\n");
		return -1;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(devID);
	if (ret != 0)
	{
		printf("Audio device disable error\n");
		return -1;
	}

	fclose(play_file);
	free(buf);
	

	return 0;
}
