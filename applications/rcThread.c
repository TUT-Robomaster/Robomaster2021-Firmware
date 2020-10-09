#include <rtthread.h>
#include <rtdevice.h>
#include "rcVariables.h"


#define DBUS "uart1"

//接收数据回调函数
static rt_err_t dbus_input(rt_device_t dev,rt_size_t size)
{
	struct rx_msg dbus_msg;
	rt_err_t result;
	dbus_msg.dev=dev;
	dbus_msg.size=size;
	
	result = rt_mq_send(&rx_mq,&dbus_msg,sizeof(dbus_msg));
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
    rt_uint32_t rx_length;
    static uint8_t rx_buffer[RT_SERIAL_RB_BUFSZ + 1];

    while (1)
    {
        rt_memset(&dbus_msg, 0, sizeof(dbus_msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &dbus_msg, sizeof(dbus_msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
					rx_length = rt_device_read(dbus_msg.dev, 0, rx_buffer, dbus_msg.size);
					rx_buffer[rx_length] = '\0';
					RC_Process(&rc,rx_buffer);
        }
    }
}

rt_err_t dbus_dma_init(void)
{
	rt_err_t ret = RT_EOK;
	static char msg_pool[256];
	//查找串口设备
	dbus = rt_device_find(DBUS);
	if(!dbus)
	{
		rt_kprintf("find %s failed! \n",DBUS);
		return RT_ERROR;
	}
	//初始化消息队列
	rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */
	dbus_control();//配置串口参数
	/* 以 DMA 接收及轮询发送方式打开串口设备 */
	rt_device_open(dbus, RT_DEVICE_FLAG_DMA_RX);
  /* 设置接收回调函数 */
  rt_device_set_rx_indicate(dbus, dbus_input);
	//创建dbus接受函数
	rt_thread_t dbus_receive_thread = rt_thread_create("dbus_receive",dbus_thread_entry,RT_NULL,512,10,1);
	//创建成功则启动线程
	if(dbus_receive_thread != RT_NULL)
	{
		rt_thread_startup(dbus_receive_thread);
	}
	else
	{
		ret = RT_ERROR;
	}
	return ret;
}

rt_err_t dbus_control(void)
{
	struct serial_configure dbus_config;
	
	dbus_config.baud_rate = 100000;
	dbus_config.data_bits = DATA_BITS_8;
	dbus_config.stop_bits = STOP_BITS_1;
	dbus_config.parity = PARITY_NONE;
	dbus_config.bit_order = BIT_ORDER_LSB;
	dbus_config.invert = NRZ_NORMAL;
	dbus_config.bufsz = 18;
	dbus_config.reserved = 0;
	
	rt_device_control(dbus,RT_DEVICE_CTRL_CONFIG,&dbus_config);
	return RT_EOK;
}

void RC_Process(rc_info_t *rc,uint8_t *rx_buffer)
{
	  rc->ch1 = (rx_buffer[0] | rx_buffer[1] << 8) & 0x07FF;
  rc->ch1 -= 1024;
  rc->ch2 = (rx_buffer[1] >> 3 | rx_buffer[2] << 5) & 0x07FF;
  rc->ch2 -= 1024;
  rc->ch3 = (rx_buffer[2] >> 6 | rx_buffer[3] << 2 | rx_buffer[4] << 10) & 0x07FF;
  rc->ch3 -= 1024;
  rc->ch4 = (rx_buffer[4] >> 1 | rx_buffer[5] << 7) & 0x07FF;
  rc->ch4 -= 1024;
	
  rc->sw1 = ((rx_buffer[5] >> 4) & 0x000C) >> 2;
  rc->sw2 = (rx_buffer[5] >> 4) & 0x0003;
  
  if ((abs(rc->ch1) > 660) || \
      (abs(rc->ch2) > 660) || \
      (abs(rc->ch3) > 660) || \
      (abs(rc->ch4) > 660))
  {
    rt_memset(rc, 0, sizeof(rc_info_t));
  }		
	
	rc->mouse.x = ((int16_t)rx_buffer[6]) | ((int16_t)rx_buffer[7] << 8);     
	rc->mouse.y = ((int16_t)rx_buffer[8]) | ((int16_t)rx_buffer[9] << 8);     
	rc->mouse.z = ((int16_t)rx_buffer[10]) | ((int16_t)rx_buffer[11] << 8);    
 
  rc->mouse.press_l = rx_buffer[12];
  rc->mouse.press_r = rx_buffer[13];
	
  rc->key = ((int16_t)rx_buffer[14]) | ((int16_t)rx_buffer[15] << 8);
	
	rc->sw  = (rx_buffer[16] | rx_buffer[17] << 8) & 0x07FF;
	rc->sw -= 1024;
}
