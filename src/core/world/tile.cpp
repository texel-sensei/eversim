#include "core/world/tile.h"
#include "core/world/tile_descriptor.h"
#include "core/rendering/render_manager.h"

namespace eversim { namespace core { namespace world {
	tile_descriptor blank_tile = {
		"blank"
	};

	bool tile::point_inside(glm::vec2 p) const
	{
		auto center = position();
		auto min = center - glm::vec2(size() / 2);
		auto max = center + glm::vec2(size() / 2);

		return p.x >= min.x && p.x < max.x && p.y >= min.y && p.y < max.y;
	}
	
	void tile::initialize_graphics(rendering::render_manager& mng)
	{
		auto const& tex_name = descriptor->texture_name;
		if (tex_name.empty())
			return;
		display = mng.register_entity();
		display->set_Texture(mng.register_texture(tex_name).get());

		display->set_Position(position() - glm::vec2(size()/2));
		display->set_Scale(glm::vec2(size()));
	}
}}}
