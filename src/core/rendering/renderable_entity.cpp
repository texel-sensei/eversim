#include "core/rendering/renderable_entity.h"

namespace eversim {	namespace core { namespace rendering {

	Multibuffer quadmesh("default quad mesh");

	RenderableEntity::RenderableEntity() : data(&quadmesh)
	{
		
	}

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

}}}