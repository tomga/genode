/*
 * \brief  VM session component for 'base-hw'
 * \author Stefan Kalkowski
 * \date   2015-02-17
 */

/*
 * Copyright (C) 2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <util/construct_at.h>

/* core includes */
#include <kernel/core_interface.h>
#include <vm_session_component.h>
#include <platform.h>
#include <core_env.h>

using namespace Genode;


void Vm_session_component::exception_handler(Signal_context_capability handler)
{
	if (_initialized) {
		PWRN("Cannot initialize kernel vm object twice!");
		return;
	}

	Core_mem_allocator * cma =
		static_cast<Core_mem_allocator*>(platform()->core_mem_alloc());
	if (Kernel::new_vm(&_vm, (void*)_ds.core_local_addr(), handler.dst(),
	                   cma->phys_addr(_table))) {
		PWRN("Cannot instantiate vm kernel object, invalid signal context?");
		return;
	}

	_initialized = true;
}


void Vm_session_component::_attach(addr_t phys_addr, addr_t vm_addr, size_t size)
{
	Page_flags pflags = Page_flags::apply_mapping(true, CACHED, false);

	try {
		for (;;)
			try {
				_table->insert_translation(vm_addr, phys_addr, size, pflags, _pslab);
				return;
			} catch(Page_slab::Out_of_slabs) {
				_pslab->alloc_slab_block();
			}
	} catch(Allocator::Out_of_memory) {
		PERR("Translation table needs to much RAM");
	} catch(...) {
		PERR("Invalid mapping %p -> %p (%zx)", (void*)phys_addr,
		     (void*)vm_addr, size);
	}
}


void Vm_session_component::attach(Dataspace_capability ds_cap, addr_t vm_addr)
{
	/* check dataspace validity */
	Object_pool<Dataspace_component>::Guard dsc(_ds_ep->lookup_and_lock(ds_cap));
	if (!dsc) throw Invalid_dataspace();

	_attach(dsc->phys_addr(), vm_addr, dsc->size());
}


void Vm_session_component::attach_pic(addr_t vm_addr)
{
	_attach(Board::IRQ_CONTROLLER_VT_CPU_BASE, vm_addr,
	        Board::IRQ_CONTROLLER_VT_CPU_SIZE);
}


void Vm_session_component::detach(addr_t vm_addr, size_t size) {
	_table->remove_translation(vm_addr, size, _pslab); }


Vm_session_component::Vm_session_component(Rpc_entrypoint  *ds_ep,
                                           size_t           ram_quota)
: _ds_ep(ds_ep), _ds(_ds_size(), _alloc_ds(ram_quota), UNCACHED, true, 0),
  _ds_cap(static_cap_cast<Dataspace>(_ds_ep->manage(&_ds)))
{
	_ds.assign_core_local_addr(core_env()->rm_session()->attach(_ds_cap));

	Core_mem_allocator * cma =
		static_cast<Core_mem_allocator*>(platform()->core_mem_alloc());
	void *tt;

	/* get some aligned space for the translation table */
	if (!cma->alloc_aligned(sizeof(Translation_table), (void**)&tt,
	                        Translation_table::ALIGNM_LOG2).is_ok()) {
		PERR("failed to allocate kernel object");
		throw Root::Quota_exceeded();
	}

	_table = construct_at<Translation_table>(tt);
	_pslab = new (cma) Page_slab(cma);
}
