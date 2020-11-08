#include <rtthread.h>
#include "rtdevice.h"
#include "mytype.h"
#include "pid.h"
#define THREAD_PRIORITY 15
#define STACK_SIZE 512
#define TIMESLICE 1

static rt_uint8_t can_write_thread_stack[STACK_SIZE];
struct rt_thread can_write_thread;
extern rc_info_t rc;
//extern float pid_calc(arm_pid_instance_f32* pid,float get,float set);
extern float pid_calc(pid_t* pid, float get, float set);
//extern arm_pid_instance_f32 pid_yaw;
//extern arm_pid_instance_f32 pid_pit;
//extern arm_pid_instance_f32 pid_poke;
extern pid_t pid_yaw;
extern pid_t pid_pit;
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
extern moto_measure_t moto_poke;	//拨弹电机

void can_write_entry(void *parameter)
{
	while(1)
	{
	
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
