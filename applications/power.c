#include <rtdevice.h>
#include <board.h>

#define VCC1    GET_PIN(H, 2)
#define VCC2    GET_PIN(H, 3)
#define VCC3    GET_PIN(H, 4)
#define VCC4    GET_PIN(H, 5)

void power_24v_switch(rt_bool_t out1,rt_bool_t out2,rt_bool_t out3,rt_bool_t out4)
{
	rt_pin_mode(VCC1, PIN_MODE_OUTPUT);
	rt_pin_mode(VCC2, PIN_MODE_OUTPUT);
	rt_pin_mode(VCC3, PIN_MODE_OUTPUT);
	rt_pin_mode(VCC4, PIN_MODE_OUTPUT);
	if(out1 == 1)
		rt_pin_write(VCC1, PIN_HIGH);
	else
		rt_pin_write(VCC1, PIN_LOW);
	if(out2 == 1)
		rt_pin_write(VCC2, PIN_HIGH);
		else
	rt_pin_write(VCC2, PIN_LOW);
	if(out3 == 1)
		rt_pin_write(VCC3, PIN_HIGH);
	else
		rt_pin_write(VCC3, PIN_LOW);
	if(out4 == 1)
		rt_pin_write(VCC4, PIN_HIGH);
	else
		rt_pin_write(VCC4, PIN_LOW);;
}
