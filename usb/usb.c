#if USB_PS_T_KEIL

#include <os/rtx/RTL.h>
#include "Usb_config.c"
#if ARCH_TYPE == ARCH_T_STM32F10X_CL
#include <mcu/stm32f10x/usbh_stm32f10x.c>
#endif
#if ARCH_TYPE == ARCH_T_STM32F20X
#include <mcu/stm32f20x/usbh_stm32f20x.c>
#endif


void usb_Init()
{

	usbh_init(0);
}

void usb_HostHandler()
{

	usbh_engine(0);
}

void usb_HostIRQ()
{

	USBH_FS_IRQHandler();
}

int usb_HostIsConnected(void *p)
{

#if USBH_HID_ENABLE
	if (usbh_hid_status(0, 0))
		return 1;
#endif
#if USBH_MSC_ENABLE
	if (usbh_msc_status(0, 0))
		return 2;
#endif
	return 0;
}

#if USBH_MSC_ENABLE
int usb_HostMscRead(void *p, u32 nSector, void *pBuf, size_t nLen)
{

	if usbh_msc_read(0, 0, nSector, pBuf, nLen) ? nLen : 0;
}

int usb_HostMscWrite(void *p, u32 nSector, const void *pBuf, size_t nLen)
{

	return usbh_msc_write(0, 0, nSector, (u8 *)pBuf, nLen) ? nLen : 0;
}
#endif


#if USBH_HID_ENABLE
int usb_Keybrd_GetData()
{

	return usbh_hid_kbd_getkey(0, 0);
}

void usb_Mouse_GetData()
{

}
#endif




#else

#if ARCH_TYPE == ARCH_T_STM32F10X_CL || ARCH_TYPE == ARCH_T_STM32F20X

#if ARCH_TYPE == ARCH_T_STM32F10X_CL
#include <mcu/stm32f10x/usblib/usb_core.c>
#include <mcu/stm32f10x/usblib/usb_bsp.c>
#include <mcu/stm32f10x/usblib/usb_hcd.c>
#include <mcu/stm32f10x/usblib/usb_hcd_int.c>

#include <mcu/stm32f10x/usblib/usbh_core.c>
#include <mcu/stm32f10x/usblib/usbh_hcs.c>
#include <mcu/stm32f10x/usblib/usbh_ioreq.c>
#include <mcu/stm32f10x/usblib/usbh_stdreq.c>

#include <mcu/stm32f10x/usblib/usbh_usr.c>

#if USBH_MSC_ENABLE
#include <mcu/stm32f10x/usblib/usbh_msc_core.c>
#include <mcu/stm32f10x/usblib/usbh_msc_bot.c>
#include <mcu/stm32f10x/usblib/usbh_msc_scsi.c>
#endif

#if USBH_HID_ENABLE
#include <mcu/stm32f10x/usblib/usbh_hid_core.c>
#include <mcu/stm32f10x/usblib/usbh_hid_keybd.c>
#include <mcu/stm32f10x/usblib/usbh_hid_mouse.c>
#endif

#endif

#if ARCH_TYPE == ARCH_T_STM32F20X

#include <mcu/stm32f20x/usblib/usb_core.c>
#include <mcu/stm32f20x/usblib/usb_bsp.c>
#include <mcu/stm32f20x/usblib/usb_hcd.c>
#include <mcu/stm32f20x/usblib/usb_hcd_int.c>

#include <mcu/stm32f20x/usblib/usbh_core.c>
#include <mcu/stm32f20x/usblib/usbh_hcs.c>
#include <mcu/stm32f20x/usblib/usbh_ioreq.c>
#include <mcu/stm32f20x/usblib/usbh_stdreq.c>

#include <mcu/stm32f20x/usblib/usbh_usr.c>

#if USBH_MSC_ENABLE
#include <mcu/stm32f20x/usblib/usbh_msc_core.c>
#include <mcu/stm32f20x/usblib/usbh_msc_bot.c>
#include <mcu/stm32f20x/usblib/usbh_msc_scsi.c>
#endif

#if USBH_HID_ENABLE
#include <mcu/stm32f20x/usblib/usbh_hid_core.c>
#include <mcu/stm32f20x/usblib/usbh_hid_keybd.c>
#include <mcu/stm32f20x/usblib/usbh_hid_mouse.c>
#endif

#endif
 

//Private Variables
USB_OTG_CORE_HANDLE		USB_OTG_Core;
USBH_HOST				    USB_Host;




void usb_Init()
{

#if USBH_MSC_ENABLE
	USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &USBH_MSC_cb, &USR_cb);
#endif
#if USBH_HID_ENABLE
	USBH_Init(&USB_OTG_Core, USB_OTG_FS_CORE_ID, &USB_Host, &HID_cb, &USR_cb);
