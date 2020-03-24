#include <net/lwip/opt.h>

#include <net/lwip/memp.h>
#include <net/lwip/pbuf.h>
#include <net/lwip/udp.h>
#include <net/lwip/raw.h>
#include <net/lwip/tcp.h>
#include <net/lwip/core/igmp.h>
#include <net/lwip/api.h>
#include <net/lwip/api_msg.h>
#include <net/lwip/tcpip.h>
#include <net/lwip/sys.h>
#include <net/lwip/stats.h>
#include <net/lwip/netif/etharp.h>
#include <net/lwip/core/ip_frag.h>

#include <string.h>

#define MEMP_ALIGN_SIZE(x) (LWIP_MEM_ALIGN_SIZE(x))

/** This array holds the element sizes of each pool. */
static const u16_t memp_sizes[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc)  MEMP_ALIGN_SIZE(size),
#include <net/lwip/memp_std.h>
};

/** This array holds a textual description of each pool. */
#ifdef LWIP_DEBUG
static const char *memp_desc[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc)  (desc),
#include <net/lwip/memp_std.h>
};
#endif /* LWIP_DEBUG */

/* fix time-wait tcp pcb issue */
static rt_uint32_t tcp_pcbs = 0;

/**
 * Initialize this module.
 *
 * Carves out memp_memory into linked lists for each pool-type.
 */
void
memp_init()
{
	/* nothing */
}

/**
 * Get an element from a specific pool.
 *
 * @param type the pool to get an element from
 *
 * the debug version has two more parameters:
 * @param file file name calling this function
 * @param line number of line where this function is called
 *
 * @return a pointer to the allocated memory or a NULL pointer on error
 */
void *
#if !MEMP_OVERFLOW_CHECK
memp_malloc(memp_t type)
#else
memp_malloc_fn(memp_t type, const char* file, const int line)
#endif
{
    void* ptr;
    rt_uint32_t size, level;

    size = memp_sizes[type];
    LWIP_DEBUGF(MEMP_DEBUG, ("memp malloc %s, size %d, ", memp_desc[type], memp_sizes[type]));

	level = rt_hw_interrupt_disable();
	if (type == MEMP_TCP_PCB)
	{
		if (tcp_pcbs >= MEMP_NUM_TCP_PCB)
		{
			rt_hw_interrupt_enable(level);
			return RT_NULL;
		}
		else
		{
			/* increased tcp pcb allocated number */
			tcp_pcbs ++;
		}
	}
	rt_hw_interrupt_enable(level);

    ptr = mem_Malloc(size);
	LWIP_DEBUGF(MEMP_DEBUG, ("mem 0x%x\n", ptr));

    return ptr;
}

/**
 * Put an element back into its pool.
 *
 * @param type the pool where to put mem
 * @param mem the memp element to free
 */
void
memp_free(memp_t type, void *mem)
{
	rt_uint32_t level;

	level = rt_hw_interrupt_disable();
	if (type == MEMP_TCP_PCB && mem)
	{
		tcp_pcbs --;
	}
	rt_hw_interrupt_enable(level);

    LWIP_DEBUGF(MEMP_DEBUG, ("memp free %s, mem 0x%x\n", memp_desc[type], mem));
    /* release this memory */
    mem_Free(mem);
}
