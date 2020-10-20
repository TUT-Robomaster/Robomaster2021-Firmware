#include <rtthread.h>
#include <rtdevice.h>

#define CAN_DEV_NAME       "can1"
#define DBUS "uart1"

rt_device_t can1_dev;
struct rt_can_filter_item can1_filter;
struct rt_can_filter_config can1_filter_cfg;
void can1_control(void);

rt_device_t dbus; //串口设备句柄
rt_err_t dbus_control(void); //串口设备控制
