/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__


/* RT_NAME_MAX*/
#define RT_NAME_MAX					8

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE				4

/* PRIORITY_MAX */
#define RT_THREAD_PRIORITY_MAX		OS_PRIO_MAX

/* Tick per Second */
#define RT_TICK_PER_SECOND			(1000 / OS_TICK_MS)

/* SECTION: RT_DEBUG */
/* Thread Debug */
#if DEBUG_ENABLE
#define RT_DEBUG
#define RT_THREAD_DEBUG

#define RT_USING_OVERFLOW_CHECK
#endif

/* Using Hook */
/* #define RT_USING_HOOK */

/* Using Software Timer */
/* #define RT_USING_TIMER_SOFT */
#define RT_TIMER_THREAD_PRIO		4
#define RT_TIMER_THREAD_STACK_SIZE	512
#define RT_TIMER_TICK_PER_SECOND	10

/* SECTION: IPC */
/* Using Semaphore*/
#define RT_USING_SEMAPHORE

/* Using Mutex */
/* #define RT_USING_MUTEX */

/* Using Event */
/* #define RT_USING_EVENT */

/* Using MailBox */
#define RT_USING_MAILBOX

/* Using Message Queue */
/* #define RT_USING_MESSAGEQUEUE */

/* SECTION: Memory Management */
/* Using Memory Pool Management*/
/* #define RT_USING_MEMPOOL */

/* Using Dynamic Heap Management */
#define RT_USING_HEAP

/* Using Small MM */
#define RT_USING_SMALL_MEM

/* SECTION: Device System */
/* Using Device System */
#if DEVICE_ENABLE
#define RT_USING_DEVICE
#define RT_USING_UART1
#endif

/* SECTION: Console options */
#if CONSOLE_ENABLE
#define RT_USING_CONSOLE
/* the buffer size of console*/
#define RT_CONSOLEBUF_SIZE			128

/* SECTION: finsh, a C-Express shell */
#define RT_USING_FINSH
/* Using symbol table */
#define FINSH_USING_SYMTAB
/* #define FINSH_USING_DESCRIPTION */
#endif

/* SECTION: device filesystem */
#define RT_USING_DFS
/* #define DFS_USING_WORKDIR */
#define RT_USING_DFS_ELMFAT

/* the max number of mounted filesystem */
#define DFS_FILESYSTEMS_MAX			3
/* the max number of opened files 		*/
#define DFS_FD_MAX					4
/* the max number of cached sector 		*/
#define DFS_CACHE_MAX_NUM   		4

/* SECTION: lwip, a lighwight TCP/IP protocol stack */
#define RT_USING_LWIP

#endif