#endif
}

void usb_HostHandler()
{

	USBH_Process(&USB_OTG_Core, &USB_Host);
}

void usb_HostIRQ()
{

	USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

void *usb_HostOpen()
{

	return &USB_OTG_Core;
}

void usb_HostClose(void *p)
{

}

int usb_HostIsConnected(void *p)
{

	return HCD_IsDeviceConnected(p) ? 1 : 0;
}

#if USBH_MSC_ENABLE
int usb_HostMscRead(void *p, u32 nSector, void *pBuf, size_t nLen)
{
	USBH_MSC_Status_TypeDef status;

	do {
		status = (USBH_MSC_Status_TypeDef)USBH_MSC_Read10(p, pBuf, nSector, nLen * 512);
		USBH_MSC_HandleBOTXfer(p, &USB_Host);
		if (HCD_IsDeviceConnected(p) == 0)
			return nLen;
	} while (status == USBH_MSC_BUSY);
	return 0;
}

int usb_HostMscWrite(void *p, u32 nSector, const void *pBuf, size_t nLen)
{
	USBH_MSC_Status_TypeDef status;

	do {
		status = (USBH_MSC_Status_TypeDef)USBH_MSC_Write10(p, (u8 *)pBuf, nSector, nLen * 512);
		USBH_MSC_HandleBOTXfer(p, &USB_Host);
		if (HCD_IsDeviceConnected(p) == 0)
			return nLen;
	} while (status == USBH_MSC_BUSY);
	return 0;
}
#endif


#if USBH_HID_ENABLE
void USR_KEYBRD_Init()
{

}

void USR_MOUSE_Init()
{

}

void USR_KEYBRD_ProcessData(u8 nData)
{

}

void USR_MOUSE_ProcessData(HID_MOUSE_Data_TypeDef *pData)
{

}
#endif

#endif



#if (ARCH_TYPE == ARCH_T_LM3S5X) || (ARCH_TYPE == ARCH_T_LM3S9X)
#include <mcu/lm3s/usblib/usblib.h>
#include <mcu/lm3s/usblib/usblibpriv.h>
#include <mcu/lm3s/usblib/device/usbdevice.h>
#include <mcu/lm3s/usblib/host/usbhost.h>
#include <mcu/lm3s/usblib/host/usbhmsc.h>
#include <mcu/lm3s/usblib/host/usbhscsi.h>

#if 1
#include <mcu/lm3s/usblib/usbmode.c>
#include <mcu/lm3s/usblib/usbtick.c>
#include <mcu/lm3s/usblib/usbdesc.c>

#include <mcu/lm3s/usblib/host/usbhostenum.c>
#include <mcu/lm3s/usblib/host/usbhscsi.c>
#include <mcu/lm3s/usblib/host/usbhmsc.c>
#endif




//*****************************************************************************
//
// The memory pool to provide to the Host controller driver.
//
//*****************************************************************************
unsigned char usb_HCDPool[128];

//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the MSC class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_USBHostMSCClassDriver,
    &g_sUSBEventDriver
};


//*****************************************************************************
//
// Hold the current state for the application.
//
//*****************************************************************************
typedef enum
{
    //
    // No device is present.
    //
    STATE_NO_DEVICE,

    //
    // Mass storage device is being enumerated.
    //
    STATE_DEVICE_ENUM,

    //
    // Mass storage device is ready.
    //
    STATE_DEVICE_READY,

    //
    // An unsupported device has been attached.
    //
    STATE_UNKNOWN_DEVICE,

    //
    // A power fault has occured.
    //
    STATE_POWER_FAULT
}
tState;
volatile tState g_eState;
volatile tState g_eUIState;

//*****************************************************************************
//
// The control table used by the uDMA controller.  This table must be aligned
// to a 1024 byte boundary.  In this application uDMA is only used for USB,
// so only the first 6 channels are needed.
//
//*****************************************************************************
tDMAControlTable g_sDMAControlTable[6] __attribute__ ((aligned(1024)));

//*****************************************************************************
//
// The current USB operating mode - Host, Device or unknown.
//
//*****************************************************************************
tUSBMode g_eCurrentUSBMode;





//*****************************************************************************
//
// USB Mode callback
//
// \param ulIndex is the zero-based index of the USB controller making the
//        callback.
// \param eMode indicates the new operating mode.
//
// This function is called by the USB library whenever an OTG mode change
// occurs and, if a connection has been made, informs us of whether we are to
// operate as a host or device.
//
// \return None.
//
//*****************************************************************************
void
ModeCallback(unsigned long ulIndex, tUSBMode eMode)
{
    //
    // Save the new mode.
    //

    g_eCurrentUSBMode = eMode;
}


