/**********
This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**********/
// Copyright (c) 1996-2018, Live Networks, Inc.  All rights reserved
// A test program that demonstrates how to stream - via unicast RTP
// - various kinds of file on demand, using a built-in RTSP server.
// main program

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C"
{
	#include "rtspd_server.h"
}
#include "version.h"
#include "pthread.h"
#include "venc-common.h"


typedef struct
{
	int channel;
	char watchVariable;
	bool b_getstream;
} param_rtspd_t;

param_rtspd_t param;
TaskScheduler *scheduler;
RTSPServer *rtspServer;
ServerMediaSession *sms;

UsageEnvironment *env;
// To make the second and subsequent client for each stream reuse the same
// input stream as the first client (rather than playing the file from the
// start for each client), change the following "False" to "True":

Boolean reuseFirstSource = False;
// To stream *only* MPEG-1 or 2 video "I" frames
// (e.g., to reduce network bandwidth),
// change the following "False" to "True":
Boolean iFramesOnly = False;

static void announceStream(RTSPServer *rtspServer, ServerMediaSession *sms, char const *streamName, char const *inputFileName);
static int rtspd_saveStream(FILE *fd, IMPEncoderStream *stream);
static int rtspd_getstream(FILE *fd, int chn);
static int rtspd_stream(char const *inFIle, int channel, bool b_stream);

int rtspd_creatFifo()
{
	unlink(INPUT_H265);
	if (mkfifo(INPUT_H265, 0777) < 0)
	{
		*env << "mkfifo Failed\n";
		exit(1);
	}

	unlink(INPUT_H264);
	if (mkfifo(INPUT_H264, 0777) < 0)
	{
		*env << "mkfifo Failed\n";
		exit(1);
	}

	return 0;
}

void *_streamH265(void *agrs)
{
	param.b_getstream = true;
	rtspd_stream(INPUT_H265, 0, param.b_getstream);
	return 0;
}
void *_streamH264(void *args)
{
	param.b_getstream = true;
	rtspd_stream(INPUT_H264, 1, param.b_getstream);
	return 0;
}

//* RTSP H265 *//
int rtspd_h265(char const *streamName, char const *inputFileName)
{
	// Begin by setting up our usage environment:
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase *authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
													 // Repeat the above with each <username>, <password> that you wish to allow
													 // access to the server.
#endif

	// // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 600000;

	// // Create the RTSP server:
	rtspServer = RTSPServer::createNew(*env, 554, authDB);
	if (rtspServer == NULL)
	{
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	char const *descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

	// Set up each of the possible streams that can be served by the
	// RTSP server.  Each such stream is implemented using a
	// "ServerMediaSession" object, plus one or more
	// "ServerMediaSubsession" objects for each audio/video substream.

	// A H.265 video elementary stream:
	sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);
	sms->addSubsession(H265VideoFileServerMediaSubsession ::createNew(*env, inputFileName, reuseFirstSource));
	rtspServer->addServerMediaSession(sms);

	announceStream(rtspServer, sms, streamName, inputFileName);

	env->taskScheduler().doEventLoop(&param.watchVariable); // does not return

	return 0;
}

//* RTSP H264 *//
int rtspd_h264(char const *streamName, char const *inputFileName)
{
	// Begin by setting up our usage environment:
	scheduler = BasicTaskScheduler::createNew();
	env = BasicUsageEnvironment::createNew(*scheduler);

	UserAuthenticationDatabase *authDB = NULL;
#ifdef ACCESS_CONTROL
	// To implement client access control to the RTSP server, do the following:
	authDB = new UserAuthenticationDatabase;
	authDB->addUserRecord("username1", "password1"); // replace these with real strings
													 // Repeat the above with each <username>, <password> that you wish to allow
													 // access to the server.
#endif

	// // Create a 'H264 Video RTP' sink from the RTP 'groupsock':
	OutPacketBuffer::maxSize = 100000;

	// // Create the RTSP server:
	rtspServer = RTSPServer::createNew(*env, 554, authDB);
	if (rtspServer == NULL)
	{
		*env << "Failed to create RTSP server: " << env->getResultMsg() << "\n";
		exit(1);
	}

	char const *descriptionString = "Session streamed by \"testOnDemandRTSPServer\"";

	// Set up each of the possible streams that can be served by the
	// RTSP server.  Each such stream is implemented using a
	// "ServerMediaSession" object, plus one or more
	// "ServerMediaSubsession" objects for each audio/video substream.

	// A H.265 video elementary stream:
	sms = ServerMediaSession::createNew(*env, streamName, streamName, descriptionString);
	sms->addSubsession(H264VideoFileServerMediaSubsession ::createNew(*env, inputFileName, reuseFirstSource));
	rtspServer->addServerMediaSession(sms);

	announceStream(rtspServer, sms, streamName, inputFileName);

	env->taskScheduler().doEventLoop(&param.watchVariable); // does not return

	return 0;
}

