/**
 * @file osd.c
 * @author nhanh7
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <imp_log.h>
#include <imp_common.h>
#include <imp_system.h>
#include <imp_framesource.h>
#include <imp_encoder.h>
#include <imp_osd.h>
#include <imp_utils.h>

#include "osd.h"
#include "venc-common.h"
#include "bgramapinfo.h"

#include "SDL.h"
#include "SDL_ttf.h"


extern struct chn_conf channel_num[];
int grpNum = 0;

static SDL_Surface *_get_FontSurface(char *string);
static void *osd_time(void *p);
static SDL_Surface *_get_LogoSurface(char *path_logo);

int venc_osd_show(osd_config_t *osd, int grpNum)
{
    int ret;
    ret = IMP_OSD_ShowRgn(osd->prHander[OSD_RGN_HANDER_0], grpNum, OSD_SHOW_ENABLE);
    if (ret != 0)
    {
        printf("IMP_OSD_ShowRgn 0 timeStamp error\n");
        return -1;
    }
    ret = IMP_OSD_ShowRgn(osd->prHander[OSD_RGN_HANDER_1], grpNum, OSD_SHOW_ENABLE);
    if (ret != 0)
    {
        printf("IMP_OSD_ShowRgn 1 timeStamp error\n");
        return -1;
    }
    ret = IMP_OSD_ShowRgn(osd->prHander[OSD_RGN_HANDER_2], grpNum, OSD_SHOW_ENABLE);
    if (ret != 0)
    {
        printf("IMP_OSD_ShowRgn 2 timeStamp error\n");
        return -1;
    }

    return 0;

}

IMPRgnHandle *venc_osd_init(int grpNum)
{
    int ret;
    IMPRgnHandle *prHander;
    IMPRgnHandle rHanderTime;
    IMPRgnHandle rHanderFont;
    IMPRgnHandle rHanderLogo;

    prHander = malloc(OSD_RGN_HANDER_MAX * sizeof(IMPRgnHandle)); /* Define num max */
    if (prHander <= 0)
    {
        printf("malloc() error !\n");
        return NULL;
    }

    rHanderTime = IMP_OSD_CreateRgn(NULL);
    if (rHanderTime == INVHANDLE)
    {
        printf("IMP_OSD_CreateRgn TimeStamp error !\n");
        return NULL;
    }
    rHanderFont = IMP_OSD_CreateRgn(NULL);
    if (rHanderFont == INVHANDLE)
    {
        printf("IMP_OSD_CreateRgn TimeStamp error !\n");
        return NULL;
    }
    rHanderLogo = IMP_OSD_CreateRgn(NULL);
    if (rHanderLogo == INVHANDLE)
    {
        printf("IMP_OSD_CreateRgn TimeStamp error !\n");
        return NULL;
    }

    /* RegisterRgn hander */
    ret = IMP_OSD_RegisterRgn(rHanderTime, grpNum, NULL);
    if (ret < 0)
    {
        printf("IVS IMP_OSD_RegisterRgn failed\n");
        return NULL;
    }
    ret = IMP_OSD_RegisterRgn(rHanderFont, grpNum, NULL);
    if (ret < 0)
    {
        printf("IVS IMP_OSD_RegisterRgn failed\n");
        return NULL;
    }
    ret = IMP_OSD_RegisterRgn(rHanderLogo, grpNum, NULL);
    if (ret < 0)
    {
        printf("IVS IMP_OSD_RegisterRgn failed\n");
        return NULL;
    }

    /* Set value HanderTime] */
    IMPOSDRgnAttr rAttr_time;
    memset(&rAttr_time, 0, sizeof(IMPOSDRgnAttr));
    rAttr_time.type = OSD_REG_PIC;
    rAttr_time.rect.p0.x = OSD_TIME_X; /* dont hard code */
    rAttr_time.rect.p0.y = OSD_TIME_Y;
    rAttr_time.rect.p1.x = rAttr_time.rect.p0.x + 20 * OSD_REGION_WIDTH - 1; // p0 is start，and p1 well be epual p0+width(or heigth)-1
    rAttr_time.rect.p1.y = rAttr_time.rect.p0.y + OSD_REGION_HEIGHT - 1;
    rAttr_time.fmt = PIX_FMT_BGRA;
    rAttr_time.data.picData.pData = NULL;
    ret = IMP_OSD_SetRgnAttr(rHanderTime, &rAttr_time);
    if (ret < 0)
    {
        printf("IMP_OSD_SetRgnAttr TimeStamp error !\n");
        return NULL;
    }

    IMPOSDGrpRgnAttr grAttr_time;
    if (IMP_OSD_GetGrpRgnAttr(rHanderTime, grpNum, &grAttr_time) < 0)
    {
        printf("IMP_OSD_GetGrpRgnAttr Logo error !\n");
        return NULL;
    }
    memset(&grAttr_time, 0, sizeof(IMPOSDGrpRgnAttr));
    grAttr_time.show = 0;

    /* Disable Font global alpha, only use pixel alpha. */
    grAttr_time.gAlphaEn = 1;
    grAttr_time.fgAlhpa = 0xff;
    grAttr_time.layer = 3;
    if (IMP_OSD_SetGrpRgnAttr(rHanderTime, grpNum, &grAttr_time) < 0)
    {
        printf("IMP_OSD_SetGrpRgnAttr Logo error !\n");
        return NULL;
    }

    /* Set value HanderFont */
    SDL_Surface *surface_font = _get_FontSurface("Camera-1");
    int width = surface_font->w;
    int heigh = surface_font->h;
    IMPOSDRgnAttr rAttrFont;
    memset(&rAttrFont, 0, sizeof(IMPOSDRgnAttr));
    rAttrFont.type = OSD_REG_PIC;
    rAttrFont.rect.p0.x = SENSOR_WIDTH - OSD_FONT_X;
    rAttrFont.rect.p0.y = SENSOR_HEIGHT - OSD_FONT_Y;
    rAttrFont.rect.p1.x = rAttrFont.rect.p0.x + width; // p0 is start，and p1 well be epual p0+width(or heigth)-1
    rAttrFont.rect.p1.y = rAttrFont.rect.p0.y + heigh;
    rAttrFont.fmt = PIX_FMT_RGB555LE;
    rAttrFont.data.picData.pData = surface_font->pixels;

    ret = IMP_OSD_SetRgnAttr(rHanderFont, &rAttrFont);
    if (ret < 0)
    {
        printf("IMP_OSD_SetRgnAttr TimeStamp error !\n");
        return NULL;
    }

    IMPOSDGrpRgnAttr grAttr_font;
    if (IMP_OSD_GetGrpRgnAttr(rHanderFont, grpNum, &grAttr_font) < 0)
    {
        printf("IMP_OSD_GetGrpRgnAttr Logo error !\n");
        return NULL;
    }
    memset(&grAttr_font, 0, sizeof(IMPOSDGrpRgnAttr));
    grAttr_font.show = 0;

    /* Disable Font global alpha, only use pixel alpha. */
    grAttr_font.gAlphaEn = 1;
    grAttr_font.fgAlhpa = 0xff;
    grAttr_font.layer = 3;
    if (IMP_OSD_SetGrpRgnAttr(rHanderFont, grpNum, &grAttr_font) < 0)
    {
        printf("IMP_OSD_SetGrpRgnAttr Logo error !\n");
        return NULL;
    }

    /* Set value HanderLogo */
    SDL_Surface *surface_logo = _get_LogoSurface(LOGO_PATH); /* free logo */
    int width_logo = surface_logo->w;
    int heigh_logo = surface_logo->h;
    // printf("With [%d] & Heigh [%d]\n", width_logo, heigh_logo);
    IMPOSDRgnAttr rAttrLogo;
    memset(&rAttrLogo, 0, sizeof(IMPOSDRgnAttr));
    rAttrLogo.type = OSD_REG_PIC;
    rAttrLogo.rect.p0.x = SENSOR_WIDTH - OSD_LOGO_X; // 220-50
    rAttrLogo.rect.p0.y = SENSOR_HEIGHT - OSD_LOGO_Y;
    rAttrLogo.rect.p1.x = rAttrLogo.rect.p0.x + width_logo - 1; // p0 is start，and p1 well be epual p0+width(or heigth)-1
    rAttrLogo.rect.p1.y = rAttrLogo.rect.p0.y + heigh_logo ;
    rAttrLogo.fmt = PIX_FMT_BGRA;
    rAttrLogo.data.picData.pData = surface_logo->pixels;

    ret = IMP_OSD_SetRgnAttr(rHanderLogo, &rAttrLogo);
    if (ret < 0)
    {
        printf("IMP_OSD_SetRgnAttr TimeStamp error !\n");
        return NULL;
    }

    IMPOSDGrpRgnAttr grAttr_logo;
    if (IMP_OSD_GetGrpRgnAttr(rHanderLogo, grpNum, &grAttr_logo) < 0)
    {
        printf("IMP_OSD_GetGrpRgnAttr Logo error !\n");
        return NULL;
    }
    memset(&grAttr_logo, 0, sizeof(IMPOSDGrpRgnAttr));
    grAttr_logo.show = 0;

    /* Disable Font global alpha, only use pixel alpha. */
    grAttr_logo.gAlphaEn = 1;
    grAttr_logo.fgAlhpa = 0xff;
    grAttr_logo.layer = 3;
    if (IMP_OSD_SetGrpRgnAttr(rHanderLogo, grpNum, &grAttr_logo) < 0)
    {
        printf("IMP_OSD_SetGrpRgnAttr Logo error !\n");
        return NULL;
    }

    /* OSD start */
    ret = IMP_OSD_Start(grpNum);
    if (ret < 0)
    {
        printf("IMP_OSD_Start TimeStamp, Logo, Cover and Rect error !\n");
        return NULL;
    }

    prHander[OSD_RGN_HANDER_0] = rHanderTime; /* define Rgn */
    prHander[OSD_RGN_HANDER_1] = rHanderFont;
    prHander[OSD_RGN_HANDER_2] = rHanderLogo;

    return prHander;
}

