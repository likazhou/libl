

#if defined(__GNUC__) && (__GNUC__ > 4)    /* GNU GCC Compiler */
extern int __bss_end__;
#elif __ICCARM__
#pragma section="HEAP"
#else
#if EXTSRAM_ENABLE
extern int Image$$RW_RAM1$$ZI$$Limit;
#else
extern int Image$$RW_IRAM1$$ZI$$Limit;
#endif
#endif

#if OS_TYPE == OS_T_RTTHREAD

#include <os/rtt/clock.c>
#include <os/rtt/idle.c>
#include <os/rtt/ipc.c>
#include <os/rtt/irq.c>
#include <os/rtt/kservice.c>
#if DEBUG_MEMORY_ENABLE
#include <os/rtt/mem_debug.c>
#else
#include <os/rtt/mem.c>
#endif
#include <os/rtt/mempool.c>
#include <os/rtt/object.c>
#include <os/rtt/scheduler.c>
#include <os/rtt/slab.c>
#include <os/rtt/thread.c>
#include <os/rtt/timer.c>

#ifdef RT_USING_FINSH
#include <hi/finsh/cmd.c>
#include <hi/finsh/finsh_compiler.c>
#include <hi/finsh/finsh_error.c>
#include <hi/finsh/finsh_heap.c>
#include <hi/finsh/finsh_init.c>
#include <hi/finsh/finsh_node.c>
#include <hi/finsh/finsh_ops.c>
#include <hi/finsh/finsh_parser.c>
#include <hi/finsh/finsh_token.c>
#include <hi/finsh/finsh_var.c>
#include <hi/finsh/finsh_vm.c>
#include <hi/finsh/shell.c>
#include <hi/finsh/symbol.c>
#endif




void rt_system_power_manager(void)
{
	
	arch_IdleEntry();
}


//----------------------------------------------------------------------------
//Function Name  :tsk_OsEntry
//Description    :   进入操作系统
//Input            : None
//Output         :  None
//Return          : None
//----------------------------------------------------------------------------
os_thd_declare(OsEntry, 2048);
void tsk_OsEntry(void *args)
{

//	rt_thread_idle_hook = arch_IdleEntry;
	sys_Init();
}



//----------------------------------------------------------------------------
//Function Name  :os_Start
//Description    :   启动操作系统
//Input            : None
//Output         :  None
//Return          : None
//----------------------------------------------------------------------------

void os_Start()
{

	/* disable interrupt first */
	rt_hw_interrupt_disable();

	/* init timer system */
	rt_system_timer_init();

	/* init memory system */
#ifdef RT_USING_HEAP
#if defined(__GNUC__) && (__GNUC__ > 4)    /* GNU GCC Compiler */
	rt_system_heap_init((void*)&__bss_end__, (void*)(MCU_SRAM_BASE_ADR + MCU_SRAM_SIZE));
#elif __ICCARM__
	rt_system_heap_init(__segment_end("HEAP"), (void*)(MCU_SRAM_BASE_ADR + MCU_SRAM_SIZE));
#else
#if EXTSRAM_ENABLE
	rt_system_heap_init((void*)&Image$$RW_RAM1$$ZI$$Limit, (void*)(EXTSRAM_BASE_ADR + EXTSRAM_SIZE));
#else	
	rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)(MCU_SRAM_BASE_ADR + MCU_SRAM_SIZE));
#endif
#endif
#endif

	/* init scheduler system */
	rt_system_scheduler_init();

	/* init console to support rt_kprintf */
#ifdef RT_USING_CONSOLE
#ifdef RT_USING_DEVICE
	rt_hw_console_init();
#endif

	rt_console_set_device("uart1");

	/* show version */
	rt_show_version();
#endif

#ifdef RT_USING_DEVICE
	/* init hardware serial device */
	rt_device_init_all();
#endif

#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init();
#ifdef RT_USING_DEVICE
	finsh_set_device("uart1");
#endif
#endif

	/* init timer thread */
	rt_system_timer_thread_init();

	/* init idle thread */
	rt_thread_idle_init();

	/* init application */
	os_thd_init(OsEntry, OS_THDPRI_HIGH);

	/* start scheduler */
	rt_system_scheduler_start();
}
#endif

#if OS_TYPE == OS_T_CHNIL
#include <os/chnil/ch.c>

#include <os/chnil/chmemcore.c>
#include <os/chnil/chheap.c>
#include <os/chnil/chmboxes.c>

#include <os/chnil/syscalls.c>

void tsk_OsEntry(void *args)
{

	sys_Init();
}

void os_Start()
{

	chSysInit();
}
#endif

#if OS_TYPE == OS_T_CHRT
#include <os/chrt/chcond.c>
#include <os/chrt/chdebug.c>
#include <os/chrt/chdynamic.c>
#include <os/chrt/chevents.c>
#include <os/chrt/chmsg.c>
#include <os/chrt/chmtx.c>
#include <os/chrt/chregistry.c>
#include <os/chrt/chschd.c>
#include <os/chrt/chsem.c>
#include <os/chrt/chstats.c>
#include <os/chrt/chsys.c>
#include <os/chrt/chthreads.c>
#include <os/chrt/chtm.c>
#include <os/chrt/chtrace.c>
#include <os/chrt/chvt.c>

#include <os/chrt/chmemcore.c>
#include <os/chrt/chheap.c>
#include <os/chrt/chmboxes.c>

#include <os/chrt/syscalls.c>

void tsk_OsEntry(void *args)
{

	sys_Init();
}

void os_Start()
{

	chSysInit();
}
#endif