void *_rtspH265(void *agrs)
{
	rtspd_h265(STREAM_H265, INPUT_H265);
	return 0;
}
void *_rtspH264(void *args)
{
	rtspd_h264(STREAM_H264, INPUT_H264);
	return 0;
}

int rtspd_startH264()
{
	pthread_t h264_id;

	pthread_create(&h264_id, NULL, _rtspH264, NULL);

	return 0;
}

int rtspd_startH265()
{
	pthread_t h265_id;

	pthread_create(&h265_id, NULL, _rtspH265, NULL);

	return 0;
}

int rtspd_startGetStream()
{
	pthread_t get_h264, get_h265;

	pthread_create(&get_h264, NULL, _streamH264, NULL);
	pthread_create(&get_h265, NULL, _streamH265, NULL);

	return 0;
}

int stop_rtspd()
{
	param.watchVariable = 1;
	param.b_getstream = false;

	rtspServer->removeServerMediaSession(sms);
	Medium::close(rtspServer);
	env->reclaim();
	delete scheduler;

	return 0;
}


/**
 * @Static function
 *
 */

static int rtspd_saveStream(FILE *fd, IMPEncoderStream *stream)
{
	unsigned int ret;
	int i, nr_pack = stream->packCount;

	for (i = 0; i < nr_pack; i++)
	{
		IMPEncoderPack *pack = &stream->pack[i];

		ret = fwrite((void *)(stream->virAddr + pack->offset), 1, pack->length, fd);
		if (ret != pack->length)
		{
			printf("stream write ret 3 (%d) != pack[%d].length(%d) error:%s\n", ret, i, pack->length, strerror(errno));
			return -1;
		}
	}

	return 0;
}

static int rtspd_getstream(FILE *fd, int chn)
{
	int ret;

	ret = IMP_Encoder_StartRecvPic(chn);
	if (ret < 0)
	{
		printf("IMP_Encoder_StartRecvPic(%d) failed\n", 1);
		return ret;
	}
	/* Polling H264 Stream, set timeout as 1000msec */
	ret = IMP_Encoder_PollingStream(chn, 100);
	if (ret < 0)
	{
		printf("Polling stream timeout\n");
	}

	IMPEncoderStream stream;
	/* Get Stream */
	ret = IMP_Encoder_GetStream(chn, &stream, 1);
	if (ret < 0)
	{
		printf("IMP_Encoder_GetStream() failed\n");
		return -1;
	}

	ret = rtspd_saveStream(fd, &stream);
	if (ret < 0)
	{
		fclose(fd);
		return ret;
	}

	IMP_Encoder_ReleaseStream(chn, &stream);

	return 0;
}

static void announceStream(RTSPServer *rtspServer, ServerMediaSession *sms, char const *streamName, char const *inputFileName)
{
	char *url = rtspServer->rtspURL(sms);
	UsageEnvironment &env = rtspServer->envir();
	env << "\n\"" << streamName << "\" stream, from the file \""
		<< inputFileName << "\"\n";
	env << "Play this stream using the URL \"" << url << "\"\n";
	delete[] url;
}

static int rtspd_stream(char const *inFIle, int channel, bool b_stream)
{
	FILE *fd;

	fd = fopen(inFIle, "a+");
	if (fd < 0)
	{
		*env << "Failed open fifo\n";
		exit(1);
		;
	}
	while (b_stream)
	{

		if (rtspd_getstream(fd, channel) < 0)
		{
			printf("get stream errrrr\n");
			break; // 基于君正提供的API实现采集和编码，并将编码后的数据保存到fifo中。
		}
		// printf("get streammmmmmm\n");
	}

	return 0;
}
