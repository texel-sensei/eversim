#include "core/system/component.h"
#include "core/system/errors.h"

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
}}}
