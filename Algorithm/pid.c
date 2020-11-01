#include "arm_math.h"
#include "pid.h"
#include "mytype.h"
//arm_pid_instance_f32 pid_yaw;
//arm_pid_instance_f32 pid_pit;
//arm_pid_instance_f32 pid_poke;
pid_t pid_yaw;
pid_t pid_pit;
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
extern moto_measure_t moto_poke;	//拨弹电机

/*参数初始化--------------------------------------------------------------*/
static void pid_param_init(
    pid_t *pid, 
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    float 	kp, 
    float 	ki, 
    float 	kd)
{
    
    pid->IntegralLimit = intergral_limit;
    pid->MaxOutput = maxout;
    pid->pid_mode = mode;
    
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
    
}

static void pid_reset(pid_t	*pid, float kp, float ki, float kd)
{
    pid->p = kp;
    pid->i = ki;
    pid->d = kd;
}

/*pid总体初始化-----------------------------------------------------------------*/
void PID_struct_init(
    pid_t* pid,
    uint32_t mode,
    uint32_t maxout,
    uint32_t intergral_limit,
    
    float 	kp, 
    float 	ki, 
    float 	kd)
{
    /*init function pointer*/
    pid->f_param_init = pid_param_init;
    pid->f_pid_reset = pid_reset;
//	pid->f_cal_pid = pid_calc;	
//	pid->f_cal_sp_pid = pid_sp_calc;	//addition
		
    /*init pid param */
    pid->f_param_init(pid, mode, maxout, intergral_limit, kp, ki, kd);
	
}

void Gimbal_PID_Init(void)
{
//	arm_pid_init_f32(&pid_yaw,0);
//	arm_pid_init_f32(&pid_pit,0);
//	arm_pid_init_f32(&pid_poke,0);
//	pid_yaw.Kp = GIMBAL_YAW_KP;
//	pid_yaw.Ki = GIMBAL_YAW_KI;
//	pid_yaw.Kd = GIMBAL_YAW_KD;
//	pid_pit.Kp = GIMBAL_PIT_KP;
//	pid_pit.Ki = GIMBAL_PIT_KI;
//	pid_pit.Kd = GIMBAL_PIT_KD;
	PID_struct_init(&pid_yaw, POSITION_PID, 30000, 5000,
									GIMBAL_YAW_KP, GIMBAL_YAW_KI, GIMBAL_YAW_KD);
	PID_struct_init(&pid_pit, POSITION_PID, 30000, 5000,
									GIMBAL_PIT_KP, GIMBAL_PIT_KI, GIMBAL_PIT_KD);
}
//float err;
//float pid_calc(arm_pid_instance_f32* pid,float get,float set)
//{
//	float out;
//	err = get - set;
//	out = arm_pid_f32(pid,err);
//	return out;
//}

void abs_limit(float *a, float ABS_MAX){
    if(*a > ABS_MAX)
        *a = ABS_MAX;
    if(*a < -ABS_MAX)
        *a = -ABS_MAX;
}

float pid_calc(pid_t* pid, float get, float set){
    pid->get[NOW] = get;
    pid->set[NOW] = set;
    pid->err[NOW] = set - get;	//set - measure
    if (pid->max_err != 0 && ABS(pid->err[NOW]) >  pid->max_err  )
		return 0;
	if (pid->deadband != 0 && ABS(pid->err[NOW]) < pid->deadband)
		return 0;
    
    if(pid->pid_mode == POSITION_PID) //位置式p
    {
        pid->pout = pid->p * pid->err[NOW];
        pid->iout += pid->i * pid->err[NOW];
        pid->dout = pid->d * (pid->err[NOW] - pid->err[LAST] );
        abs_limit(&(pid->iout), pid->IntegralLimit);
        pid->pos_out = pid->pout + pid->iout + pid->dout;
        abs_limit(&(pid->pos_out), pid->MaxOutput);
        pid->last_pos_out = pid->pos_out;	//update last time 
    }
    else if(pid->pid_mode == DELTA_PID)//增量式P
    {
        pid->pout = pid->p * (pid->err[NOW] - pid->err[LAST]);
        pid->iout = pid->i * pid->err[NOW];
        pid->dout = pid->d * (pid->err[NOW] - 2*pid->err[LAST] + pid->err[LLAST]);
        
        abs_limit(&(pid->iout), pid->IntegralLimit);
        pid->delta_u = pid->pout + pid->iout + pid->dout;
        pid->delta_out = pid->last_delta_out + pid->delta_u;
        abs_limit(&(pid->delta_out), pid->MaxOutput);
        pid->last_delta_out = pid->delta_out;	//update last time
    }
    
    pid->err[LLAST] = pid->err[LAST];
    pid->err[LAST] = pid->err[NOW];
    pid->get[LLAST] = pid->get[LAST];
    pid->get[LAST] = pid->get[NOW];
    pid->set[LLAST] = pid->set[LAST];
    pid->set[LAST] = pid->set[NOW];
    return pid->pid_mode==POSITION_PID ? pid->pos_out : pid->delta_out;
//	
}
