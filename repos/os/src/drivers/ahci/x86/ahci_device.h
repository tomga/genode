/*
 * \brief  AHCI device
 * \author Sebastian Sumpf <Sebastian.Sumpf@genode-labs.com>
 * \author Martin Stein    <Martin.Stein@genode-labs.com>
 * \date   2011-08-10
 */

/*
 * Copyright (C) 2011-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _AHCI_DEVICE_H_
#define _AHCI_DEVICE_H_

/* Genode includes */
#include <pci_session/connection.h>
#include <pci_device/client.h>

/* local includes */
#include <ahci_device_base.h>

/**
 * AHCI device
 */
class Ahci_device : public Ahci_device_base
{
	private:

		enum Pci_config {
			PCI_CFG_BMIBA_OFF   = 0x24, /* offset in PCI config space */
			CLASS_MASS_STORAGE  = 0x10000U,
			SUBCLASS_AHCI       = 0x0600U,
			CLASS_MASK          = 0xffff00U,
			AHCI_BASE_ID        = 0x5,  /* resource id of AHCI base addr <BAR 5> */
			AHCI_INTR_OFF       = 0x3c, /* offset of interrupt information in config space */
		};

		::Pci::Connection        &_pci;
		::Pci::Device_client     *_pci_device;

		/**
		 * Return next PCI device
		 */
		static Pci::Device_capability _scan_pci(Pci::Connection &pci, Pci::Device_capability prev_device_cap)
		{
			Pci::Device_capability device_cap;

			for (unsigned i = 0; i < 2; i++) {
				try {
					device_cap = pci.next_device(prev_device_cap,
					                             CLASS_MASS_STORAGE | SUBCLASS_AHCI,
					                             CLASS_MASK);
					break;
				} catch (Pci::Device::Quota_exceeded) {
					Genode::env()->parent()->upgrade(pci.cap(), "ram_quota=4096");
				}
			}

			if (prev_device_cap.valid())
				pci.release_device(prev_device_cap);

			return device_cap;
		}

		/**
		 * Initialize port
		 */
		void _init(::Pci::Device_client *pci_device)
		{
			uint32_t version = _ctrl->version();
			PINF("AHCI Version: %x.%04x", (version >> 16), version & 0xffff);

			/* HBA capabilities at offset 0 */
			uint32_t caps = _ctrl->hba_cap();
			PINF("CAPs:");
			PINF("\tPort count: %u", _ctrl->port_count());
			PINF("\tCommand slots: %u", _ctrl->cmd_slots());
			PINF("\tAHCI only: %s", (caps & 0x20000) ? "yes" : "no");
			PINF("\tNative command queuing: %s", (caps & 0x40000000) ? "yes" : "no");
			PINF("\t64 Bit: %s", (caps & 0x80000000) ? "yes" : "no");

			/* setup up AHCI data structures */
			_setup_memory();

			/* check and possibly enable AHCI mode */
			_ctrl->global_enable_ahci();

			/* enable global interrupts */
			_ctrl->global_interrupt_enable();

			/* disable power mgmt. */
			_port->disable_pm();

			/* startup device */
			_port->get_ready();

			/* reset port */
			_port->reset();

			/* clear error register */
			_port->err(_port->err());

			/* port interrupt enable */
			_port->interrupt_enable();

			/* ack all possibly pending interrupts */
			_port->interrupt_ack();
			_ctrl->hba_interrupt_ack();

			/* retrieve block count */
			_identify_device();
		}

	public:

		Ahci_device(addr_t base, Io_mem_session_capability io_cap,
		            Pci::Connection &pci)
		: Ahci_device_base(base, io_cap), _pci(pci), _pci_device(0) { }

		/**
		 * Probe PCI-bus for AHCI and ATA-devices
		 */
		static Ahci_device *probe(Pci::Connection &pci)
		{
			Pci::Device_capability device_cap;
			Ahci_device *device;

			while ((device_cap = _scan_pci(pci, device_cap)).valid()) {

				::Pci::Device_client * pci_device =
					new(env()->heap()) ::Pci::Device_client(device_cap);

				PINF("Found AHCI HBA (Vendor ID: %04x Device ID: %04x Class:"
				     " %08x)\n", pci_device->vendor_id(),
				     pci_device->device_id(), pci_device->class_code());

				/* read and map base address of AHCI controller */
				Pci::Device::Resource resource = pci_device->resource(AHCI_BASE_ID);

				Io_mem_connection io(resource.base(), resource.size());
				addr_t addr = (addr_t)env()->rm_session()->attach(io.dataspace());

				/* add possible resource offset */
				addr += resource.base() & 0xfff;

				if (verbose)
					PDBG("resource base: %x virt: %lx", resource.base(), addr);

				/* create and test device */
				device = new(env()->heap()) Ahci_device(addr, io.cap(), pci);
				if (device->_scan_ports()) {

					io.on_destruction(Io_mem_connection::KEEP_OPEN);

					/* read IRQ information */
					unsigned long intr = pci_device->config_read(AHCI_INTR_OFF,
					                                             ::Pci::Device::ACCESS_32BIT);

					if (verbose) {
						PDBG("Interrupt pin: %lu line: %lu", (intr >> 8) & 0xff, intr & 0xff);

						unsigned char bus, dev, func;
						pci_device->bus_address(&bus, &dev, &func);
						PDBG("Bus address: %x:%02x.%u (0x%x)", bus, dev, func, (bus << 8) | ((dev & 0x1f) << 3) | (func & 0x7));
					}

					device->_irq = new(env()->heap()) Irq_session_client(pci_device->irq(0));
					Genode::Signal_context_capability cap = device->_irq_rec.manage(&device->_irq_ctx);
					device->_irq->sigh(cap);
					device->_irq->ack_irq();

					device->_pci_device = pci_device;
					/* trigger assignment of pci device to the ahci driver */
					try {
						pci.config_extended(device_cap);
					} catch (Pci::Device::Quota_exceeded) {
						Genode::env()->parent()->upgrade(pci.cap(), "ram_quota=4096");
						pci.config_extended(device_cap);
					}

					/* get device ready */
					device->_init(pci_device);

					return device;
				}

				destroy(env()->heap(), pci_device);
				env()->rm_session()->detach(addr);
				destroy(env()->heap(), device);
			}
			return 0;
		}

		Ram_dataspace_capability alloc_dma_buffer(size_t size)
		{
			/* transfer quota to pci driver, otherwise we get a exception */
			char quota[32];
			Genode::snprintf(quota, sizeof(quota), "ram_quota=%zd", size);
			Genode::env()->parent()->upgrade(_pci.cap(), quota);

			return _pci.alloc_dma_buffer(size);
		}

		void free_dma_buffer(Ram_dataspace_capability cap) {
			return _pci.free_dma_buffer(cap); }
};

#endif /* _AHCI_DEVICE_H_ */

