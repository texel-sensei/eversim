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
			private:
				size_t unique_id;

			public:
				virtual void bind() const = 0;
				virtual glm::ivec2 get_resolution() const = 0;

				size_t get_unique_id() const;
				void set_unique_id(const size_t);
			};
		}
	}
}