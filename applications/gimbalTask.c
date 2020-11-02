#include "arm_math.h"
#include "mytype.h"
#include "pid.h"
#define YAW_CORRECTING_ANGLE 5461
#define PIT_CORRECTING_ANGLE 2730
#define YAW_OFFSET_ANGLE 0
#define PIT_OFFSET_ANGLE 0

#define THREAD_PRIORITY 15
#define STACK_SIZE 1024
#define TIMESLICE 1

static rt_uint8_t gimbalTask_stack[STACK_SIZE];
struct rt_thread gimbalCalc_thread;

pid_t pid_yaw;
pid_t pid_pit;
pid_t pid_poke;

moto_measure_t moto_pit;
moto_measure_t moto_yaw;
moto_measure_t moto_poke;	//拨弹电机

int16_t voltage_yaw;
int16_t voltage_pit;
int16_t current_poke;
extern rc_info_t rc;
extern float pid_calc(pid_t* pid, float get, float set);

void Gimbal_PID_Init(void)
{
	PID_struct_init(&pid_yaw, POSITION_PID, 30000, 5000,
									GIMBAL_YAW_KP, GIMBAL_YAW_KI, GIMBAL_YAW_KD);
	PID_struct_init(&pid_pit, POSITION_PID, 30000, 5000,
									GIMBAL_PIT_KP, GIMBAL_PIT_KI, GIMBAL_PIT_KD);
	PID_struct_init(&pid_poke, POSITION_PID, 30000, 5000,
									GIMBAL_POKE_KP, GIMBAL_POKE_KI, GIMBAL_POKE_KD);
}

void GimbalInit(void)
{
	Gimbal_PID_Init();
	moto_yaw.correcting_angle = YAW_CORRECTING_ANGLE; 
	moto_pit.correcting_angle = PIT_CORRECTING_ANGLE;
	moto_yaw.offset_angle = YAW_OFFSET_ANGLE; 
	moto_pit.offset_angle = PIT_OFFSET_ANGLE;
}

void gimbalCalc(void *parameter)
{
	GimbalInit();
	while(1)
	{
	voltage_yaw = pid_calc(&pid_yaw,moto_yaw.angle,rc.sw+2690);
	voltage_pit = pid_calc(&pid_pit,moto_pit.angle,rc.ch4*0.9+2900);
	}
}

rt_err_t gimbalTask(void)
{
	rt_err_t result;
	result = rt_thread_init(&gimbalCalc_thread,
														"gimbalTask",
														gimbalCalc,
														RT_NULL,
														gimbalTask_stack,
														STACK_SIZE,
														THREAD_PRIORITY,
														TIMESLICE);
	if(result == RT_EOK)
		rt_thread_startup(&gimbalCalc_thread);
	else
		rt_kprintf("gimbalTask Failed!\n");
	return result;
}
