#include "devVariables.h"

void dbus_init(void)
{
	//查找串口设备
	dbus = rt_device_find(DBUS);
	dbus_control();//配置串口参数
	/* 以 DMA 接收及轮询发送方式打开串口设备 */
	rt_device_open(dbus, RT_DEVICE_FLAG_DMA_RX);
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
	dbus_config.bufsz = 19; //设置为18无法运行，暂时不知道为什么
	dbus_config.reserved = 0;
	
	rt_device_control(dbus,RT_DEVICE_CTRL_CONFIG,&dbus_config);
	return RT_EOK;
}
