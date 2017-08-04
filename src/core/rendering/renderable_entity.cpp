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

	glm::fmat3 RenderableEntity::get_M() const { return M; };

	ShaderProgram* RenderableEntity::get_ShaderProgram() const { return program; };

	TextureBase* RenderableEntity::get_Texture() const { return tex; };

	Multibuffer* RenderableEntity::get_Multibuffer() const { return data; };

	void RenderableEntity::set_M(const glm::fmat3& m)
	{
		M = m; 
		touch();
	};

	void RenderableEntity::set_ShaderProgram(ShaderProgram* p)
	{
		program = p; 
		touch();
	};

	void RenderableEntity::set_ShaderProgram(ShaderProgram& p)
	{
		set_ShaderProgram(&p); 
		touch();
	};

	void RenderableEntity::set_Texture(Texture* t)
	{
		tex = t;
		texsize = t->get_resolution();
		spritesize = texsize;
		touch();
	};

	void RenderableEntity::set_Texture(Texture& t) { set_Texture(&t); };

	void RenderableEntity::set_Texture(Spritemap* sm)
	{
		tex = sm;
		texsize = sm->get_texture().get_resolution();
		spritesize = texsize;
		touch();
	};

	void RenderableEntity::set_Texture(Spritemap* sm,
		const glm::ivec2& offset, const glm::ivec2& resolution)
	{
		tex = sm;
		texoffset = offset;
		texsize = resolution;
		spritesize = sm->get_texture().get_resolution();
		touch();
	};

	void RenderableEntity::set_Texture(Spritemap& sm)
	{
		set_Texture(&sm);
	};

	void RenderableEntity::set_Texture(Spritemap& sm,
		const glm::ivec2& offset, const glm::ivec2& resolution)
	{
		set_Texture(&sm, offset, resolution);
	};

	void RenderableEntity::set_Multibuffer(Multibuffer* b)
	{
		data = b; 
		touch();
	};

	void RenderableEntity::set_Multibuffer(Multibuffer& b)
	{
		data = &b; 
		touch();
	};

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

	void RenderableEntity::default_Multibuffer()
	{
		set_Multibuffer(*default_quadmesh_ptr);
		touch();
	}

	void RenderableEntity::default_ShaderProgram()
	{
		set_ShaderProgram(default_shader);
		touch();
	}

	void RenderableEntity::default_Texture()
	{
		set_Texture(default_texture);
		touch();
	}

	void RenderableEntity::touch()
	{
		touched = true;
	}
	void RenderableEntity::untouch()
	{
		touched = false;
	};

	bool RenderableEntity::get_touched() const
	{
		return touched;
	}

	void RenderableEntity::set_Type(const entity_type t)
	{
		type = t;
	}

	entity_type RenderableEntity::get_Type() const
	{
		return type;
	}

	void RenderableEntity::default_State()
	{
		default_Multibuffer();
		default_ShaderProgram();
		default_Texture();
	}
}}}