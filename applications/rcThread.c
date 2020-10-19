#include "rcVariables.h"


//接收数据回调函数
static rt_err_t dbus_input(rt_device_t dev,rt_size_t size)
{
	struct rx_msg dbus_msg;
	rt_err_t result;
	dbus_msg.dev=dev;
	dbus_msg.size=size;
	
	result = rt_mq_send(&dbus_rx_mq,&dbus_msg,sizeof(dbus_msg));
	if( result == -RT_EFULL)
	{
		//消息队列满
		rt_kprintf("message queue full! \n");
	}
	return result;
}

static void dbus_thread_entry(void *parameter)
{
    struct rx_msg dbus_msg;
    rt_err_t result;

    while (1)
    {
        rt_memset(&dbus_msg, 0, sizeof(dbus_msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&dbus_rx_mq, &dbus_msg, sizeof(dbus_msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
					rt_device_read(dbus_msg.dev, 0, dbus_rx_buffer, dbus_msg.size);
					RC_Process(&rc,dbus_rx_buffer);
        }
    }
}

rt_err_t dbus_open(void)
{
	rt_err_t result;
	static char msg_pool[256];

	if(!dbus)
	{
		rt_kprintf("find %s failed! \n",DBUS);
		return RT_ERROR;
	}
	//初始化消息队列
	rt_mq_init(&dbus_rx_mq, "dbus_rx_mq",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

  /* 设置接收回调函数 */
  rt_device_set_rx_indicate(dbus, dbus_input);
	//创建dbus接受函数
	result  = rt_thread_init(&dbus_receive_thread,
							 "dbus_receive",
							 dbus_thread_entry,
							 RT_NULL,
							 dbus_thread_stack,
							 STACK_SIZE,
							 THREAD_PRIORITY,
							 TIMESLICE);
	//创建成功则启动线程
	if(result == RT_EOK)
	{
		rt_thread_startup(&dbus_receive_thread);
	}
	return result;
}

void RC_Process(rc_info_t *rc,uint8_t *dbus_rx_buffer)
{
	  rc->ch1 = (dbus_rx_buffer[0] | dbus_rx_buffer[1] << 8) & 0x07FF;
  rc->ch1 -= 1024;
  rc->ch2 = (dbus_rx_buffer[1] >> 3 | dbus_rx_buffer[2] << 5) & 0x07FF;
  rc->ch2 -= 1024;
  rc->ch3 = (dbus_rx_buffer[2] >> 6 | dbus_rx_buffer[3] << 2 | dbus_rx_buffer[4] << 10) & 0x07FF;
  rc->ch3 -= 1024;
  rc->ch4 = (dbus_rx_buffer[4] >> 1 | dbus_rx_buffer[5] << 7) & 0x07FF;
  rc->ch4 -= 1024;
	
  rc->sw1 = ((dbus_rx_buffer[5] >> 4) & 0x000C) >> 2;
  rc->sw2 = (dbus_rx_buffer[5] >> 4) & 0x0003;
  
  if ((abs(rc->ch1) > 660) || \
      (abs(rc->ch2) > 660) || \
      (abs(rc->ch3) > 660) || \
      (abs(rc->ch4) > 660))
  {
    rt_memset(rc, 0, sizeof(rc_info_t));
  }		
	
	rc->mouse.x = ((int16_t)dbus_rx_buffer[6]) | ((int16_t)dbus_rx_buffer[7] << 8);     
	rc->mouse.y = ((int16_t)dbus_rx_buffer[8]) | ((int16_t)dbus_rx_buffer[9] << 8);     
	rc->mouse.z = ((int16_t)dbus_rx_buffer[10]) | ((int16_t)dbus_rx_buffer[11] << 8);    
 
  rc->mouse.press_l = dbus_rx_buffer[12];
  rc->mouse.press_r = dbus_rx_buffer[13];
	
  rc->key = ((int16_t)dbus_rx_buffer[14]) | ((int16_t)dbus_rx_buffer[15] << 8);
	
	rc->sw  = (dbus_rx_buffer[16] | dbus_rx_buffer[17] << 8) & 0x07FF;
	rc->sw -= 1024;
}
