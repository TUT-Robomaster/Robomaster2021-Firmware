#include <rtthread.h>
#include <rtdevice.h>
#define THREAD_PRIORITY 10
#define STACK_SIZE 256
#define TIMESLICE 1

#define DBUS "uart1"

//结构体定义
typedef struct
{
	/* mouse move information */
	int16_t x;
	int16_t y;
	int16_t z;
	
	/* mouse move information */
	uint8_t press_l ;
	uint8_t press_r;
} mouse_info_t; //鼠标结构体

typedef struct
{
  /* rocker channel information */
  int16_t ch1;
  int16_t ch2;
  int16_t ch3;
  int16_t ch4;

  /* left and right lever information */
  uint8_t sw1;
  uint8_t sw2;
	int16_t sw;
	
	/* PC control information */
	mouse_info_t mouse;
	uint16_t key;
} rc_info_t; //遥控器通道结构体


struct rx_msg
{
	rt_device_t dev;
	rt_size_t size;
};
//函数相关
void RC_Process(rc_info_t *rc,uint8_t *dbus_rx_buffer);
rt_err_t dbus_control(void);


rc_info_t rc; //遥控器通道结构体变量

extern rt_device_t dbus; //串口设备句柄
static struct rt_messagequeue dbus_rx_mq; //消息队列控制块
static uint8_t dbus_rx_buffer[18];//串口接收缓冲区

static rt_uint8_t dbus_thread_stack[STACK_SIZE];//线程栈
static struct rt_thread dbus_receive_thread;