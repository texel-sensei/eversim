#include "core/world/level_loader.h"
#include "core/world/level.h"
#include "core/utility/filesystem_wrapper.h"
#include "core/utility/helper.h"

#include <soil/SOIL.h>

#include <boost/format.hpp>

#include <stdexcept>
#include <fstream>
#include "core/world/tile_descriptor.h"


using namespace std;

namespace eversim { namespace core { namespace world {

	level_loader::level_loader()
	{
		register_tile_descriptor(&blank_tile);
	}

	shared_ptr<level> level_loader::load_file(string const& filename) const
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
		
		auto _ = utility::finally([&image_data]{
			SOIL_free_image_data(image_data);
		});


		auto layout = boost::const_multi_array_ref<uint32_t, 2>(
			reinterpret_cast<uint32_t*>(image_data),
			boost::extents[res.x][res.y]
		);

		auto lvl = make_shared<level>(res);

		for(int x = 0; x < res.x; ++x)
		{
			for(int y = 0; y < res.y; ++y)
			{
				auto& t = lvl->get_tile_by_index({ x,y });
				auto id = layout[x][y];
				if(idtable.find(id) == idtable.end())
				{
					throw runtime_error{
						(boost::format("Unknown id %d at tile (%d,%d)") % id % x % y).str()
					};
				}
				auto desc_name = idtable[id];
				auto desc = descriptors.at(desc_name);
				if(!desc)
				{
					throw runtime_error{ "Unknown tile descriptor! " + desc_name };
				}
				t.set_descriptor(desc);
			}
		}

		return lvl;
	}

	map<int, string> level_loader::load_id_table(istream& data)
	{
		auto ret = map<int, string>();
		auto line = string();
		while(getline(data, line))
		{
			stringstream in(line);
			in >> ws;
			if (in.peek() == '#')
				continue;
			int num;
			in >> num;

			string text;
			in >> text;
			ret[num] = text;
		}
		return ret;
	}

	void level_loader::register_tile_descriptor(tile_descriptor const* desc)
	{
		assert(desc);
		descriptors[desc->name] = desc;
	}
}}}
