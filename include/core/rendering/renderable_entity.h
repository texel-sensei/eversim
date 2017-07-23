#pragma once

#include "core/utility/plattform.h"
#include "core/rendering/shader_program.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/camera.h"

#include <easylogging++.h>
#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {
			class RenderableEntity {
			private:

				glm::fmat3 M = glm::fmat3(1.f);

				RenderableEntity();

			public:
				Camera* cam = nullptr;
				ShaderProgram* program = nullptr;
				Multibuffer data;

				void bind() const;
				void draw() const;

				glm::fmat3 get_M() const { return M; };

				friend class render_manager;
			};
		}
	}
}