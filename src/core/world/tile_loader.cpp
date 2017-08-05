#include "core/world/tile_loader.h"
#include "core/world/errors.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

using namespace std;

namespace eversim { namespace core { namespace world {

	std::string tile_loader::key_to_name(key_type const& key) const
	{
		return key + ".json";
	}

	tile_loader::ptr_type tile_loader::load_file(string const& path)
	{
		cycle_check_set check;
		return load_impl(path, check);
	}

	tile_loader::ptr_type tile_loader::load_impl(string const& name, cycle_check_set& cycle_check)
	{
		// check for cycles
		const auto already_touched = cycle_check.find(name);
		if (already_touched != cycle_check.end())
		{
			EVERSIM_THROW(tile_error::InheritanceCycle);
		}
		cycle_check.insert(already_touched, name);

		auto the_tile = make_shared<tile_descriptor>();

		using namespace boost::property_tree;
		ptree tree;
		auto data = ifstream(name);
		if(!data.is_open())
		{
			EVERSIM_THROW(tile_error::UnknownTile, name);
		}
		read_json(data, tree);

		if(auto parent = tree.get_optional<string>("parent_tile"))
		{
			*the_tile = *load(*parent);
		}

		if(auto col = tree.get_optional<bool>("has_collision"))
			the_tile->collision = (*col) ? collision_type::solid : collision_type::none;

		if(auto texture = tree.get_optional<string>("texture"))
			the_tile->texture_name = *texture;

		return the_tile;
	}
}}}
