#include <libl.h>

#include "bsp_cfg.c"

#include <lib/lib.c>
#include <lib/bcd.c>
#include <lib/ecc.c>
#include <lib/math.c>
#include <lib/string.c>
#include <lib/time.c>

#include <arch/arch.c>

#if OS_TYPE
#include <os/os.c>
#include <sys/dev.c>

#include <dbg/dbg.c>

#include <lib/memory.c>

#if LIB_ZIP_ENABLE
#include <lib/zip/cceman.c>
#include <lib/zip/compressfun.c>
#include <lib/zip/compressfunnew.c>
#endif

#endif

#if DEBUG_LOG_ENABLE
#include <dbg/log.c>
#endif

#if IRQ_ENABLE
#include <sys/interrupt.c>
#endif

#if WDG_ENABLE
#include <sys/wdg.c>
#endif

#if GPIO_ENABLE
#include <sys/gpio.c>
#endif
#include <lib/buffer.c>

#if HC138_ENABLE
#include <drivers/hc138.c>
#endif

#if HC165_ENABLE
#include <drivers/hc165.c>
#endif

#if HC595_ENABLE
#include <drivers/hc595.c>
#endif

#if HT1621_ENABLE
#include <drivers/ht1621.c>
#endif

#if KEY_ENABLE
#include <hi/keypad/keypad.c>
#endif

#if BATTERY_ENABLE
#include <sys/battery.c>
#endif

#if I2C_ENABLE
#if I2C_SOFTWARE
#include <drivers/i2cbus.c>
#endif
#include <sys/i2c.c>
#endif

#if SPI_ENABLE
#if SPI_SOFTWARE
#include <drivers/spibus.c>
#endif
#include <sys/spi.c>
#endif

#if UART_ENABLE
#if SWUART_ENABLE
#include <drivers/swuart.c>
#endif
#include <sys/uart.c>
#endif

#if DQUEUE_ENABLE
#include <lib/dqueue.c>
#endif


//-------------------------------------------------------------------------
//Memory Technology Device Modules
//-------------------------------------------------------------------------
#include <mtd/flash.c>

//-------------------------------------------------------------------------
//USB Modules
//-------------------------------------------------------------------------
#if USB_ENABLE
#include <usb/usb.c>
#endif


//-------------------------------------------------------------------------
//Network Protocal Stack Modules
//-------------------------------------------------------------------------
#if TCPPS_ENABLE
#if DM9000_ENABLE
#include <drivers/dm9000.c>
#endif
#include <net/net.c>
#endif


//-------------------------------------------------------------------------
//External Parallel Interface Modules
//-------------------------------------------------------------------------
#if GUI_ENABLE
#include <hi/gui.c>
#endif

//-------------------------------------------------------------------------
//I2C Interface Modules
//-------------------------------------------------------------------------
#if PCA955X_ENABLE
#include <drivers/pca955x.c>
#endif

//-------------------------------------------------------------------------
//SPI Interface Modules
//-------------------------------------------------------------------------
#if SC16IS7X_ENABLE
#include <drivers/sc16is7x.c>
#endif
#if ATT7022_ENABLE
#include <drivers/att7022.c>
#endif
#if RN8302_ENABLE
#include <drivers/rn8302.c>
#endif
#if NRSEC3000_ENABLE
#include <drivers/nrsec3000.c>
#endif
#if VOICE_ENABLE
#include <drivers/wtv.c>
#endif
#if MIFARE_ENABLE
#include <drivers/rc522.c>
#endif

//-------------------------------------------------------------------------
//UART Interface Modules
//-------------------------------------------------------------------------
#if VK321X_ENABLE
#include <drivers/vk321x.c>
#endif
#if TDK6515_ENABLE
#include <drivers/tdk6515.c>
#endif
#if MODEM_ENABLE
#include <drivers/modem.c>
#endif
#if ATSVR_ENABLE
#include <drivers/atserver.c>
#endif
#if RDSS_ENABLE
#include <drivers/rdss.c>
#endif
#if RNSS_ENABLE
#include <drivers/rnss.c>
#endif
#if RDRN_ENABLE
#include <drivers/rdrn.c>
#endif
#if SMARTCARD_ENABLE
#include <drivers/smartcard.c>
#endif

//-------------------------------------------------------------------------
//Real Time Clock Modules
//-------------------------------------------------------------------------
#if RTC_ENABLE
#include <sys/rtc.c>
#endif

//-------------------------------------------------------------------------
//Simple FileSytem Modules
//-------------------------------------------------------------------------
#if SFS_ENABLE
#include <fs/sfs/sfs.c>
#endif

//-------------------------------------------------------------------------
//Backup Modules
//-------------------------------------------------------------------------
#if BKP_ENABLE
#include <fs/bkp/bkp.c>
#endif


//-------------------------------------------------------------------------
//File System Modules
//-------------------------------------------------------------------------
#if FS_ENABLE
#include <fs/fs.c>
#endif

//-------------------------------------------------------------------------
//Communication Channel Modules
//-------------------------------------------------------------------------
#if CHANNEL_ENABLE
#include <chl/chl.c>
#if UART_ENABLE
#include <chl/rs232.c>
#endif
#if TCPPS_ENABLE
#include <chl/sockchl.c>
#endif
#endif


//-------------------------------------------------------------------------
//Communication Protocol Modules
//-------------------------------------------------------------------------
#if PULSE_COL_ENABLE
#include <cp/lcp/pulse.c>
#endif
#if DLT645_ENABLE
#include <cp/lcp/dlt645.c>
#endif
#if PLC_ENABLE
#include <cp/lcp/plc.c>
#if GW3762_ENABLE
#include <cp/lcp/gw3762.c>
#endif
#if XCN6N12_ENABLE
#include <cp/lcp/xcn6.c>
#endif
#endif

#if DLRCP_ENABLE
#include <cp/dlrcp.c>
#endif
#if GD5100_ENABLE
#include <cp/gd5100.c>
#endif
#if GDVMS_ENABLE
#include <cp/gdvms.c>
#endif
#if GWVMS_ENABLE
#include <cp/gwvms.c>
#endif
#if GDFTS_ENABLE
#include <cp/gdfts.c>
#endif
#if NW12_ENABLE
#include <cp/nw12.c>
#endif
#if GW3761_ENABLE
#include <cp/gw3761.c>
#include <cp/gw3761_convert.c>
#endif

#if RTP_ENABLE
#include <cp/rtp/glib.c>
#include <cp/rtp/rtp.c>
#include <cp/rtp/rtp-packet.c>
#endif

//-------------------------------------------------------------------------
//System Layer
//-------------------------------------------------------------------------
#include <sys/system.c>

