#pragma once

#include <glm/glm.hpp>

namespace eversim
{
	namespace core
	{
		namespace rendering
		{
			class TextureBase
			{
			public:
				virtual void bind() const = 0;
			};
		}
	}
}