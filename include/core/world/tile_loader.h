#pragma once
#include "core/utility/resource_manager.h"
#include "core/world/tile_descriptor.h"
#include <functional>
#include <set>

namespace eversim {namespace core { namespace world {

	class tile_loader final : public utility::resource_manager<tile_loader, std::string, tile_descriptor> {
		using super_t = utility::resource_manager<tile_loader, std::string, tile_descriptor>;
	public:
		using loader_function = std::function<std::istream&&(std::string)>;

		std::string key_to_name(key_type const& key) const;
	protected:
		ptr_type load_file(std::string const& path) override;
	private:
		using cycle_check_set = std::set<std::string>;
		ptr_type load_impl(std::string const& name, cycle_check_set& cycle_check);

	};


} } }
