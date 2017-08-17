#pragma once

#include "core/utility/resource_manager.h"
#include <istream>
#include <map>
#include <string>
#include <unordered_map>

namespace eversim { namespace core { namespace world {
	class level;
	struct tile_descriptor;
	class tile_loader;

	class level_loader final: public utility::resource_manager<level_loader, std::string, level> {
	public:
		using tile_loader_ptr = std::shared_ptr<tile_loader>;
		explicit level_loader(tile_loader_ptr = nullptr);

		static std::map<uint32_t, std::string> load_id_table(std::istream& data);		

		void register_tile_descriptor(tile_descriptor const*);

		std::vector<std::string> get_level_tiles(std::string const& name) const;
	protected:
		ptr_type load_file(std::string const& filename) override;
	private:
		tile_loader_ptr tile_loader_;
		std::unordered_map<std::string, tile_descriptor const*> descriptors;
	};

}}}
