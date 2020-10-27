#include <rtthread.h>
#include <rtdevice.h>

#define DBUS "uart1"
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
