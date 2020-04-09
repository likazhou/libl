#ifndef __OS_H__
#define __OS_H__


#ifdef __cplusplus
extern "C" {
#endif

//Waiting timeout defines
#define OS_TMO_FOREVER			(0xFFFFFFFF)
#define OS_TMO_NO				0


#if OS_TYPE == OS_T_RTTHREAD
#include <os/rtt/rthw.h>
#include <os/rtt/rtthread.h>

#if LIB_MINILIBC_ENABLE
#define RT_USING_MINILIBC
#endif

typedef struct rt_thread		os_thd_t;
typedef struct rt_semaphore		os_sem_t;
typedef struct rt_mailbox		os_mbox_t;

//Thread operates
#define OS_THDPRI_IDLE			(RT_THREAD_PRIORITY_MAX - 1)
#define OS_THDPRI_LOWEST		(RT_THREAD_PRIORITY_MAX - 2)
#define OS_THDPRI_LOW			(RT_THREAD_PRIORITY_MAX - 3)
#define OS_THDPRI_NORMAL		(RT_THREAD_PRIORITY_MAX - 4)
#define OS_THDPRI_HIGH			(RT_THREAD_PRIORITY_MAX - 5)
#define OS_THDPRI_HIGHEST		(RT_THREAD_PRIORITY_MAX - 6)

#define os_thd_declare(n, x)	struct rt_thread thd_##n;						\
								__attribute__((aligned(8))) u8 stk_##n[(x) & ~7]
#define os_thd_init(n, p)																	\
								do {																	\
									rt_thread_init(&thd_##n, #n, tsk_##n, NULL, stk_##n, sizeof(stk_##n), p, 20);\
									rt_thread_startup(&thd_##n);												\
								} while (0)

#define os_thd_sleep(t)			rt_thread_delay((t) / OS_TICK_MS)
#define os_thd_slp1tick()		rt_thread_delay(1)
#define os_thd_idself()			rt_thread_self()

#define os_thd_lock()			rt_enter_critical()
#define os_thd_unlock()			rt_exit_critical()

#define os_irq_enter()			rt_interrupt_enter()
#define os_irq_leave()			rt_interrupt_leave()

#define os_sem_init(s, v)		rt_sem_init(s, NULL, v, RT_IPC_FLAG_FIFO)
#define os_sem_wait(s)			rt_sem_take(s, RT_WAITING_FOREVER)
#define os_sem_waittmo(s, t)	rt_sem_take(s, t)
#define os_sem_signal(s)		rt_sem_release(s)

#define os_mb_init(m, p, s)		rt_mb_init(m, NULL, p, s, RT_IPC_FLAG_FIFO)
#define os_mb_recv(m, v)		rt_mb_recv(m, v, RT_WAITING_FOREVER)
#define os_mb_recvtmo(m, v, t)	rt_mb_recv(m, v, t)
#define os_mb_send(m, v)		rt_mb_send(m, v)

//External Functions


#endif




#if OS_TYPE == OS_T_CHRT || OS_TYPE == OS_T_CHNIL

#if OS_TYPE == OS_T_CHRT
#include <os/chrt/ch.h>

#define OS_THDPRI_IDLE			(IDLEPRIO)
#define OS_THDPRI_LOWEST		(LOWPRIO)
#define OS_THDPRI_LOW			(LOWPRIO + 1)
#define OS_THDPRI_NORMAL		(NORMALPRIO)
#define OS_THDPRI_HIGH			(HIGHPRIO - 1)
#define OS_THDPRI_HIGHEST		(HIGHPRIO)

#define os_thd_declare(n, x)	THD_WORKING_AREA(stk_##n, x)
#define os_thd_init(n, p)		chThdStart(chThdCreateStatic(stk_##n, sizeof(stk_##n), p, tsk_##n, NULL))
#endif

#if OS_TYPE == OS_T_CHNIL
#include <os/chnil/ch.h>

#define OS_THDPRI_NORMAL		(0)
#endif

typedef thread_t				os_thd_t;
typedef semaphore_t				os_sem_t;
typedef mailbox_t				os_mbox_t;


//Thread operates
#define os_thd_sleep(t)			chThdSleepMilliseconds(t)
#define os_thd_slp1tick()		chThdSleepMilliseconds(OS_TICK_MS)
#define os_thd_idself()			chThdGetSelfX()

#define os_thd_lock()			chSysLock()
#define os_thd_unlock()			chSysUnlock()

#define os_irq_enter()			chSysLockFromISR()
#define os_irq_leave()			chSysUnlockFromISR()

#define os_sem_init(s, v)		chSemObjectInit(s, v)
#define os_sem_wait(s)			chSemWait(s)
#define os_sem_waittmo(s, t)	chSemWaitTimeout(s, t)
#define os_sem_signal(s)		chSemSignal(s)

#define os_mb_init(m, p, s)		chMBObjectInit(m, p, s)
#define os_mb_recv(m, v)		chMBFetchI(m, v)
#define os_mb_recvtmo(m, v, t)	chMBFetch(m, v, t)
#define os_mb_send(m, v)		chMBPostI(m, v)

//External Functions


#endif





#if OS_TYPE == OS_T_FREERTOS
#include <os/freertos/FreeRTOS.h>

typedef StaticTask_t			os_thd_t;
typedef StaticQueue_t			os_sem_t;
typedef StaticQueue_t			os_mbox_t;


//Thread operates
#define os_thd_declare(n, x)	StaticTask_t thd_##n;						\
								__attribute__((aligned(8))) u8 stk_##n[(x) & ~7]
#define os_thd_init(n, p)		xTaskCreateStatic(tsk_##n, #n, sizeof(stk_##n), NULL, (p), stk_##n, &thd_##n);

#define os_thd_sleep(t)			vTaskDelay((t) / OS_TICK_MS)
#define os_thd_slp1tick()		vTaskDelay(1)

#define os_thd_lock()			taskENTER_CRITICAL()
#define os_thd_unlock()			taskEXIT_CRITICAL()

#define os_irq_enter()			taskENTER_CRITICAL_FROM_ISR()
#define os_irq_leave()			taskEXIT_CRITICAL_FROM_ISR()

#define os_sem_init(s, v)		xSemaphoreCreateCountingStatic(1, v, s)
#define os_sem_wait(s)			xSemaphoreTake(s, portMAX_DELAY)
#define os_sem_waittmo(s, t)	xSemaphoreTake(s, t)
#define os_sem_signal(s)		xSemaphoreGive(s)

#define os_mb_init(m, p, s)		rt_mb_init(m, NULL, p, s, RT_IPC_FLAG_FIFO)
#define os_mb_recv(m, v)		rt_mb_recv(m, v, RT_WAITING_FOREVER)
#define os_mb_recvtmo(m, v, t)	rt_mb_recv(m, v, t)
#define os_mb_send(m, v)		rt_mb_send(m, v)

//External Functions


#endif



//External Functions
void os_Start(void);



#ifdef __cplusplus
}
#endif

#endif