int vnec_osd_exit(IMPRgnHandle *prHander, int grpNum)
{
    int ret;

    ret = IMP_OSD_ShowRgn(prHander[OSD_RGN_HANDER_0], grpNum, OSD_SHOW_DISABLE);
    if (ret < 0)
    {
        printf("IMP_OSD_ShowRgn 0 close timeStamp error\n");
        return -1;
    }

    ret = IMP_OSD_ShowRgn(prHander[OSD_RGN_HANDER_1], grpNum, OSD_SHOW_DISABLE);
    if (ret < 0)
    {
        printf("IMP_OSD_ShowRgn 0 close timeStamp error\n");
        return -1;
    }
    ret = IMP_OSD_ShowRgn(prHander[OSD_RGN_HANDER_2], grpNum, OSD_SHOW_DISABLE);
    if (ret < 0)
    {
        printf("IMP_OSD_ShowRgn 0 close timeStamp error\n");
        return -1;
    }

    /* UnRegisterRgn */
    ret = IMP_OSD_UnRegisterRgn(prHander[OSD_RGN_HANDER_0], grpNum);
    if (ret < 0)
    {
        printf("IMP_OSD_UnRegisterRgn 0 timeStamp error\n");
        return -1;
    }

    ret = IMP_OSD_UnRegisterRgn(prHander[OSD_RGN_HANDER_1], grpNum);
    if (ret < 0)
    {
        printf("IMP_OSD_UnRegisterRgn 0 timeStamp error\n");
        return -1;
    }

    ret = IMP_OSD_UnRegisterRgn(prHander[OSD_RGN_HANDER_2], grpNum);
    if (ret < 0)
    {
        printf("IMP_OSD_UnRegisterRgn Cover error\n");
    }

    /* Destroy Rgn */
    IMP_OSD_DestroyRgn(prHander[OSD_RGN_HANDER_0]);
    IMP_OSD_DestroyRgn(prHander[OSD_RGN_HANDER_1]);
    IMP_OSD_DestroyRgn(prHander[OSD_RGN_HANDER_2]);

    ret = IMP_OSD_DestroyGroup(grpNum);
    if (ret < 0)
    {
        printf("IMP_OSD_DestroyGroup(0) error\n");
        return -1;
    }

    free(prHander);

    return 0;
}

