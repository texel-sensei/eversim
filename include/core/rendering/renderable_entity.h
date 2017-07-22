#pragma once

#include "core/utility/plattform.h"
#include "core/rendering/shader_program.h"

#include <easylogging++.h>
#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {
			class RenderableEntity {
			private:
				ShaderProgram* program = nullptr;
				RenderableEntity();

			public:
				void bind();

				friend class render_manager;
			};
		}
	}
}