/**
 * @file qrcode.h
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef __QR_DECODE_H__
#define __QR_DECODE_H__



#include <stdint.h>
#include <string.h>

#define PATH_CONFIG_WIFI "/configs/"

int qrcode_getJpegData(const char *filename,
                       int *width, int *height,
                       void **raw);
int qrcode_scanImage(const char *filename);

/**
 * @brief qrcode
 *
 * @param data
 * @param size
 * @return result ** char*
 * result data scan from QRcode
 * free if not NULL
 */

// char *_scan(zbar_image_t *zbar);
char *qrcode_getData(uint8_t *data, size_t size);
int qrcode_setData(char *result_scan, char *id, char *pass);
int qrcode_saveFile(char *ssid, char *psk);

#endif //__QR_DECODE_H__