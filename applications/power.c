#include <rtdevice.h>
#include <board.h>

#define VCC1    GET_PIN(H, 2)
#define VCC2    GET_PIN(H, 3)
#define VCC3    GET_PIN(H, 4)
#define VCC4    GET_PIN(H, 5)

rt_err_t power_24v_on(void)
{
	rt_pin_mode(VCC1, PIN_MODE_OUTPUT);
	rt_pin_mode(VCC2, PIN_MODE_OUTPUT);
	rt_pin_mode(VCC3, PIN_MODE_OUTPUT);
	rt_pin_mode(VCC4, PIN_MODE_OUTPUT);
	rt_pin_write(VCC1, PIN_HIGH);
	rt_pin_write(VCC2, PIN_HIGH);
	rt_pin_write(VCC3, PIN_HIGH);
	rt_pin_write(VCC4, PIN_HIGH);
	return RT_EOK;
}