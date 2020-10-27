#include <rtthread.h>
#include <rtdevice.h>

#define CAN_DEV_NAME       "can1"
#define DBUS "uart1"

rt_device_t dbus; //串口设备句柄
rt_err_t dbus_control(void); //串口设备控制
