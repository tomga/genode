/*
 * \brief  USB initialization for Raspberry Pi
 * \author Norman Feske
 * \date   2013-09-11
 */

/*
 * Copyright (C) 2013-2017 Genode Labs GmbH
 *
 * This file is distributed under the terms of the GNU General Public License
 * version 2.
 */

/* Genode includes */
#include <io_mem_session/connection.h>
#include <util/mmio.h>
#include <platform_session/connection.h>


/* emulation */
#include <platform.h>
#include <lx_emul.h>

// /* dwc2 */
// #define new new_
// #include <dwc2_dbg.h>
// #undef new

using namespace Genode;


/************************************************
 ** Resource info passed to the dwc2 driver **
 ************************************************/

enum {
	DWC_BASE = 0x3f980000,
	DWC_SIZE = 0x20000,
};

unsigned dwc2_irq() { return 17; }

static resource _dwc2_resource[] =
{
	{ DWC_BASE, DWC_BASE + DWC_SIZE - 1, "dwc2", IORESOURCE_MEM },
	{ dwc2_irq(), dwc2_irq(), "dwc2-irq" /* name unused */, IORESOURCE_IRQ }
};


/*******************
 ** Init function **
 *******************/

extern "C" void module_dwc2_platform_driver_init();

void platform_hcd_init(Genode::Env &, Services *services)
{
	/* enable USB power */
	Platform::Connection platform(services->env);
	platform.power_state(Platform::Session::POWER_USB_HCD, true);

	/* disable fiq optimization */
	// fiq_enable = false;
	// fiq_fsm_enable = false;

	module_dwc2_platform_driver_init();

	/* setup host-controller platform device */
	platform_device *pdev = (platform_device *)kzalloc(sizeof(platform_device), 0);
	pdev->name            = (char *)"dwc2";
	pdev->id              = 0;
	pdev->num_resources   = sizeof(_dwc2_resource)/sizeof(resource);
	pdev->resource        = _dwc2_resource;

	pdev->dev.of_node             = (device_node*)kzalloc(sizeof(device_node), 0);
	pdev->dev.of_node->properties = (property*)kzalloc(sizeof(property), 0);
	pdev->dev.of_node->properties->name  = "compatible";
	pdev->dev.of_node->properties->value = (void*)"brcm,bcm2835-usb";

	/* needed for DMA buffer allocation. See 'hcd_buffer_alloc' in 'buffer.c' */
	static u64 dma_mask         = ~(u64)0;
	pdev->dev.dma_mask          = &dma_mask;
	pdev->dev.coherent_dma_mask = ~0;

	platform_device_register(pdev);
}
