#include <rtthread.h>
#include "rtdevice.h"
#include "rcVariables.h"

#define THREAD_PRIORITY 5
#define STACK_SIZE 512
#define TIMESLICE 1

static rt_uint8_t can_write_thread_stack[STACK_SIZE];
struct rt_thread can_write_thread;

extern rc_info_t rc;
extern void set_motor_voltage(uint8_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4);
int16_t v1,v2,v3,v4;
void can_write_entry(void *parameter)
{
	while(1)
	{
	//v1 = rc.ch1*30;
	v2 = rc.ch2*30;
	v3 = rc.ch3*30;
	v4 = rc.ch4*30;
	set_motor_voltage(0,v1,v2,v3,v4);
	}
}

rt_err_t can_write(void)
{
	rt_err_t result;
	result = rt_thread_init(&can_write_thread,
														"can_write",
														can_write_entry,
														RT_NULL,
														can_write_thread_stack,
														STACK_SIZE,
														THREAD_PRIORITY,
														TIMESLICE);
	if(result == RT_EOK)
		rt_thread_startup(&can_write_thread);
	return result;
}
