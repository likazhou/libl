

//Private Defines


//Private Variables
static struct rt_device spif_device;


//Private Macros






//Private Functions
static rt_err_t spif_init(rt_device_t dev)
{

	return RT_EOK;
}

static rt_err_t spif_open(rt_device_t dev, rt_uint16_t oflag)
{

	return RT_EOK;
}

static rt_err_t spif_close(rt_device_t dev)
{

	return RT_EOK;
}

static rt_size_t spif_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	uint8_t *pData = (uint8_t *)buffer;
	uint_t i, nEnd;

	pos += SPIF_FS_BASE_BLOCK;
	nEnd = pos + size;
	for (i = pos; i < nEnd; i++, pData += SPIF_SEC_SIZE)
	{
		spif_SecRead(i, pData);
	}
	
	return (i - pos);
}

static rt_size_t spif_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	uint8_t *pData = (uint8_t *)buffer;
	uint_t i, nEnd;

	pos += SPIF_FS_BASE_BLOCK;
	nEnd = pos + size;
	for (i = pos; i < nEnd; i++, pData += SPIF_SEC_SIZE)
	{
		spif_SecWrite(i, pData);
	}

	return (i - pos);
}

static rt_err_t spif_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
	struct rt_device_blk_geometry *p = (struct rt_device_blk_geometry *)args;

	switch (cmd)
	{
	case RT_DEVICE_CTRL_BLK_GETGEOME:
		p->sector_count = spif_GetSize() - SPIF_FS_BASE_BLOCK;
		p->bytes_per_sector = SPIF_SEC_SIZE;
		p->block_size = SPIF_SEC_SIZE;
		break;
	default:
		break;
	}
	return RT_EOK;
}

void spif_dev_Init()
{

	spif_device.type = RT_Device_Class_Block;

	/* register sdcard device */
	spif_device.init = spif_init;
	spif_device.open = spif_open;
	spif_device.close = spif_close;
	spif_device.read = spif_read;
	spif_device.write = spif_write;
	spif_device.control = spif_control;

	/* no private */
	spif_device.user_data = RT_NULL;

	rt_device_register(&spif_device, "spif0", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STANDALONE);
}



