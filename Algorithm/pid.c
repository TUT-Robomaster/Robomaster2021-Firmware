#include "arm_math.h"
#include "pid.h"

arm_pid_instance_f32 pid_yaw;
arm_pid_instance_f32 pid_pit;
arm_pid_instance_f32 pid_poke;

void Gimbal_PID_Init(void)
{
	arm_pid_init_f32(&pid_yaw,0);
	arm_pid_init_f32(&pid_pit,0);
	arm_pid_init_f32(&pid_poke,0);
	pid_yaw.Kp = GIMBAL_YAW_KP;
	pid_yaw.Ki = GIMBAL_YAW_KI;
	pid_yaw.Kp = GIMBAL_YAW_KD;
	pid_pit.Kp = GIMBAL_PIT_KP;
	pid_pit.Ki = GIMBAL_PIT_KI;
	pid_pit.Kd = GIMBAL_PIT_KD;
}

float pid_calc(arm_pid_instance_f32* pid,float get,float set)
{
	float out,err;
	err = get - set;
	out = arm_pid_f32(pid,err);
	return out;
}
