/*
 * File: led_state.h
 * Project: led_state
 * File Created: 01-01-1970  08:00:00:AM
 * Author: nhanh7 
 */


#ifndef __GPIO_H__
#define __GPIO_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>


#define LED_CONFIG "echo %d > /sys/class/gpio/gpio%d/value"


enum
{
    LED_PRIORITY_NONE,
    LED_PRIORITY_SD_RECORDING,       /* num 9 */
    LED_PRIORITY_CONNECTED_INTERNET, /* num 8 */
    LED_PRIORITY_NOTSD_RECORDING,    /* num 7 */
    LED_PRIORITY_NOT_SDCARD,         /* num 6 */
    LED_PRIORITY_NOT_INTERNET,       /* num 5 */
    LED_PRIORITY_CONNECTING_ROUTER,  /* num 4 */
    LED_PRIORITY_SETUP_WIFI,         /* num 3 */
    LED_PRIORITY_APP_CTRL,           /* num 2 */
    LED_PRIORITY_RESET               /* num 1 */

};
enum
{
    LED_EVENT_NONE,
    LED_EVENT_RESET,
    LED_EVENT_APP_CTRL,
    LED_EVENT_SETUP_WIFI,
    LED_EVENT_CONNECTING_ROUTER,
    LED_EVENT_NOT_INTERNET,
    LED_EVENT_NOT_SDCARD,
    LED_EVENT_NOTSD_RECORDING,
    LED_EVENT_CONNECTED_INTERNET,
    LED_EVENT_SD_RECORDING

};

enum
{
    NOTHING,
    RESET,
    APP_CTRL,
    SETUP_WIFI,
    CONNECTING_ROUTER,
    NOT_INTERNET,
    NOT_SDCARD,
    NOTSD_RECORDING,
    CONNECTED_INTERNET,
    SD_RECORDING
};


typedef struct
{
    int event;
    bool bpriority;
    int current;
    int previous;
    int old_event;
    bool b_appctrl;

    bool b_state;
} led_state_t;

extern led_state_t led_event;

int led_init(led_state_t *evt);
int led_appCtrl(bool onoff);
int led_setEvent(int evt);
int led_start();


#endif // __GPIO_H__
    