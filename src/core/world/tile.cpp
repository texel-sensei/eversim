#include "core/world/tile.h"
#include "core/world/tile_descriptor.h"

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
}}}
