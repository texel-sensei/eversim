#include "core/rendering/renderable_entity.h"

#include "core/utility/plattform.h"

#include <easylogging++.h>

namespace eversim {	namespace core { namespace rendering {

	std::unique_ptr<Multibuffer> default_quadmesh_ptr = nullptr;
	Texture default_texture(glm::ivec2(4,4));
	ShaderProgram default_shader("default uv shader");

	RenderableEntity::RenderableEntity() : 
		program(&default_shader), tex(&default_texture), data(&(*default_quadmesh_ptr))
	{}

	void RenderableEntity::bind() const
	{

		if(data != nullptr) data->bind();
		if(tex != nullptr) {

			//TODO bind "in_Texoffset"
			const auto& uniforms = program->getActiveUniforms();
			
			auto has_tex_offset = false;
			auto has_tex_size = false;
			auto has_sprite_size = false;
			const std::string texoffsetname("texoffset");
			const std::string texsizename("texsize");
			const std::string spritesizename("spritesize");
			for(size_t i = 0; i < uniforms.size(); ++i)
			{
				const auto& uniform = uniforms.at(i);
				if(std::get<0>(uniform) == GL_FLOAT_VEC2 &&
					std::get<1>(uniform) == texoffsetname)
				{
					//LOG(INFO) << "upload" << texoffset[0] << "/" << texoffset[1];
					auto location = program->getUniformLocation(texoffsetname);
					glUniform2f(location, texoffset[0], texoffset[1]);
					has_tex_offset = true;
				}

				else if (std::get<0>(uniform) == GL_FLOAT_VEC2 &&
					std::get<1>(uniform) == texsizename)
				{
					//LOG(INFO) << "upload" << texoffset[0] << "/" << texoffset[1];
					auto location = program->getUniformLocation(texsizename);
					glUniform2f(location, texsize[0], texsize[1]);
					has_tex_size = true;
				}

				else if (std::get<0>(uniform) == GL_FLOAT_VEC2 &&
					std::get<1>(uniform) == spritesizename)
				{
					//LOG(INFO) << "upload" << texoffset[0] << "/" << texoffset[1];
					auto location = program->getUniformLocation(spritesizename);
					glUniform2f(location, spritesize[0], spritesize[1]);
					has_sprite_size = true;
				}
			}

			tex->bind();
		}
	}

	void RenderableEntity::draw() const
	{
		data->draw();
	}

	const instanced_entity_information& RenderableEntity::get_instanced_entity_information() const
	{
		instanced_entity_information ifo;
		get_instanced_entity_information(ifo);
		return ifo;
	}

	void RenderableEntity::get_instanced_entity_information(instanced_entity_information& ifo) const
	{
		ifo.set_M(M);
		ifo.set_texoffset(texoffset);
		ifo.set_texsize(texsize);
		ifo.set_spritesize(spritesize);
	}

	void  RenderableEntity::set_Position(glm::fvec2 pos)
	{
		M[2] = glm::fvec3(pos,1.f);
		touch();
	}

	glm::fvec2  RenderableEntity::get_Position() const
	{
		return { M[2][0], M[2][1] };
	}

	void RenderableEntity::set_Scale(glm::fvec2 scale)
	{
		M[0] = { scale[0], 0, 0 };
		M[1] = { 0, scale[1], 0};
		touch();
	}

	glm::fvec2 RenderableEntity::get_Scale() const
	{
		return { M[0][0], M[1][1] };
	}

	void RenderableEntity::default_State()
	{
		default_Multibuffer();
		default_ShaderProgram();
		default_Texture();
	}
}}}