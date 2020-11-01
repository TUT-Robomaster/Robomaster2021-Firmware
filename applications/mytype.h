#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#define FILTER_BUF_LEN		5

typedef uint8_t 	u8;
typedef uint16_t 	u16;
typedef uint32_t 	u32;

typedef int8_t 		s8;
typedef int16_t 	s16;
typedef int32_t		s32;

typedef volatile uint8_t 	vu8;
typedef volatile uint16_t 	vu16;
typedef volatile uint32_t 	vu32;

typedef volatile int8_t 	vs8;
typedef volatile int16_t 	vs16;
typedef volatile int32_t	vs32;


typedef union{
	s8 		s8_fmt[8];	//for angle and omega
	u8 		u8_fmt[8];	//for angle and omega
	char 	ch_fmt[8];	//
	s16		s16_fmt[4];
	u16		u16_fmt[4];
	s32 	s32_fmt[2];
	u32 	u32_fmt[2];
	float 	f32_fmt[2];
	double 	d64_fmt;
}data_convert_ut;	//for diaobi gyro

/*CAN发送或是接收的ID*/
typedef enum
{
	CAN_YAW_FEEDBACK_ID   = 0x205,		//云台Yaw
	CAN_PIT_FEEDBACK_ID  =  0x206,			//云台Pitch
	CAN_POKE_FEEDBACK_ID  = 0x207,

	CAN_MotorLF_ID 	= 0x201,    //左前
	CAN_MotorRF_ID 	= 0x202,		//右前
	CAN_MotorLB_ID 	= 0x203,    //左后
	CAN_MotorRB_ID 	= 0x204,		//右后
}CAN_Message_ID;
/*接收到的云台电机的参数结构体*/
typedef struct{
	int16_t	 	speed_rpm;
  int16_t  	real_current;
  int16_t  	given_current;
	int16_t   real_angle;
  uint8_t  	hall;
	uint16_t 	angle;				//abs angle range:[0,8191]
	uint16_t 	last_angle;	  //abs angle range:[0,8191]
	uint16_t	correcting_angle;
	uint16_t	offset_angle;
	int32_t		round_cnt;
	int32_t		total_angle;
	u8			buf_idx;
	u16			angle_buf[FILTER_BUF_LEN];
	u16			fited_angle;
	u32			msg_cnt;
}moto_measure_t;

//遥控结构体定义
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

