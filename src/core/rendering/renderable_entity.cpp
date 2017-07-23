#include "core/rendering/renderable_entity.h"

namespace eversim {	namespace core { namespace rendering {

	RenderableEntity::RenderableEntity() : data("default_name")
	{
		
	}

	void RenderableEntity::bind() const
	{
		data.bind();
	}

	void RenderableEntity::draw() const
	{
		data.draw();
	}

}}}