/* venc_startOSD(IPC_OSD_Conf_t *param, int group) */
int venc_startOSD(osd_config_t *osd, int grpNum)
{
    int i, ret;

    /* Create group OSD */
    if (IMP_OSD_CreateGroup(grpNum) < 0)
    {
        printf("IMP_OSD_CreateGroup (%d) err !\n", grpNum);
        return -1;
    }

    /* Init OSD */
    osd->prHander = venc_osd_init(grpNum);
    if (osd->prHander <= 0)
    {
        printf("OSD init err !\n");
        return -1;
    }

    /* Bind */
    IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};
    for (i = 0; i < 2; i++)
    {
        ret = IMP_System_Bind(&channel_num[i].framesource_chn, &osdcell);
        if (ret < 0)
        {
            printf("Bind Framesource channel & OSD err !\n");
            return -1;
        }

        ret = IMP_System_Bind(&osdcell, &channel_num[i].imp_encoder);
        if (ret < 0)
        {
            printf("Bind OSD & channel encoder err !\n");
            return -1;
        }
    }
    /* Create OSD bgramap thread */
    pthread_t osd_id;

    /* set framesouce depth  */
    IMP_FrameSource_SetFrameDepth(0, 0); // group 0

    ret = pthread_create(&osd_id, NULL, osd_time, osd);
    if (ret)
    {
        printf("Create thread OSD err !\n");
        return -1;
    }

    return 0;
}

