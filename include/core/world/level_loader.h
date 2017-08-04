#pragma once

#include "core/utility/resource_manager.h"
#include <istream>
#include <map>
#include <string>
#include <unordered_map>

namespace eversim { namespace core { namespace world {
	class level;
	struct tile_descriptor;

	class level_loader final: public utility::resource_manager<level_loader, std::string, level> {
	public:
		level_loader();


		static std::map<uint32_t, std::string> load_id_table(std::istream& data);
		void register_tile_descriptor(tile_descriptor const*);

	protected:
		ptr_type load_file(std::string const& filename) const override;
	private:
		std::unordered_map<std::string, tile_descriptor const*> descriptors;
	};

}}}
