#pragma once

#include "core/utility/resource_manager.h"
#include <istream>
#include <map>
#include <string>
#include <unordered_map>

namespace eversim { namespace core { namespace world {
	class level;
	struct tile_descriptor;

	class level_loader : public utility::resource_manager<level_loader, std::string, level> {
	public:
		level_loader();

		std::shared_ptr<value_type> load_file(std::string const& filename) const;

		static std::map<int, std::string> load_id_table(std::istream& data);
		void register_tile_descriptor(tile_descriptor const*);
	private:
		std::unordered_map<std::string, tile_descriptor const*> descriptors;
	};

}}}
