#pragma once

#include "core/utility/plattform.h"
#include "core/rendering/shader_program.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/camera.h"
#include "core/rendering/texture.h"

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
				Texture* tex = nullptr;
				Multibuffer data;

				void bind() const;
				void draw() const;

				glm::fmat3 get_M() const { return M; };
				void set_M(const glm::fmat3& m) { M = m; };

				friend class render_manager;
			};
		}
	}
}