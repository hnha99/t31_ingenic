/**
 * @file led_driver.h
 * @author nhanh7 
 * @brief 
 * @version 0.1
 * @date 2023-01-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LED_H__
#define __LED_H__

#define LED_STATUS_OFF          (0x00)
#define LED_STATUS_ON           (0xff)

#define LED_DISABLE             (0x00)
#define LED_ENABLE              (0xff)

typedef void (*pf_led_ctrl)();

typedef struct {
	/* led attribute */
	unsigned char     blink_enable;
	unsigned char     status;
	unsigned long    duty;
	unsigned long    freq;	
	unsigned long    counter;
	unsigned char 	repeate;
	unsigned char 	round;

	/* led control function */
	pf_led_ctrl pf_ctrl;
	pf_led_ctrl pf_on;
	pf_led_ctrl pf_off;
} led_t;

// void led_init(led_t* led, pf_led_ctrl init, pf_led_ctrl on, pf_led_ctrl off);
void led_enable(led_t* led);
void led_disable(led_t* led);
void led_on(led_t* led);
void led_off(led_t* led);
void led_blink(led_t* led, unsigned long freq, unsigned long duty);
void led_blink_set(led_t* led, unsigned long freq, unsigned long duty, unsigned char repeate);
void led_blink_reset(led_t* led);
unsigned char led_is_blink(led_t* led);
void led_blink_polling(led_t* led); /* this function must be called periodically 10ms */

#endif //__LED_H__
