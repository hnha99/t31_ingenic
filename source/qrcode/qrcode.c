/**
 * @file qrcode.c
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2022-12-06
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "qrcode.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zbar.h>
#include <jpeglib.h>
#include <jerror.h>

static void print_jpeg_info(struct jpeg_decompress_struct *cinfo)
{
    printf("JPEG File Information: \n");
    printf("Image width and height: %d pixels and %d pixels.\n", cinfo->image_width, cinfo->image_height);
    printf("Color components per pixel: %d.\n", cinfo->num_components);
    printf("Color space: %d.\n", cinfo->jpeg_color_space);
    printf("Raw flag is: %d.\n", cinfo->raw_data_out);
    printf("cinfo->output_scanline flag is: %d.\n", cinfo->output_scanline);

}

/// @brief 
/// @param scan qrcode data stream 
/// @return 
static char *_scan(zbar_image_t *zbar)
{
    char *result = NULL;
    if (!zbar)
        return result;

    /* create a reader */
    zbar_image_scanner_t *scanner = zbar_image_scanner_create();
    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, zbar);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(zbar);

    if (n == 1)
    {
        for (; symbol; symbol = zbar_symbol_next(symbol))
        {
            /* do something useful with results */
            const char *data = zbar_symbol_get_data(symbol);
            zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
            printf("decoded %s symbol \"%s\"\n", zbar_get_symbol_name(typ), data);

            result = (char *)calloc(strlen(data) + 1, 1);
            strcpy(result, data);

        }
    }

    /* clean up */
    zbar_image_scanner_destroy(scanner);

    return result;
}

char *qrcode_getData(uint8_t *data, size_t size)
{
    void *raw;
    char *result = NULL;
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr err; // the error handler

    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&err);

    /* Step 1: allocate and initialize JPEG decompression object */
    jpeg_create_decompress(&cinfo);

    /* set data from buffer */
    jpeg_mem_src(&cinfo, data, size);

    /* Step 3: read file parameters with jpeg_read_header() */
    jpeg_read_header(&cinfo, TRUE);

    /* Step 4: set parameters for decompression */
    cinfo.out_color_space = JCS_GRAYSCALE;
    // cinfo.num_components = 1;

    // print_jpeg_info(&cinfo);

    /* Step 5: Start decompressor */
    jpeg_start_decompress(&cinfo);

    // int row_stride = cinfo.output_width * cinfo.output_components;
    // printf("Row Stride (%d)\n", row_stride);

    raw = (void *)malloc(cinfo.output_width * cinfo.output_height * 3);

    // step 6, read the image line by line
    unsigned bpl = cinfo.output_width * cinfo.output_components;
    JSAMPROW buf = (void *)raw;
    JSAMPARRAY line = &buf;
    for (; cinfo.output_scanline < cinfo.output_height; buf += bpl)
    {
        jpeg_read_scanlines(&cinfo, line, 1);
        /* FIXME pad out to dst->width */
    }

    /* Step 7: Finish decompression */
    jpeg_finish_decompress(&cinfo);
    /* Step 8: Release JPEG decompression object */
    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

    /* create zbar */
    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int *)"Y800");
    zbar_image_set_size(image, cinfo.image_width, cinfo.image_height);
    /* zbar free image */
    zbar_image_set_data(image, raw, cinfo.image_width * cinfo.image_height, zbar_image_free_data);

    result = _scan(image);
    /* clean up */
    zbar_image_destroy(image);

    return result;
}

int qrcode_setData(char *result_scan, char *id, char *pass)
{
    int i = 0;
    const char *delim = ":;";
    const char* SSID = ";P:";
    const char* PASS = ";S:";
    char *str_data;

    if(strstr(result_scan, PASS) == 0 || strstr(result_scan, SSID) == 0)
        return -1;

    char *tmp = calloc(strlen(result_scan) + 1, sizeof(char));
    strcpy(tmp, result_scan);
    str_data = strtok(tmp, delim);

    while (str_data != NULL)
    {
        if (str_data[i] == 'P')
        {
            str_data = strtok(NULL, delim);
            printf("Pass=[%s]\n", str_data);
            strcpy(pass, str_data);

        }
        else if (str_data[i] == 'S')
        {
            str_data = strtok(NULL, delim);
            printf("ID=[%s]\n", str_data);
            strcpy(id, str_data);
        }

        str_data = strtok(NULL, delim);
    }

    free(tmp);
    // printf("ID & PASS [%s] [%s]\n", id, pass);

    return 0;
}

