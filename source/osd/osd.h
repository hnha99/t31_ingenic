/**
 * @file osd.h
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __OSD_H__
#define __OSD_H__

#define OSD_REGION_WIDTH		16
#define OSD_REGION_HEIGHT		36

#define OSD_LETTER_NUM          20

#define FONT_PATH "/tmp/t31-ingenic/nhanh7/t31_ingenic/times.ttf"
#define LOGO_PATH "/tmp/t31-ingenic/nhanh7/t31_ingenic/TextLogo_136x22.bmp"

#define OSD_RGN_HANDER_MAX      3
#define OSD_SHOW_ENABLE         1
#define OSD_SHOW_DISABLE        0

#define OSD_RGN_HANDER_0        0
#define OSD_RGN_HANDER_1        1
#define OSD_RGN_HANDER_2        2

#define OSD_TIME_X              10
#define OSD_TIME_Y              10
#define OSD_FONT_X              180
#define OSD_FONT_Y              1070
#define OSD_LOGO_X              200
#define OSD_LOGO_Y              50

typedef struct
{
    IMPRgnHandle *prHander;
} osd_config_t;
osd_config_t osd;

extern int grpNum;
IMPRgnHandle *venc_osd_init(int grpNum);
int vnec_osd_exit(IMPRgnHandle *prHander,int grpNum);
int venc_startOSD(osd_config_t *osd, int grpNum);
int venc_stopOSD(osd_config_t *osd, int grpNum);


#endif // __OSD_H__