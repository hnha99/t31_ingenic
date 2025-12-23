/*
 * File Name: led_state.c
 * Project: T31 Ingenic
 * File Created: 01-01-1970  08:00:00:AM
 * Author: hnha99
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#include "led_driver.h"
#include "led_state.h"
#include "venc-common.h"

#define LED_RED 38
#define LED_BLUE 39

#define ON 1
#define OFF 0

led_state_t led_event;

static int led_state(int state, int led_color);
static int led_handle(int led_color, int timeon, int timeoff);
static int led_handleState(led_state_t *evt);
static void *_getled(void *args);

/* init */
int led_init(led_state_t *evt)
{
    evt->current = LED_EVENT_NONE;
    evt->event = 0;
    evt->old_event = 0;
    evt->previous = LED_PRIORITY_NONE;
    evt->bpriority = 0;
    evt->b_state = false;

    return 0;
}

/* app control led*/
int led_appCtrl(bool onoff)
{
    led_event.b_appctrl = onoff;
    return 0;
}

 /* set event led */
int led_setEvent(int evt)
{
    switch (evt)
    {
    case NOTHING:
    {
        led_event.bpriority = 0;
        led_event.event = LED_EVENT_NONE;
        break;
    }
    case RESET:
    {
        led_event.event = LED_EVENT_RESET;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_RESET;
        // printf("Led reset\n");

        break;
    }
    case APP_CTRL:
    {
        led_event.event = LED_EVENT_APP_CTRL;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_APP_CTRL;

        break;
    }
    case SETUP_WIFI:
    {
        led_event.event = LED_EVENT_SETUP_WIFI;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_SETUP_WIFI;
        // printf("Led setup wifi\n");

        break;
    }
    case CONNECTING_ROUTER:
    {
        led_event.event = LED_EVENT_CONNECTING_ROUTER;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_CONNECTING_ROUTER;
        // printf("Led connecting router\n");

        break;
    }
    case NOT_INTERNET:
    {
        led_event.event = LED_EVENT_NOT_INTERNET;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_NOT_INTERNET;
        // printf("Led not internet\n");

        break;
    }
    case NOT_SDCARD:
    {
        led_event.event = LED_EVENT_NOT_SDCARD;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_NOT_SDCARD;
        // printf("Led not sdcard\n");

        break;
    }
    case NOTSD_RECORDING:
    {
        led_event.event = LED_EVENT_NOTSD_RECORDING;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_NOTSD_RECORDING;
        // printf("Led not sd recording\n");

        break;
    }
    case CONNECTED_INTERNET:
    {
        led_event.event = LED_EVENT_CONNECTED_INTERNET;
        led_event.bpriority = 1;
        led_event.current = LED_PRIORITY_CONNECTED_INTERNET;
        // printf("Led connected internet\n");

        break;
    }
    case SD_RECORDING:
    {
        led_event.event = LED_EVENT_SD_RECORDING;
        led_event.bpriority = 0;
        led_event.current = LED_PRIORITY_SD_RECORDING;
        // printf("Led sd recording\n");

        break;
    }
    default:
        break;
    }

    return 0;
}

int led_start()
{
    pthread_t get_id;

    pthread_create(&get_id, NULL, _getled, &led_event);

    return 0;
}

//*********************** STATIC FUNC ***********************//
///////////////////////////////////////////////////////////////
//** config led **//
static int led_state(int state, int led_color)
{
    char LED_BUFF[64];
    sprintf(LED_BUFF, LED_CONFIG, state, led_color);
    system(LED_BUFF);
    return 0;
}

//* handle led *//
static int led_handle(int led_color, int timeon, int timeoff)
{
    led_state(ON, led_color);
    usleep(timeon);
    led_state(OFF, led_color);
    usleep(timeoff);
    return 0;
}

