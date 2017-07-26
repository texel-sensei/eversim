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

}}}