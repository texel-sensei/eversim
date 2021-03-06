#include "core/system/component.h"
#include "core/system/errors.h"
#include "core/system/gameobject.h"
#include "core/system/game.h"

namespace eversim { namespace core { namespace system {
	std::unique_ptr<component> component::clone() const
	{
		auto cloned_comp = this->do_clone();
		if(typeid(*cloned_comp) != typeid(*this))
		{
			EVERSIM_THROW(game_error::ComponentMissingClone, typeid(*this).name());
		}
		return cloned_comp;
	}

	utility::clock::duration component::get_frametime() const
	{
		return owner->get_frametime();
	}

	utility::clock::time_point component::get_current_time() const
	{
		return owner->get_game().get_time();
	}
}}}
