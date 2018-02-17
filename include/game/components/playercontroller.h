#pragma once

#include "core\system\component.h"
#include "core\input\inputhandler.h"

#include "core\utility\event_delegate.h"

namespace eversim::game::components {
	class PlayerController : public core::system::component {
	public:
		// Geerbt über component
		virtual std::unique_ptr<core::system::component> do_clone() const override;
		
		PlayerController(core::system::gameobject* owner, core::input::InputHandler& handler);
	private:
		core::utility::Delegate delg;
		core::input::InputHandler *handler_ptr;
	};


	
}