int qrcode_saveFile(char *ssid, char *psk)
{
    char buf_tmp[500];

    memset(buf_tmp, 0, sizeof(buf_tmp));
    FILE *fd = fopen("/configs/wpa_supplicant.conf", "w");
    if (fd == NULL)
    {
        printf("Open file error!\n");
        return -1;
    }
    sprintf(buf_tmp,
            "ctrl_interface=/var/run/wpa_supplicant\n"
            "ap_scan=1\n"
            "network={\n"
            "\tssid=\"%s\"\n"
            "\tkey_mgmt=WPA-PSK\n"
            "\tpairwise=CCMP TKIP\n"
            "\tgroup=CCMP TKIP WEP104 WEP40\n"
            "\tpsk=\"%s\"\n"
            "\tscan_ssid=1\n"
            "\tpriority=2\n"
            "}\n",
            ssid, psk);

    // printf("Create buffer ID=[%s] PASS=[%s]\n", ssid, psk);
    fwrite(buf_tmp, strlen(buf_tmp), 1, fd);
    fclose(fd);
    

    return 0;
}


/// @brief 
/// @param scan qrcode Image 
/// @return 
int qrcode_getJpegData(const char *filename,
                       int *width, int *height,
                       void **raw)
{

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr err; // the error handler
    /* More stuff */
    FILE *infile; /* source file */
    // JSAMPARRAY buffer;        /* Output row buffer */
    // unsigned char *rowptr[1]; // pointer to an array
    int row_stride; /* physical row width in output buffer */
    if ((infile = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "can't open %s\n", filename);
        return 0;
    }

    /* Step 1: allocate and initialize JPEG decompression object */
    /* We set up the normal JPEG error routines, then override error_exit. */
    cinfo.err = jpeg_std_error(&err);
    /* Now we can initialize the JPEG decompression object. */

    jpeg_create_decompress(&cinfo);

    /* Step 2: specify data source (eg, a file) */
    jpeg_stdio_src(&cinfo, infile);

    /* Step 3: read file parameters with jpeg_read_header() */
    jpeg_read_header(&cinfo, TRUE);

    /* Step 4: set parameters for decompression */
    cinfo.out_color_space = JCS_GRAYSCALE;
    // cinfo.num_components = 1;

    print_jpeg_info(&cinfo);

    /* Step 5: Start decompressor */
    jpeg_start_decompress(&cinfo);

    *width = cinfo.image_width;
    *height = cinfo.image_height;

    row_stride = cinfo.output_width * cinfo.output_components;
    printf("Row Stride (%d)\n", row_stride);

    *raw = (void *)malloc(cinfo.output_width * cinfo.output_height * 3);

    // long counter = 0;

    // step 6, read the image line by line
    unsigned bpl = cinfo.output_width * cinfo.output_components;
    JSAMPROW buf = (void *)*raw;
    JSAMPARRAY line = &buf;
    for (; cinfo.output_scanline < cinfo.output_height; buf += bpl)
    {
        jpeg_read_scanlines(&cinfo, line, 1);
        /* FIXME pad out to dst->width */
    }
    /*
    while (cinfo.output_scanline < cinfo.output_height) {
        // Enable jpeg_read_scanlines() to fill our jdata array
        rowptr[0] = (unsigned char *)(*raw) +  // secret to method
            3* cinfo.output_width * cinfo.output_scanline;

        jpeg_read_scanlines(&cinfo, rowptr, 1);

    }*/

    /* Step 7: Finish decompression */
    jpeg_finish_decompress(&cinfo);

    /* Step 8: Release JPEG decompression object */
    /* This is an important step since it will release a good deal of memory. */
    jpeg_destroy_decompress(&cinfo);

    fclose(infile);
    /* And we're done! */
    return 1;
}

int qrcode_scanImage(const char *filename)
{
    /* create a reader */
    zbar_image_scanner_t *scanner = zbar_image_scanner_create();

    /* configure the reader */
    zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

    /* obtain image data */
    int width = 0, height = 0;
    void *raw = NULL;
    // get_data(argv[1], &width, &height, &raw);
    qrcode_getJpegData(filename, &width, &height, &raw);
    printf("load %s done\n", filename);

    /* wrap image data */
    zbar_image_t *image = zbar_image_create();
    zbar_image_set_format(image, *(int *)"Y800");
    zbar_image_set_size(image, width, height);
    zbar_image_set_data(image, raw, width * height, zbar_image_free_data);

    /* scan the image for barcodes */
    int n = zbar_scan_image(scanner, image);
    printf("Scan Image (%d)\n", n);

    /* extract results */
    const zbar_symbol_t *symbol = zbar_image_first_symbol(image);

    for (; symbol; symbol = zbar_symbol_next(symbol))
    {
        /* do something useful with results */
        const char *data = zbar_symbol_get_data(symbol);
        zbar_symbol_type_t typ = zbar_symbol_get_type(symbol);
        printf("decoded %s symbol \"%s\"\n", zbar_get_symbol_name(typ), data);

        if (n == 1)
        {
            // qrcode_saveFile(data);
        }
    }

    /* clean up */
    zbar_image_destroy(image);
    zbar_image_scanner_destroy(scanner);

    printf("Image scanner destroy!!!\n");

    return n;
}
