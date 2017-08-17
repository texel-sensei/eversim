#include "core/world/level_loader.h"
#include "core/world/level.h"
#include "core/world/tile_descriptor.h"
#include "core/world/tile_loader.h"
#include "core/world/errors.h"

#include "core/utility/filesystem_wrapper.h"
#include "core/utility/helper.h"

#include <soil/SOIL.h>

#include <boost/format.hpp>
#include <boost/range/adaptors.hpp>

#include <stdexcept>
#include <fstream>

using namespace std;

namespace eversim { namespace core { namespace world { namespace {
	uint32_t load_big_endian(uint32_t in)
	{
		auto* dat = reinterpret_cast<unsigned char*>(&in);
		return (dat[0] << 24) + (dat[1] << 16) + (dat[2] << 8) + dat[3];
	}
}

level_loader::level_loader(tile_loader_ptr tl) : tile_loader_(tl)
{
	register_tile_descriptor(&blank_tile);
}

shared_ptr<level> level_loader::load_file(string const& filename)
{
	using namespace utility::fs;
	const auto dir = path(filename);
	const auto layout_path = dir / "layout.png";
	const auto table_path = dir / "ids.txt";
	// TODO check extra data

	if (!exists(layout_path))
	{
		throw runtime_error{"Layout file missing in " + filename};
	}
	if (!exists(table_path))
	{
		throw runtime_error{"ID table file missing in " + filename};
	}

	auto idtablestream = ifstream(table_path);
	auto idtable = load_id_table(idtablestream);

	glm::ivec2 res;
	auto image_data = SOIL_load_image(layout_path.string().c_str(), &res.x, &res.y, nullptr, SOIL_LOAD_RGBA);

	auto _ = utility::finally([&image_data]
	{
		SOIL_free_image_data(image_data);
	});


	auto layout = boost::const_multi_array_ref<uint32_t, 2>(
		reinterpret_cast<uint32_t*>(image_data),
		boost::extents[res.x][res.y]
	);

	auto lvl = make_shared<level>(res);

	for (int x = 0; x < res.x; ++x)
	{
		for (int y = 0; y < res.y; ++y)
		{
			auto& t = lvl->get_tile_by_index({x,y});
			auto id = load_big_endian(layout[res.y - y - 1][x]);
			if (idtable.find(id) == idtable.end())
			{
				EVERSIM_THROW(level_error::UnknownID,
					(boost::format("Unknown id %d at tile (%d,%d)") % id % x % y).str()
				);
			}
			const auto desc_name = idtable[id];
			const auto desc_it = descriptors.find(desc_name);

			tile_descriptor const* desc = nullptr;
			if(desc_it != descriptors.end())
			{
				desc = desc_it->second;
			}else if(tile_loader_)
			{
				const auto ptr = tile_loader_->load(desc_name);
				if(ptr.use_count() == 1)
				{
					// loader currently can't set the name of the tile descriptor by itself
					ptr->name = desc_name;
				}
				desc = ptr.get();
				lvl->add_tile_descriptor(ptr);
			}

			if (!desc)
			{
				EVERSIM_THROW(level_error::UnknownTile, desc_name);
			}
			t.set_descriptor(desc);
		}
	}
	lvl->calculate_collision_shapes();
	return lvl;
}

map<uint32_t, string> level_loader::load_id_table(istream& data)
{
	auto ret = map<uint32_t, string>();
	auto line = string();
	while (getline(data, line))
	{
		stringstream in(line);
		in >> ws;
		if (in.peek() == '#')
			continue;
		string num_txt;
		in >> num_txt;

		// use base '0' to let stoi automatically detect hex numbers
		auto num = stoll(num_txt, nullptr, 0);

		string text;
		in >> text;
		ret[num] = text;
	}
	return ret;
}

void level_loader::register_tile_descriptor(tile_descriptor const* desc)
{
	EVERSIM_ASSERT(desc);
	descriptors[desc->name] = desc;
}

vector<string> level_loader::get_level_tiles(string const& name) const
{
	const auto dir = search(name);
	const auto table_path = dir / "ids.txt";
	auto idtablestream = ifstream(table_path);
	const auto idtable = load_id_table(idtablestream);

	vector<std::string> names;
	names.reserve(idtable.size());
	for(auto& tile : idtable | boost::adaptors::map_values)
	{
		if(tile == "blank")
		{
			continue;
		}
		names.push_back(tile);
	}
	return names;
}

}}}