//*****************************************************************************
//
// This is the callback from the MSC driver.
//
// \param ulInstance is the driver instance which is needed when communicating
// with the driver.
// \param ulEvent is one of the events defined by the driver.
// \param pvData is a pointer to data passed into the initial call to register
// the callback.
//
// This function handles callback events from the MSC driver.  The only events
// currently handled are the MSC_EVENT_OPEN and MSC_EVENT_CLOSE.  This allows
// the main routine to know when an MSC device has been detected and
// enumerated and when an MSC device has been removed from the system.
//
// \return Returns \e true on success or \e false on failure.
//
//*****************************************************************************
void
MSCCallback(unsigned long ulInstance, unsigned long ulEvent, void *pvData)
{
    //
    // Determine the event.
    //
    switch(ulEvent)
    {
        //
        // Called when the device driver has successfully enumerated an MSC
        // device.
        //
        case MSC_EVENT_OPEN:
        {
            //
            // Proceed to the enumeration state.
            //
            g_eState = STATE_DEVICE_ENUM;
            break;
        }

        //
        // Called when the device driver has been unloaded due to error or
        // the device is no longer present.
        //
        case MSC_EVENT_CLOSE:
        {
            //
            // Go back to the "no device" state and wait for a new connection.
            //
            g_eState = STATE_NO_DEVICE;

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// \param pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occured that is outside those releated to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occured.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
// \return None.
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ulEvent)
    {
        //
        // New keyboard detected.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // An unknown device was detected.
            //
            g_eState = STATE_UNKNOWN_DEVICE;

            break;
        }

        //
        // Keyboard has been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Unknown device has been removed.
            //
            g_eState = STATE_NO_DEVICE;

            break;
        }

        case USB_EVENT_POWER_FAULT:
        {
            //
            // No power means no device is present.
            //
            g_eState = STATE_POWER_FAULT;

            break;
        }

        default:
        {
            break;
        }
    }
}

void USBDeviceIntHandlerInternal(unsigned long ulIndex, unsigned long ulStatus)
{

}


void usb_Init()
{

	//
	// Enable Clocking to the USB controller.
	//
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

	//
	// Set the USB pins to be controlled by the USB controller.
	//
#if USB_CONTROLL_ENABLE
#if ARCH_TYPE == ARCH_T_LM3S9X
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);
	MAP_GPIOPinConfigure(GPIO_PH3_USB0EPEN);
	MAP_GPIOPinConfigure(GPIO_PH4_USB0PFLT);
	MAP_GPIOPinTypeUSBDigital(GPIO_PORTH_BASE, GPIO_PIN_3 | GPIO_PIN_4);
#elif ARCH_TYPE == ARCH_T_LM3S5X
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	MAP_GPIOPinConfigure(GPIO_PC5_USB0EPEN);
	MAP_GPIOPinConfigure(GPIO_PC6_USB0PFLT);
	MAP_GPIOPinTypeUSBDigital(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6);
#endif
#endif

	//
	// Enable the uDMA controller and set up the control table base.
	//
	MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	MAP_uDMAEnable();
	MAP_uDMAControlBaseSet(g_sDMAControlTable);

	//
	// Initialize the USB stack mode and pass in a mode callback.
	//
	USBStackModeSet(0, USB_MODE_HOST, ModeCallback);

	//
	// Register the host class drivers.
	//
	USBHCDRegisterDrivers(0, g_ppHostClassDrivers, ARR_SIZE(g_ppHostClassDrivers));

	//
	// Initialize the power configuration.	This sets the power enable signal
	// to be active high and does not enable the power fault.
	//
#if USB_CONTROLL_ENABLE
	USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);
#endif

	//
	// Initialize the USB controller for OTG operation with a 2ms polling
	// rate.
	//
	USBHCDInit(0, usb_HCDPool, sizeof(usb_HCDPool));
}

void usb_HostHandler()
{

	USBHCDMain();
}

void *usb_HostOpen()
{

	return (void *)USBHMSCDriveOpen(0, MSCCallback);
}

void usb_HostClose(void *p)
{

	USBHMSCDriveClose((int)p);
}

int usb_HostIsConnected(void *p)
{

	return USBHMSCDriveReady((int)p) ? 1 : 0;
}

int usb_HostMscRead(void *p, u32 nOffset, void *pBuf, size_t nLen)
{

	return USBHMSCBlockRead((int)p, nOffset, pBuf, nLen);
}

int usb_HostMscWrite(void *p, u32 nOffset, const void *pBuf, size_t nLen)
{

	return USBHMSCBlockWrite((int)p, nOffset, (u8 *)pBuf, nLen);
}

#endif

#endif

