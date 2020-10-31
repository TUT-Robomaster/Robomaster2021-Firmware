#include "mytype.h"
#include "arm_math.h"
#include "rcVariables.h"
#define FILTER_BUF_LEN		5
#define YAW_CORRECTING_ANGLE 5461
#define PIT_CORRECTING_ANGLE 2730
#define YAW_OFFSET_ANGLE 0
#define PIT_OFFSET_ANGLE 0


#define THREAD_PRIORITY 5
#define STACK_SIZE 512
#define TIMESLICE 1

static rt_uint8_t gimbalTask_stack[STACK_SIZE];
struct rt_thread gimbalCalc_thread;


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

moto_measure_t moto_pit;
moto_measure_t moto_yaw;
moto_measure_t moto_poke;	//拨弹电机

extern void Gimbal_PID_Init(void);
extern float pid_calc(arm_pid_instance_f32* pid,float get,float set);
extern arm_pid_instance_f32 pid_yaw;
extern arm_pid_instance_f32 pid_pit;
extern arm_pid_instance_f32 pid_poke;
extern rc_info_t rc;
extern int16_t v1;
void GimbalInit(void)
{
	Gimbal_PID_Init();
	moto_yaw.correcting_angle = YAW_CORRECTING_ANGLE; 
	moto_pit.correcting_angle = PIT_CORRECTING_ANGLE;
	
	moto_yaw.offset_angle = YAW_OFFSET_ANGLE; 
	moto_pit.offset_angle = PIT_OFFSET_ANGLE;
}

void gimbalCalc()
{
	GimbalInit();
	while(1)
	{
	v1 = pid_calc(&pid_yaw,moto_pit.speed_rpm,rc.ch1*30);
	}
}

rt_err_t gimbalTask(void)
{
	rt_err_t result;
	result = rt_thread_init(&gimbalCalc_thread,
														"can_write",
														gimbalCalc,
														RT_NULL,
														gimbalTask_stack,
														STACK_SIZE,
														THREAD_PRIORITY,
														TIMESLICE);
	if(result == RT_EOK)
		rt_thread_startup(&gimbalCalc_thread);
	return result;
}
