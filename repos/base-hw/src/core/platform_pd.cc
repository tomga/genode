/*
 * \brief   Protection-domain facility
 * \author  Martin Stein
 * \author  Stefan Kalkowski
 * \date    2012-02-12
 */

/*
 * Copyright (C) 2012-2015 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* core includes */
#include <platform_pd.h>

extern int _prog_img_beg;
extern int _prog_img_end;

using namespace Genode;


/**************************************
 ** Hw::Address_space implementation **
 **************************************/

Core_mem_allocator * Hw::Address_space::_cma() {
	return static_cast<Core_mem_allocator*>(platform()->core_mem_alloc()); }


void * Hw::Address_space::_tt_alloc()
{
	void * ret;
	if (!_cma()->alloc_aligned(sizeof(Translation_table), (void**)&ret,
	                           Translation_table::ALIGNM_LOG2).is_ok())
		throw Root::Quota_exceeded();
	return ret;
}


bool Hw::Address_space::insert_translation(addr_t virt, addr_t phys,
                                           size_t size, Page_flags flags)
{
	try {
		for (;;) {
			try {
				Lock::Guard guard(_lock);
				_tt->insert_translation(virt, phys, size, flags, _pslab);
				return true;
			} catch(Page_slab::Out_of_slabs) {
				_pslab->alloc_slab_block();
			}
		}
	} catch(Allocator::Out_of_memory) {
		PWRN("Translation table needs to much RAM");
	} catch(...) {
		PERR("Invalid mapping %p -> %p (%zx)", (void*)phys, (void*)virt, size);
	}
	return false;
}


void Hw::Address_space::flush(addr_t virt, size_t size)
{
	Lock::Guard guard(_lock);

	try {
		if (_tt) _tt->remove_translation(virt, size, _pslab);

		/* update translation caches */
		Kernel::update_pd(_kernel_pd);
	} catch(...) {
		PERR("tried to remove invalid region!");
	}
}


Hw::Address_space::Address_space(Kernel::Pd* pd, Translation_table * tt,
                                 Page_slab * slab)
: _tt(tt), _tt_phys(tt), _pslab(slab), _kernel_pd(pd) { }


Hw::Address_space::Address_space(Kernel::Pd * pd)
: _tt(construct_at<Translation_table>(_tt_alloc())),
  _tt_phys(reinterpret_cast<Translation_table*>(_cma()->phys_addr(_tt))),
  _pslab(new (_cma()) Page_slab(_cma())),
  _kernel_pd(pd)
{
	Lock::Guard guard(_lock);
	Kernel::mtc()->map(_tt, _pslab);
}


/********************************
 ** Platform_pd implementation **
 ********************************/

int Platform_pd::bind_thread(Platform_thread * t)
{
	/* is this the first and therefore main thread in this PD? */
	bool main_thread = !_thread_associated;
	_thread_associated = true;
	return t->join_pd(this, main_thread, Address_space::weak_ptr());
}


void Platform_pd::unbind_thread(Platform_thread *t) {
	t->join_pd(nullptr, false, Address_space::weak_ptr()); }


int Platform_pd::assign_parent(Native_capability parent)
{
	if (!parent.valid()) {
		PERR("parent invalid");
		return -1;
	}
	_parent = parent;
	return 0;
}


Platform_pd::Platform_pd(Translation_table * tt, Page_slab * slab)
: Hw::Address_space(reinterpret_cast<Kernel::Pd*>(&_kernel_object), tt, slab),
  _label("core") { new (&_kernel_object) Kernel::Pd(tt, this); }


Platform_pd::Platform_pd(Allocator * md_alloc, char const *label)
: Hw::Address_space(reinterpret_cast<Kernel::Pd*>(&_kernel_object)),
  _label(label)
{
	/* create kernel object */
	if (Kernel::new_pd(reinterpret_cast<Kernel::Pd*>(_kernel_object), this)) {
		PERR("failed to create kernel object");
		throw Root::Unavailable();
	}
}


Platform_pd::~Platform_pd()
{
	Kernel::delete_pd(kernel_pd());
	flush(platform()->vm_start(), platform()->vm_size());

	/* TODO: destroy page slab and translation table!!! */
}


/*************************************
 ** Core_platform_pd implementation **
 *************************************/

Translation_table * const Core_platform_pd::_table()
{
	return unmanaged_singleton<Translation_table,
	                           1 << Translation_table::ALIGNM_LOG2>();
}


Page_slab * const Core_platform_pd::_slab()
{
	using Slab      = Kernel::Early_translations_slab;
	using Allocator = Kernel::Early_translations_allocator;

	return unmanaged_singleton<Slab,
	                           Slab::ALIGN>(unmanaged_singleton<Allocator>());
}


void Core_platform_pd::_map(addr_t start, addr_t end, bool io_mem)
{
	const Page_flags flags =
		Page_flags::apply_mapping(true, io_mem ? UNCACHED : CACHED, io_mem);

	start        = trunc_page(start);
	size_t size  = round_page(end) - start;

	try {
		_table()->insert_translation(start, start, size, flags, _slab());
	} catch(Page_slab::Out_of_slabs) {
		PERR("Not enough page slabs");
	} catch(Allocator::Out_of_memory) {
		PERR("Translation table needs to much RAM");
	} catch(...) {
		PERR("Invalid mapping %p -> %p (%zx)", (void*)start,
			 (void*)start, size);
	}
}


Core_platform_pd::Core_platform_pd()
: Platform_pd(_table(), _slab())
{
	/* map exception vector for core */
	Kernel::mtc()->map(_table(), _slab());

	/* map core's program image */
	_map((addr_t)&_prog_img_beg, (addr_t)&_prog_img_end, false);

	/* map core's mmio regions */
	Native_region * r = Platform::_core_only_mmio_regions(0);
	for (unsigned i = 0; r;
		 r = Platform::_core_only_mmio_regions(++i))
		_map(r->base, r->base + r->size, true);
}
