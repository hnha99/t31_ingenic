Thêm hàm sau để lấy data picture JPEG từ luông stream: 

char *venc_get_jpeg_snap(int chn) 

{
    int ret; 

    char *data = NULL; 

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

    /* Get JPEG Snap */ 

    ret = IMP_Encoder_GetStream(channel_num[chn].index, &stream, 1); 
    if (ret < 0) 
    { 
        printf("IMP_Encoder_GetStream() failed\n"); 
        return NULL; 
    } 

    IMPEncoderPack *pack = &stream.pack[0]; 
    if (pack->length) 

    { 
        data = qrcode_getData((uint8_t *)(stream.virAddr + pack->offset), pack->length); 
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

Trong hàm main cần viết thêm các hàm : 

//*************** init snap JPEG ****************// 

int app_snapJPEG_init() 
{ 
    int ret; 

    ret = venc_framesource_init(CHANNEL_2); 
    printf("Framesource JPEG init!\n"); 
    if (ret < 0) 
    { 
        printf("Framesource JPEG init error!\n"); 
        return -1; 
    } 

    ret = venc_jpeg_init(CHANNEL_2, true); 
    printf("JPEG init----------!\n"); 
    if (ret < 0) 
    { 
        printf("JPEG init error!\n"); 
        return -1; 
    } 

    ret = IMP_System_Bind(&channel_num[CHANNEL_2].framesource_chn, &channel_num[CHANNEL_2].imp_encoder); 
    printf("Bind channel JPEG (%d) !\n", CHANNEL_2); 
    if (ret < 0) 
    { 
        printf("Bind channel JPEG (%d) err !\n", CHANNEL_2); 
        return -1; 
    } 

    ret = venc_framesource_streamon(CHANNEL_2); 
    printf("Framesource JPEG stream on!\n"); 

    if(ret < 0) 
    { 
        printf("Framesource JPEG streamon error!\n"); 
        return -1; 
    } 

    return 0; 

} 

//*************** exit snap JPEG ***************// 

int app_snapJPEG_exit() 
{ 
    int ret; 

    /* snap stream off */ 

    ret = venc_framesource_streamoff(CHANNEL_2); 
    if (ret < 0) 
    { 
        printf("Framesource stream off channel err (%d)!\n", ret); 
        return -1; 
    } 

    /* Unbin */ 

    ret = IMP_System_UnBind(&channel_num[CHANNEL_2].framesource_chn, &channel_num[CHN2_EN].imp_encoder); 
    if (ret < 0) 
    { 
        printf("Unbin framesource & channel encoder err (%d)!\n", ret); 
        return -1; 
    } 

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

//************ scan QRCcode *************// 

Tạo cờ để check scan qrcode thành công hay không 

bool qrcode_flag; 

int app_scan_QRCode() 
{ 
    int ret; 
    char *return_data, *de_data; 
    char id[64], pass[64]; 

    memset(pass, 0, sizeof(pass)); 
    memset(id, 0, sizeof(id)); 
    while (qrcode_flag) 
    { 
        return_data = venc_get_jpeg_snap(CHANNEL_2); 

        if (return_data) 

        { 

            de_data = b64_decode(return_data, strlen(return_data)); 
            ret = qrcode_setData(de_data, id, pass); 
            if (ret == 0) 
                break; 

        } 
        printf("Scaning...\n\n"); 
    } 
    qrcode_saveFile(id, pass); 
    printf("Save ID[%s] & PASS[%s] OK!\n", id, pass); 

    return 0; 

} 


Khi cần scan chỉ cần chạy theo trình tự sau: 

bật cờ qrcode_flag 
init snap JPEG 
scan QRcode 
exit snap JPEG 
tắt cờ qrcdoe_flag 

 
NOTE: 

Channel dùng để snap jpeg không được trùng với channel đang stream. 
Format wifi: "WIFI:T:WPA;P:pass;S:id;H:false;"
Encrypt & decrypt: base64

OSD logo: 

Format file logo: file bitmap, chuẩn bgra 
File có width & height < 300 ( tùy theo độ phân giải của cam ). 
Công thức osd: 

 SDL_Surface *surface_logo = _get_LogoSurface(LOGO_PATH); 

 int width_logo = surface_logo->w; 
 int heigh_logo = surface_logo->h; 

 printf("With [%d] & Heigh [%d]\n", width_logo, heigh_logo); 

 IMPOSDRgnAttr rAttrLogo; 
 memset(&rAttrLogo, 0, sizeof(IMPOSDRgnAttr)); 
 rAttrLogo.type = OSD_REG_PIC; rAttrLogo.rect.p0.x = 100; 
 rAttrLogo.rect.p0.y = 100; 
 rAttrLogo.rect.p1.x = rAttrLogo.rect.p0.x + width_logo - 1; 
 rAttrLogo.rect.p1.y = rAttrLogo.rect.p0.y + heigh_logo; 
 rAttrLogo.fmt = PIX_FMT_BGRA; 
 rAttrLogo.data.picData.pData = surface_logo->pixels; 


ifconfig wlan0 up
ifconfig wlan0 down

vi /etc/resolv.conf

PING 8.8.8.8 (8.8.8.8): 56 data bytes
ping: sendto: Network is unreachable


PING google.com (142.250.66.110): 56 data bytes
ping: sendto: Network is unreachable


pkill -9 app



cp -r /tmp/t31-ingenic/nhanh7/times.ttf ./

cp -r /tmp/t31-ingenic/nhanh7/TextLogo_136x22.bmp ./

cp -r /tmp/t31-ingenic/nhanh7/app ./

cp -r /tmp/t31-ingenic/nhanh7/start_wlan0 /tmp