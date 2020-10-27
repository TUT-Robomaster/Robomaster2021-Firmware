#include <rtthread.h>
#include "rtdevice.h"
//#include "rcVariables.h"

//extern rc_info_t rc;
//extern void msgprocess(void);
int16_t v = 0;
//void output_thread(void *parameter)
//{
////	v = rc.ch4*300;
////	msgprocess();
//}

//rt_err_t output(int argc, char *argv[])
//{
//	rt_thread_t thread;
//	thread = rt_thread_create("output", output_thread, RT_NULL, 1024, 25, 1);
//	if (thread != RT_NULL)
//	{
//			rt_thread_startup(thread);
//	}
//	else
//	{
//			rt_kprintf("create can_rx thread failed!\n");
//	}
//	return RT_EOK;
//}