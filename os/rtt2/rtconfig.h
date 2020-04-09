/* RT-Thread config file */
#ifndef __RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/* PRIORITY_MAX */
#define RT_THREAD_PRIORITY_MAX		8

/* Tick per Second */
#define RT_TICK_PER_SECOND			(1000 / OS_TICK_MS)

/* RT_ALIGN_SIZE*/
#define RT_ALIGN_SIZE				4

/* RT_NAME_MAX*/
#define RT_NAME_MAX					8

//Using RT-Thread components initialization
//#define RT_USING_COMPONENTS_INIT


#if OS_SIZE_TINY
#define RT_TINY_SIZE
#define RT_USING_CPU_FFS
#endif

#if 0//DEBUG_ENABLE
/* SECTION: RT_DEBUG */
/* Thread Debug */
#define RT_DEBUG
#define RT_THREAD_DEBUG

#define RT_USING_OVERFLOW_CHECK
#endif

/* Using Hook */
#define RT_USING_HOOK

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

#if DEVICE_ENABLE
/* SECTION: Device System */
/* Using Device System */
#define RT_USING_DEVICE
#define RT_USING_UART1
#endif

#if CONSOLE_ENABLE
/* SECTION: Console options */
#define RT_USING_CONSOLE
/* the buffer size of console*/
#define RT_CONSOLEBUF_SIZE	128

/* SECTION: finsh, a C-Express shell */
#define RT_USING_FINSH
/* Using symbol table */
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#endif

#if FS_ENABLE
/* SECTION: device filesystem */
/* #define RT_USING_DFS */ 
#define RT_USING_DFS_ELMFAT
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_DRIVES			1
#define RT_DFS_ELM_USE_LFN			2
#define RT_DFS_ELM_MAX_LFN			255
#define RT_DFS_ELM_MAX_SECTOR_SIZE  512

/* the max number of mounted filesystem */
#define DFS_FILESYSTEMS_MAX			2
/* the max number of opened files 		*/
#define DFS_FD_MAX					4
#endif

/* SECTION: lwip, a lighwight TCP/IP protocol stack */
/* #define RT_USING_LWIP */


#endif
