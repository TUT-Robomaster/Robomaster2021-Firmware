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

extern void power_24v_switch(rt_bool_t,rt_bool_t,rt_bool_t,rt_bool_t);
extern rt_err_t dbus_open(void);
extern rt_err_t can_rx(void);
extern rt_err_t motionTask_init(void);
int main(void)
{
		power_24v_switch(1,0,1,0);
		dbus_open();
		can_rx();
		motionTask_init();
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    while (count++)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }

    return RT_EOK;
}
