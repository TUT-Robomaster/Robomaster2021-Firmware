#include <rtthread.h>

typedef __packed struct
{
	/* mouse move information */
	int16_t x;
	int16_t y;
	int16_t z;
	
	/* mouse move information */
	uint8_t press_l ;
	uint8_t press_r;
} mouse_info_t;

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
} rc_info_t;

void RC_Process(rc_info_t *rc,uint8_t *rx_buffer);
