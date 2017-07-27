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

			extern Multibuffer quadmesh;

			class RenderableEntity {
			private:

				glm::fmat3 M = glm::fmat3(1.f);
				ShaderProgram* program = nullptr;
				Texture* tex = nullptr;
				Multibuffer* data;

				RenderableEntity();

				void bind() const;
				void draw() const;

			public:

				glm::fmat3 get_M() const { return M; };
				ShaderProgram* get_ShaderProgram() const { return program; };
				Texture* get_Texture() const { return tex; };
				Multibuffer* get_Multibuffer() const { return data; };

				void set_M(const glm::fmat3& m) { M = m; };
				void set_ShaderProgram(ShaderProgram& p) { program = &p; };
				void set_Texture(Texture& t) { tex = &t; };
				void set_Multibuffer(Multibuffer& b) { data = &b; };

				void set_ShaderProgram(ShaderProgram* p) { program = p; };
				void set_Texture(Texture* t) { tex = t; };
				void set_Multibuffer(Multibuffer* b){ data = b;	};

				void default_Multibuffer(){	data = &quadmesh; }

				friend class render_manager;
			};
		}
	}
}