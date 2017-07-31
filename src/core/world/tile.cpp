#include "core/world/tile.h"
#include "core/world/tile_descriptor.h"
#include "core/rendering/render_manager.h"
#include "core/world/level.h"

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

	tile const* tile::get_neighbour(glm::ivec2 delta) const
	{
		const auto idx = index() + delta;
		if (!lvl->contains_index(idx)) return nullptr;
		return &lvl->get_tile_by_index(idx);
	}

	bool tile::has_collision() const noexcept
	{
		return descriptor->collision != collision_type::none;
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

	std::bitset<4> tile::calculate_collision_shape() const
	{
		using namespace glm;
		
		std::bitset<4> sides = 0;

		const ivec2 offsets[] = {
			ivec2(-1,0), ivec2(0,-1), ivec2(1,0), ivec2(0,1)
		};
		for(int i = 0; i < sides.size(); ++i)
		{
			const auto n = get_neighbour(offsets[3-i]);
			if(n && n->has_collision())
			{
				sides[i] = true;
			}
		}
		return sides;
	}
}}}
