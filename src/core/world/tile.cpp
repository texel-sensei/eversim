#include "core/world/tile.h"
#include "core/world/tile_descriptor.h"
#include "core/rendering/render_manager.h"

// VERY hacky!
extern eversim::core::rendering::ShaderProgram* default_shader;

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
		auto M = display->get_M();
		M[2] = glm::vec3(position() - glm::vec2(size()), 1);
		display->set_M(M);

		LOG(WARNING) << "Hacky solution for tile display!";
		static std::map<std::string, rendering::Texture> textures;

		const auto it = textures.find(tex_name);
		if(it == textures.end())
		{
			textures[tex_name] = rendering::Texture(tex_name);
		}

		display->set_Texture(textures[tex_name]);
		display->set_ShaderProgram(default_shader);

	}
}}}
