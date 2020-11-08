#include "arm_math.h"
#include "mytype.h"
#include "pid.h"
#define MAX_CHASSIS_VX_SPEED			14000//3200//150
#define MAX_CHASSIS_VY_SPEED			14000//4000//150
#define MAX_CHASSIS_OMEGA_SPEED		10000//300
#define YAW_CORRECTING_ANGLE 5461
#define PIT_CORRECTING_ANGLE 2730
#define YAW_OFFSET_ANGLE 0
#define PIT_OFFSET_ANGLE 0
float MAX_WHEEL_SPEED = 6000;
#define CHASSIS_OLD

#define MyAbs(x) 	( (x>0) ? (x) : (-x) )
#define THREAD_PRIORITY 15
#define STACK_SIZE 1024
#define TIMESLICE 1

static rt_uint8_t motionTask_stack[STACK_SIZE];
struct rt_thread motionTask;

pid_t pid_macnum[4];
pid_t pid_yaw;
pid_t pid_pit;
pid_t pid_poke;

moto_measure_t moto_macnum[4];
moto_measure_t moto_pit;
moto_measure_t moto_yaw;
moto_measure_t moto_poke;	//拨弹电机

int16_t current_macnum[4]={0};
int16_t set_spd_macnum[4]={0};
int16_t voltage_yaw=0;
int16_t voltage_pit=0;
int16_t current_poke=0;

extern rc_info_t rc;
extern float pid_calc(pid_t* pid, float get, float set);
void Mecanum_calc(float vx, float vy, float omega, const int each_max_spd, int16_t speed[]);
void PID_Init(void)
{
	for(int i = 0;i<4;i++)
	PID_struct_init(&pid_macnum[i], POSITION_PID, 30000, 5000,
									CHASSIS_KP, CHASSIS_KI, CHASSIS_KD);
	PID_struct_init(&pid_yaw, POSITION_PID, 30000, 5000,
									GIMBAL_YAW_KP, GIMBAL_YAW_KI, GIMBAL_YAW_KD);
	PID_struct_init(&pid_pit, POSITION_PID, 30000, 5000,
									GIMBAL_PIT_KP, GIMBAL_PIT_KI, GIMBAL_PIT_KD);
	PID_struct_init(&pid_poke, POSITION_PID, 30000, 5000,
									GIMBAL_POKE_KP, GIMBAL_POKE_KI, GIMBAL_POKE_KD);
	moto_yaw.correcting_angle = YAW_CORRECTING_ANGLE; 
	moto_pit.correcting_angle = PIT_CORRECTING_ANGLE;
	moto_yaw.offset_angle = YAW_OFFSET_ANGLE; 
	moto_pit.offset_angle = PIT_OFFSET_ANGLE;
}

double rcch[3];
void motionTask_entry(void *parameter)
{
	PID_Init();
	while(1)
	{
		rcch[0] = -rc.ch1*9.0;
		rcch[1] = -rc.ch2*9.0;
		rcch[2] = -rc.ch3*9.0;
		voltage_yaw = pid_calc(&pid_yaw,moto_yaw.angle,rc.sw+2690);
		voltage_pit = pid_calc(&pid_pit,moto_pit.angle,rc.ch4*0.9+2900);
		Mecanum_calc(rcch[0], rcch[1], rcch[2], MAX_WHEEL_SPEED, set_spd_macnum);
		current_macnum[0] = pid_calc(&pid_macnum[0],moto_macnum[0].speed_rpm,set_spd_macnum[0]);
		current_macnum[1] = pid_calc(&pid_macnum[1],moto_macnum[1].speed_rpm,set_spd_macnum[1]);
		current_macnum[2] = pid_calc(&pid_macnum[2],moto_macnum[2].speed_rpm,set_spd_macnum[2]);
		current_macnum[3] = pid_calc(&pid_macnum[3],moto_macnum[3].speed_rpm,set_spd_macnum[3]);
	}
}

rt_err_t motionTask_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&motionTask,
														"motionTask",
														motionTask_entry,
														RT_NULL,
														motionTask_stack,
														STACK_SIZE,
														THREAD_PRIORITY,
														TIMESLICE);
	if(result == RT_EOK)
		rt_thread_startup(&motionTask);
	else
		rt_kprintf("chassisTask Failed!\n");
	return result;
}

void Mecanum_calc(float vx, float vy, float omega, const int each_max_spd, int16_t speed[]){
	int16_t buf[4];
	int i;
	float max=0, rate;
	
	vx = vx > MAX_CHASSIS_VX_SPEED ? MAX_CHASSIS_VX_SPEED : vx;
	vx = vx < -MAX_CHASSIS_VX_SPEED ? -MAX_CHASSIS_VX_SPEED : vx;	
	vy = vy > MAX_CHASSIS_VY_SPEED ? MAX_CHASSIS_VY_SPEED : vy;
	vy = vy < -MAX_CHASSIS_VY_SPEED ? -MAX_CHASSIS_VY_SPEED : vy;
	omega = omega > MAX_CHASSIS_OMEGA_SPEED ? MAX_CHASSIS_OMEGA_SPEED : omega;
	omega = omega < -MAX_CHASSIS_OMEGA_SPEED ? -MAX_CHASSIS_OMEGA_SPEED : omega;
	
#ifdef DEBUG_REVERSE
	buf[0] = ( -vx + vy + omega );
	buf[1] = ( -vx - vy + omega );
	buf[2] = ( +vx + vy + omega );
	buf[3] = ( +vx - vy + omega ); 
#endif	
	
	
#ifdef CHASSIS_OLD
	buf[0] = ( -vx + vy - omega );
	buf[1] = ( -vx - vy - omega );
	buf[2] = ( +vx - vy - omega );
	buf[3] = ( +vx + vy - omega );  
#else 
	buf[0] = ( vx + vy + omega );
	buf[1] = -( vx - vy - omega );
	buf[2] = ( vx - vy + omega );
	buf[3] = -( vx + vy - omega );  
#endif

	for(i=0; i<4; i++){
		if ( MyAbs(buf[i]) > max )
			max = MyAbs(buf[i]) ;
	}
//	if (max > each_max_spd){
//		rate = each_max_spd / max;
//		for(i=0; i<4; i++)
//			buf[i] *= rate;
//	}
	//output
	memcpy(speed, buf, sizeof(s16)*4); 
}
