#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define THREAD_PRIORITY 5
#define STACK_SIZE 128
#define TIMESLICE 1

static rt_uint8_t static_chassisThread_stack[STACK_SIZE];
static struct rt_thread chassisThread;
static void chassisThread_entry(void* parameter);

static void chassisThread_entry(void* parameter)
{
}
