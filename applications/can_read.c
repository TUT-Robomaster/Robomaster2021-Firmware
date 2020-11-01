#include <rtthread.h>
#include "rtdevice.h"
#include "mytype.h"
#define THREAD_PRIORITY 5
#define STACK_SIZE 512
#define TIMESLICE 1

#define CAN_DEV_NAME       "can1"      /* CAN 设备名称 */

static struct rt_semaphore rx_sem;     /* 用于接收消息的信号量 */
rt_device_t can1_dev;            /* CAN 设备句柄 */
struct rt_thread can1_recv;
static rt_uint8_t can1_recv_thread_stack[STACK_SIZE];//线程栈
struct rt_can_msg rxmsg = {0};
extern void get_moto_measure(moto_measure_t *moto, struct rt_can_msg can_msg);
struct rt_can_msg can_msg;
extern moto_measure_t moto_pit;
extern moto_measure_t moto_yaw;
extern moto_measure_t moto_poke;	//拨弹电机

/* 接收数据回调函数 */
static rt_err_t can_rx_call(rt_device_t dev, rt_size_t size)
{
    /* CAN 接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);
    return RT_EOK;
}

static void can_rx_thread(void *parameter)
{
    rt_err_t res;
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(can1_dev, can_rx_call);
#ifdef RT_CAN_USING_HDR
    struct rt_can_filter_item items[2] =
    {
        RT_CAN_FILTER_ITEM_INIT(0x100, 0, 0, 1, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x100~0x1ff，hdr 为 - 1，设置默认过滤表 */
        RT_CAN_FILTER_ITEM_INIT(0x200, 0, 0, 1, 0x700, RT_NULL, RT_NULL), /* std,match ID:0x200~0x2ff，hdr 为 - 1 */
    };
    struct rt_can_filter_config cfg = {2, 1, items}; /* 一共有 5 个过滤表 */
    /* 设置硬件过滤表 */
    res = rt_device_control(can1_dev, RT_CAN_CMD_SET_FILTER, &cfg);
    RT_ASSERT(res == RT_EOK);
#endif
    while (1)
    {
        /* hdr 值为 - 1，表示直接从 uselist 链表读取数据 */
        rxmsg.hdr = -1;
        /* 阻塞等待接收信号量 */
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        /* 从 CAN 读取一帧数据 */
        rt_device_read(can1_dev, 0, &rxmsg, sizeof(rxmsg));
			switch(rxmsg.id)
			{
				case CAN_YAW_FEEDBACK_ID:
					get_moto_measure(&moto_yaw, rxmsg);
				break;
				case CAN_PIT_FEEDBACK_ID:
					get_moto_measure(&moto_pit, rxmsg);
				break;
				case CAN_POKE_FEEDBACK_ID:
					get_moto_measure(&moto_poke, rxmsg);
				break;
			}
//			if(rxmsg.id == CAN_YAW_FEEDBACK_ID)
//				get_moto_measure(&moto_yaw, rxmsg);
//			else if(rxmsg.id == CAN_PIT_FEEDBACK_ID)
//				get_moto_measure(&moto_pit, rxmsg);
//			else if(rxmsg.id == CAN_POKE_FEEDBACK_ID)
//				get_moto_measure(&moto_poke, rxmsg);
    }
}

int can_rx(int argc, char *argv[])
{
		rt_err_t thread_ready;
    rt_err_t res;
    rt_size_t  size;
		static char can_msg_pool[256];
    char can_name[RT_NAME_MAX];
    if (argc == 2)
        rt_strncpy(can_name, argv[1], RT_NAME_MAX);
    else
        rt_strncpy(can_name, CAN_DEV_NAME, RT_NAME_MAX);
    /* 查找 CAN 设备 */
    can1_dev = rt_device_find(can_name);
    if (!can1_dev)
    {
        rt_kprintf("find %s failed!\n", can_name);
        return RT_ERROR;
    }

    /* 初始化 CAN 接收信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及发送方式打开 CAN 设备 */
    res = rt_device_open(can1_dev, RT_DEVICE_FLAG_INT_TX | RT_DEVICE_FLAG_INT_RX);
    RT_ASSERT(res == RT_EOK);
    /* 创建数据接收线程 */
    thread_ready = rt_thread_init(&can1_recv,
																	"can_rx",
																	can_rx_thread,
																	RT_NULL,
																	&can1_recv_thread_stack,
																	STACK_SIZE, 
																	THREAD_PRIORITY, 
																	TIMESLICE);
    if (thread_ready == RT_EOK)
    {
        rt_thread_startup(&can1_recv);
    }
    else
    {
        rt_kprintf("create can_rx thread failed!\n");
    }
    return res;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(can_rx, can device sample);
