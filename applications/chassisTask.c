#include "arm_math.h"
#include "mytype.h"
#include "pid.h"
#define MAX_CHASSIS_VX_SPEED			14000//3200//150
#define MAX_CHASSIS_VY_SPEED			14000//4000//150
#define MAX_CHASSIS_OMEGA_SPEED		10000//300

float MAX_WHEEL_SPEED = 6000;
#define CHASSIS_OLD
#define MyAbs(x) 	( (x>0) ? (x) : (-x) )
#define THREAD_PRIORITY 15
#define STACK_SIZE 1024
#define TIMESLICE 1

static rt_uint8_t chassisTask_stack[STACK_SIZE];
struct rt_thread chassisTask;

pid_t pid_macnum[4];
moto_measure_t moto_macnum[4];
int16_t current_macnum[4];
int16_t set_spd_macnum[4];

extern rc_info_t rc;
extern float pid_calc(pid_t* pid, float get, float set);
void Mecanum_calc(float vx, float vy, float omega, const int each_max_spd, int16_t speed[]);
void Chassis_PID_Init(void)
{
	for(int i = 0;i<4;i++)
	PID_struct_init(&pid_macnum[i], POSITION_PID, 30000, 5000,
									CHASSIS_KP, CHASSIS_KI, CHASSIS_KD);
}
double rcch[3];
void chassisTask_entry(void *parameter)
{
	Chassis_PID_Init();
	while(1)
	{
		rcch[0] = -rc.ch1*9.0;
		rcch[1] = -rc.ch2*9.0;
		rcch[2] = -rc.ch3*9.0;
		Mecanum_calc(rcch[0], rcch[1], rcch[2], MAX_WHEEL_SPEED, set_spd_macnum);
		for(int i = 0;i<4;i++)
		current_macnum[i] = pid_calc(&pid_macnum[i],moto_macnum[i].speed_rpm,set_spd_macnum[i]);
	}
}

rt_err_t chassisTask_init(void)
{
	rt_err_t result;
	result = rt_thread_init(&chassisTask,
														"chassisTask",
														chassisTask_entry,
														RT_NULL,
														chassisTask_stack,
														STACK_SIZE,
														THREAD_PRIORITY,
														TIMESLICE);
	if(result == RT_EOK)
		rt_thread_startup(&chassisTask);
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
	if (max > each_max_spd){
		rate = each_max_spd / max;
		for(i=0; i<4; i++)
			buf[i] *= rate;
	}
	//output
	memcpy(speed, buf, sizeof(s16)*4); 
}
