#include "core/rendering/renderable_entity.h"

namespace eversim {	namespace core { namespace rendering {

	std::unique_ptr<Multibuffer> default_quadmesh_ptr = nullptr;
	Texture default_texture(glm::ivec2(4,4));
	ShaderProgram default_shader("default uv shader");

	RenderableEntity::RenderableEntity() : 
		program(&default_shader), tex(&default_texture), data(&(*default_quadmesh_ptr))
	{}

	void RenderableEntity::bind() const
	{
		data->bind();
		if (tex != nullptr) tex->bind();
	}

	void RenderableEntity::draw() const
	{
		data->draw();
	}

	void  RenderableEntity::set_Position(glm::fvec2 pos)
	{
		M[2] = glm::fvec3(pos,1.f);
	}

	glm::fvec2  RenderableEntity::get_Position() const
	{
		return { M[2][0], M[2][1] };
	}

	void RenderableEntity::set_Scale(glm::fvec2 scale)
	{
		M[0] = { scale[0], 0, 0 };
		M[1] = { 0, scale[1], 0};
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