//* update handle *//
static int led_handleState(led_state_t *evt)
{

    switch (evt->event)
    {
    case LED_EVENT_NONE:
    {
        // led_state(ON, LED_BLUE);
        // led_state(OFF, LED_RED);

        break;
    }
    case LED_EVENT_RESET:
    {
        led_state(OFF, LED_BLUE);
        led_state(OFF, LED_RED);
    }

    break;
    case LED_EVENT_APP_CTRL:
    {
        if (evt->b_state == true)
        {
            led_state(OFF, LED_BLUE);
            led_state(OFF, LED_RED);
            evt->b_state = false;
        }
    }
    break;
    case LED_EVENT_SETUP_WIFI:
    {
        led_handle(LED_BLUE, 200000, 200000);
        if (evt->b_state == true)
        {
            led_state(OFF, LED_RED);
            evt->b_state = false;
            printf("STATE SETUP WIFI\n");
        }
    } 
    break;
    case LED_EVENT_CONNECTING_ROUTER:
    {
        led_handle(LED_BLUE, 1000000, 1000000);
        if (evt->b_state == true)
        {
            led_state(OFF, LED_RED);
            evt->b_state = false;
            printf("STATE CONNECTING\n");
        }
    }
    break;
    case LED_EVENT_NOT_INTERNET:
    {
        led_handle(LED_RED, 500000, 500000);
        if (evt->b_state == true)
        {
            led_state(OFF, LED_BLUE);
            evt->b_state = false;
            printf("STATE NOT INTERNET\n");
        }
    }
    break;
    case LED_EVENT_NOT_SDCARD:
    {
        led_handle(LED_RED, 1000000, 1000000);
        if (evt->b_state == true)
        {
            led_state(OFF, LED_BLUE);
            evt->b_state = false;
            printf("STATE NOT SD CARD\n");
        }
    }
    break;
    case LED_EVENT_NOTSD_RECORDING:
    {
        if (evt->b_state == true)
        {
            led_state(ON, LED_RED);
            led_state(OFF, LED_BLUE);
            evt->b_state = false;
            printf("STATE NOTSD RECORDING\n");
        }
    }
    break;
    case LED_EVENT_CONNECTED_INTERNET:
    {
        if (evt->b_state == true)
        {
            led_state(ON, LED_BLUE);
            led_state(OFF, LED_RED);
            evt->b_state = false;
            printf("STATE CONNECTED\n");
        }
    }
    break;
    case LED_EVENT_SD_RECORDING:
    {
        led_handle(LED_BLUE, 500000, 4000000);
        if (evt->b_state == true)
        {
            led_state(OFF, LED_RED);
            evt->b_state = false;
            printf("STATE SD RECORDING\n");
        }
    }
    break;

    default:
        break;
    }

    return 0;
}

/* loop get event handle led */
static void *_getled(void *args)
{
    led_event.b_appctrl = true;
    pthread.b_getevt = true;
    led_event.b_state = true;
    while (pthread.b_getevt)
    {
        if (led_event.b_appctrl == true)
        {
            if (led_event.bpriority == 0)
            {
                led_event.previous = LED_PRIORITY_NONE;
                led_event.current = LED_EVENT_NONE;
            }
            else if (led_event.current != led_event.previous)
            {
                led_event.b_state = true;
                printf("FLAG STATE\n");
            }
            if (led_event.current >= led_event.previous)
            {
                led_event.old_event = led_event.event;
                led_event.previous = led_event.current;
                led_handleState(&led_event);
                // printf("LED GET NEW EVT [%d]\n", led_event.current);
            }
            else if (led_event.current < led_event.previous)
            {
                led_event.b_state = false;
                led_event.event = led_event.old_event;
                led_handleState(&led_event);
                // printf("LED GET OLD EVT [%d]\n", led_event.old_event);
            }
        }
        else
        {
            led_event.event = LED_EVENT_NONE;
            led_handleState(&led_event);
            led_event.bpriority = 0;
        }
    }

    return NULL;
}

