#pragma once

#include "core/utility/resource_manager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace eversim {
	namespace core {
		namespace input {

			class json_loader final : public utility::resource_manager<json_loader, std::string, std::string>
			{
			protected:
				ptr_type load_file(std::string const& s) override
				{
					return std::make_shared<std::string>(s);
				}
			};
		}
	}
}