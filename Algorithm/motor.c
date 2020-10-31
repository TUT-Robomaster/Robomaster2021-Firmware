#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

extern rt_device_t can1_dev;
void set_moto_current(rt_int16_t iq1, rt_int16_t iq2, rt_int16_t iq3, rt_int16_t iq4, rt_int8_t id)
{
	struct	rt_can_msg current_msg={0};
	(id == 0x00)?(current_msg.id = 0x200):(current_msg.id = 0x1FF);
	current_msg.ide = CAN_ID_STD;
	current_msg.rtr = CAN_RTR_DATA;
	current_msg.len = 0x08;
	current_msg.data[0] = iq1 >> 8;
	current_msg.data[1] = iq1;
	current_msg.data[2] = iq2 >> 8;
	current_msg.data[3] = iq2;
	current_msg.data[4] = iq3 >> 8;
	current_msg.data[5] = iq3;
	current_msg.data[6] = iq4 >> 8;
	current_msg.data[7] = iq4;
	rt_device_write(can1_dev,0,&current_msg,sizeof(current_msg));
}	

void set_motor_voltage(uint8_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4)
{
	rt_size_t size;
  struct rt_can_msg voltage_msg={0};
  voltage_msg.id = (id_range == 0)?(0x1ff):(0x2ff);
  voltage_msg.ide   = CAN_ID_STD;
  voltage_msg.rtr   = CAN_RTR_DATA;
  voltage_msg.len   = 8;
  voltage_msg.data[0] = (v1>>8)&0xff;
  voltage_msg.data[1] =    (v1)&0xff;
  voltage_msg.data[2] = (v2>>8)&0xff;
  voltage_msg.data[3] =    (v2)&0xff;
  voltage_msg.data[4] = (v3>>8)&0xff;
  voltage_msg.data[5] =    (v3)&0xff;
  voltage_msg.data[6] = (v4>>8)&0xff;
  voltage_msg.data[7] =    (v4)&0xff;
	size = rt_device_write(can1_dev,0,&voltage_msg,sizeof(voltage_msg));
	if(size ==0 )
		rt_kprintf("can device write failed!\n");
}