int venc_stopOSD(osd_config_t *osd, int grpNum)
{
    int ret, i;
    IMPCell osdcell = {DEV_ID_OSD, grpNum, 0};

    /* Unbind */
    for (i = 0; i < 2; i++)
    {
        ret = IMP_System_UnBind(&osdcell, &channel_num[i].imp_encoder);
        if (ret < 0)
        {
            printf("UnBind OSD & channel encoder err !\n");
            return -1;
        }

        ret = IMP_System_UnBind(&channel_num[i].framesource_chn, &osdcell);
        if (ret < 0)
        {
            printf("UnBind Framesource & OSD err !\n");
            return -1;
        }
    }
    /* Exit OSD */
    ret = vnec_osd_exit(osd->prHander, grpNum);
    if (ret < 0)
    {
        printf("OSD exit err !\n");
        return -1;
    }

    return 0;
}

/**
 * @brief static func
 *
 */
static SDL_Surface *_get_FontSurface(char *string)
{
    TTF_Font *font;
    SDL_Surface *text;
    SDL_Surface *temp;

    /* Initialize the TTF library */
    if (TTF_Init() < 0)
    {
        fprintf(stderr, "Couldn't initialize TTF: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    font = TTF_OpenFont(FONT_PATH, 32);
    if (font == NULL)
    {
        fprintf(stderr, "Couldn't open TTF: %s\n", SDL_GetError());
        return NULL;
    }

    SDL_Color forecol = {0xFF, 0xFF, 0xFF, 0};
    text = TTF_RenderUTF8_Solid(font, string, forecol);

    SDL_PixelFormat *fmt;
    fmt = (SDL_PixelFormat *)malloc(sizeof(SDL_PixelFormat));
    memset(fmt, 0, sizeof(SDL_PixelFormat));
    fmt->BitsPerPixel = 16;
    fmt->BytesPerPixel = 2;
    fmt->Rmask = 0xff000000; // 0x00FF0000
    fmt->Gmask = 0x0000ff00; // 0x0000FF00
    fmt->Bmask = 0x00ff00ff; // 0x000000FF
    fmt->alpha = 0xff;

    temp = SDL_ConvertSurface(text, fmt, 0);

    SDL_FreeSurface(text);
    TTF_CloseFont(font);
    TTF_Quit();

    return temp;
}

static SDL_Surface *_get_LogoSurface(char *path_logo)
{
    SDL_Surface *dataLogo;

    dataLogo = SDL_LoadBMP(path_logo);
    if (dataLogo == NULL)
    {
        printf("SDL_LoadBMP err !\n");
    }

    return dataLogo;
}

static void *osd_time(void *p)
{
    int ret;
    /*generate time*/
    char DateStr[40];
    time_t currTime;
    struct tm *currDate;
    unsigned i = 0, j = 0;
    void *dateData = NULL;
    osd_config_t *osd = (osd_config_t*)p;

    /* free */
    uint32_t *data = malloc(OSD_LETTER_NUM * OSD_REGION_HEIGHT * OSD_REGION_WIDTH * sizeof(uint32_t));
    if (data == NULL)
    {
        printf("valloc data error\n");
        return NULL;
    }
    IMPOSDRgnAttrData rAttrData;

    ret = venc_osd_show(osd, grpNum);
    if (ret < 0)
    {
        printf("OSD show error\n");
        return NULL;
    }
    while (pthread.b_osd)
    {
        int penpos_t = 0;
        int fontadv = 0;

        time(&currTime);
        currDate = localtime(&currTime);
        memset(DateStr, 0, 40);
        strftime(DateStr, 40, "%Y-%m-%d %I:%M:%S", currDate);
        for (i = 0; i < OSD_LETTER_NUM; i++)
        {
            switch (DateStr[i])
            {
            case '0' ... '9':
                dateData = (void *)gBgramap[DateStr[i] - '0'].pdata;
                fontadv = gBgramap[DateStr[i] - '0'].width;
                penpos_t += gBgramap[DateStr[i] - '0'].width;
                break;
            case '-':
                dateData = (void *)gBgramap[10].pdata;
                fontadv = gBgramap[10].width;
                penpos_t += gBgramap[10].width;
                break;
            case ' ':
                dateData = (void *)gBgramap[11].pdata;
                fontadv = gBgramap[11].width;
                penpos_t += gBgramap[11].width;
                break;
            case ':':
                dateData = (void *)gBgramap[12].pdata;
                fontadv = gBgramap[12].width;
                penpos_t += gBgramap[12].width;
                break;
            default:
                break;
            }

            for (j = 0; j < OSD_REGION_HEIGHT; j++)
            {
                memcpy((void *)((uint32_t *)data + j * OSD_LETTER_NUM * OSD_REGION_WIDTH + penpos_t),
                       (void *)((uint32_t *)dateData + j * fontadv), fontadv * sizeof(uint32_t));
            }
        }

        rAttrData.picData.pData = data;
        IMP_OSD_UpdateRgnAttrData(osd->prHander[0], &rAttrData);

        sleep(1);
    }

    return NULL;
}
