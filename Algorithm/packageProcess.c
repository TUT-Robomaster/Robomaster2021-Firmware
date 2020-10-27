#include "rcVariables.h"

extern struct rt_can_msg msg;

void RC_Process(rc_info_t *rc,uint8_t *dbus_rx_buffer)
{
	  rc->ch1 = (dbus_rx_buffer[0] | dbus_rx_buffer[1] << 8) & 0x07FF;
  rc->ch1 -= 1024;
  rc->ch2 = (dbus_rx_buffer[1] >> 3 | dbus_rx_buffer[2] << 5) & 0x07FF;
  rc->ch2 -= 1024;
  rc->ch3 = (dbus_rx_buffer[2] >> 6 | dbus_rx_buffer[3] << 2 | dbus_rx_buffer[4] << 10) & 0x07FF;
  rc->ch3 -= 1024;
  rc->ch4 = (dbus_rx_buffer[4] >> 1 | dbus_rx_buffer[5] << 7) & 0x07FF;
  rc->ch4 -= 1024;
	
  rc->sw1 = ((dbus_rx_buffer[5] >> 4) & 0x000C) >> 2;
  rc->sw2 = (dbus_rx_buffer[5] >> 4) & 0x0003;
  
  if ((abs(rc->ch1) > 660) || \
      (abs(rc->ch2) > 660) || \
      (abs(rc->ch3) > 660) || \
      (abs(rc->ch4) > 660))
  {
    rt_memset(rc, 0, sizeof(rc_info_t));
  }		
	
	rc->mouse.x = ((int16_t)dbus_rx_buffer[6]) | ((int16_t)dbus_rx_buffer[7] << 8);     
	rc->mouse.y = ((int16_t)dbus_rx_buffer[8]) | ((int16_t)dbus_rx_buffer[9] << 8);     
	rc->mouse.z = ((int16_t)dbus_rx_buffer[10]) | ((int16_t)dbus_rx_buffer[11] << 8);    
 
  rc->mouse.press_l = dbus_rx_buffer[12];
  rc->mouse.press_r = dbus_rx_buffer[13];
	
  rc->key = ((int16_t)dbus_rx_buffer[14]) | ((int16_t)dbus_rx_buffer[15] << 8);
	
	rc->sw  = (dbus_rx_buffer[16] | dbus_rx_buffer[17] << 8) & 0x07FF;
	rc->sw -= 1024;
}
