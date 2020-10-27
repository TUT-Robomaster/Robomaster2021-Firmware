/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/* defined the LED_G pin: PF14 */
#define LED0_PIN    GET_PIN(F, 14)
extern void msgprocess(void);
extern rt_device_t can_dev;
extern struct rt_can_msg msg;
extern rt_err_t power_24v_on(void);
extern rt_err_t dbus_open(void);
extern rt_err_t dbus_init(void);
extern rt_err_t can_sample(void);
int16_t v=30000;
int main(void)
{

	
		power_24v_on();
	
		dbus_init();
		dbus_open();
		//can1_init();
		can_sample();
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    while (count++)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
			v=1000;
			msgprocess();
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
			v=3000;
			msgprocess();
    }

    return RT_EOK;
}
