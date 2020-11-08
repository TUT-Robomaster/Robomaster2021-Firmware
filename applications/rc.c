#include <rtthread.h>
#include <rtdevice.h>
#include "mytype.h"

#define DBUS "uart1"

static uint8_t dbus_rx_buffer[18];//串口接收缓冲区
rc_info_t rc; //遥控器通道结构体变量
void RC_Process(rc_info_t *rc,uint8_t *dbus_rx_buffer);
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
	rt_err_t result;
	rt_device_read(dev, 0, dbus_rx_buffer, size);
	RC_Process(&rc,dbus_rx_buffer);
	return result;
}

rt_err_t dbus_open(void)
{
	rt_err_t result;
	dbus_init();
	if(!dbus)
	{
		rt_kprintf("find %s failed! \n",DBUS);
		return RT_ERROR;
	}
	/* 设置接收回调函数 */
  rt_device_set_rx_indicate(dbus, dbus_input);

	return result;
}
