#include <rtthread.h>
#include <rtdevice.h>
#include "mytype.h"

#define DBUS "uart1"
#define THREAD_PRIORITY 15
#define STACK_SIZE 256
#define TIMESLICE 1

struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};
static struct rt_messagequeue dbus_rx_mq; //消息队列控制块
static uint8_t dbus_rx_buffer[18];//串口接收缓冲区
rc_info_t rc; //遥控器通道结构体变量
static rt_uint8_t dbus_thread_stack[STACK_SIZE];//线程栈
void RC_Process(rc_info_t *rc,uint8_t *dbus_rx_buffer);
static struct rt_thread dbus_receive_thread;
rt_device_t dbus; //串口设备句柄

rt_err_t dbus_init(void)
{
	//查找串口设备
	dbus = rt_device_find(DBUS);
	struct serial_configure dbus_config;
	dbus_config.baud_rate = 100000;
	dbus_config.data_bits = DATA_BITS_8;
	dbus_config.stop_bits = STOP_BITS_1;
	dbus_config.parity = PARITY_NONE;
	dbus_config.bit_order = BIT_ORDER_LSB;
	dbus_config.invert = NRZ_NORMAL;
	dbus_config.bufsz = 19; //设置为18无法运行，暂时不知道为什么
	dbus_config.reserved = 0;
	rt_device_control(dbus,RT_DEVICE_CTRL_CONFIG,&dbus_config);
	/* 以 DMA 接收及轮询发送方式打开串口设备 */
	rt_device_open(dbus, RT_DEVICE_FLAG_DMA_RX);
	return RT_EOK;
}

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
	static char dbus_msg_pool[256];
	dbus_init();
	if(!dbus)
	{
		rt_kprintf("find %s failed! \n",DBUS);
		return RT_ERROR;
	}
	//初始化消息队列
	rt_mq_init(&dbus_rx_mq, "dbus_rx_mq",
               dbus_msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(dbus_msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

	//创建dbus接受函数
	result  = rt_thread_init(&dbus_receive_thread,
							 "dbus_receive",
							 dbus_thread_entry,
							 RT_NULL,
							 dbus_thread_stack,
							 STACK_SIZE,
							 THREAD_PRIORITY,
							 TIMESLICE);
	/* 设置接收回调函数 */
  rt_device_set_rx_indicate(dbus, dbus_input);
	//创建成功则启动线程
	if(result == RT_EOK)
	{
		rt_thread_startup(&dbus_receive_thread);
	}

	return result;
}
