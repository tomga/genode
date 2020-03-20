/*
 * \brief  Driver for Raspberry Pi specific platform devices
 * \author Norman Feske
 * \date   2013-09-16
 */

/*
 * Copyright (C) 2013-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

/* Genode includes */
#include <base/attached_rom_dataspace.h>
#include <base/log.h>
#include <base/component.h>
#include <base/heap.h>
#include <root/component.h>
#include <util/xml_node.h>

/* platform includes */
#include <platform_session/platform_session.h>
#include <platform/property_message.h>

/* local includes */
#include <mbox.h>
#include <property_command.h>
#include <framebuffer_message.h>


namespace Platform {
	class Session_component;
	class Root;
}


class Platform::Session_component : public Genode::Rpc_object<Platform::Session>
{
	private:

		Mbox &_mbox;
		int _rpi_version;

	public:

		Session_component(Mbox &mbox, int rpi_version)
		: _mbox(mbox), _rpi_version(rpi_version) { }


		/**********************************
		 **  Platform session interface  **
		 **********************************/

		void setup_framebuffer(Framebuffer_info &info) override
		{
			{
				auto &msg = _mbox.message<Property_message>();
				auto const &res = msg.append<Property_command::Get_physical_w_h>();
				_mbox.call<Property_message>();
				log("Get_physical_w_h: ", res.width, "x", res.height);
			}

			auto &msg = _mbox.message<Property_message>();
			msg.append<Property_command::Set_physical_w_h>(info.phys_width, info.phys_height);
			msg.append<Property_command::Set_virtual_w_h>(info.virt_width, info.virt_height);
			msg.append<Property_command::Set_depth>(info.depth);
			auto const &res = msg.append<Property_command::Allocate_buffer>();
			_mbox.call<Property_message>();

			switch (_rpi_version) {
			case 2:
			case 3:
				info.addr = res.address & ~0xC0000000; // bus to phys memory translation
				                                       // TODO: make a function for this
				break;
			case 1:
			default:
				info.addr = res.address & ~0x40000000; // bus to phys memory translation
				                                       // TODO: make a function for this
			}

			info.size = res.size;

			log("setup_framebuffer ", (void*) (long) info.addr, ", ", info.size, ", ", info.phys_width, ", ", info.phys_height);
		}

		bool power_state(Power id) override
		{
			auto &msg = _mbox.message<Property_message>();
			auto const &res = msg.append<Property_command::Get_power_state>(id);
			_mbox.call<Property_message>();
			return res.state;
		}

		void power_state(Power id, bool enable) override
		{
			auto &msg = _mbox.message<Property_message>();
			msg.append_no_response<Property_command::Set_power_state>(id, enable, true);
			_mbox.call<Property_message>();
		}

		uint32_t clock_rate(Clock id) override
		{
			auto &msg = _mbox.message<Property_message>();
			auto const &res = msg.append<Property_command::Get_clock_rate>(id);
			_mbox.call<Property_message>();
			return res.hz;
		}
};


class Platform::Root : public Genode::Root_component<Platform::Session_component>
{
	private:

		Mbox _mbox;
		int _rpi_version;

	protected:

		Session_component *_create_session(const char *) override {
			return new (md_alloc()) Session_component(_mbox, _rpi_version); }

	public:

		Root(Env& env, Allocator & md_alloc, int rpi_version)
		: Root_component<Session_component>(env.ep(), md_alloc),
			_mbox(env, rpi_version), _rpi_version(rpi_version)
		{ }
};


struct Main
{
	Genode::Env &  env;
	Genode::Heap   heap { env.ram(), env.rm() };

	Genode::Attached_rom_dataspace _config { env, "config" };

	Platform::Root root { env, heap, config_rpi_version(_config.xml()) };

	Main(Genode::Env & env) : env(env) {
		Genode::log("rpi_version ", config_rpi_version(_config.xml()));

		env.parent().announce(env.ep().manage(root)); }

private:
	static int config_rpi_version(Genode::Xml_node node)
	{
		return node.attribute_value("rpi_version", 0UL);
	}

};



void Component::construct(Genode::Env &env)
{
	Genode::log("--- Raspberry Pi platform driver ---");

	static Main main(env);
}
