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

void can1_init(void)
{
	can1_dev = rt_device_find(CAN_DEV_NAME);
	can1_control();
	rt_device_open(can1_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);

}
void can1_control(void)
{
	/* 设置 CAN 通信的波特率为 1Mbit/s*/
	rt_device_control(can1_dev, RT_CAN_CMD_SET_BAUD, (void *)CAN1MBaud);
	/* 设置 CAN 的工作模式为正常工作模式 */
	rt_device_control(can1_dev, RT_CAN_CMD_SET_MODE, (void *)RT_CAN_MODE_NORMAL);
	struct rt_can_filter_item items[1] =
{
    RT_CAN_FILTER_ITEM_INIT(0x01, 0, 0, 1, 0x01, RT_NULL, RT_NULL),
    /* 过滤 ID 为 0x01,match ID:0x100~0x1ff，hdr 为 - 1，设置默认过滤表 */
};

	struct rt_can_filter_config cfg = {1, 1, items}; /* 一共有 1 个过滤表 */
	/* 设置硬件过滤表 */
	rt_device_control(can1_dev, RT_CAN_CMD_SET_FILTER, &cfg);
}
