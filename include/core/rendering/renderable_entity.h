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

			/*
			* unit sized quad 0/0 to 1/1
			*/
			extern Multibuffer default_quadmesh;
			/*
			 * default shader takes positions and uv coordinates
			 * and texture
			 */
			extern ShaderProgram default_shader;
			/*
			 * default texture 4x4 basic color
			 */
			extern Texture default_texture;

			class RenderableEntity {
			private:

				glm::fmat3 M = glm::fmat3(1.f);
				ShaderProgram* program;
				Texture* tex;
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

				void set_Position(glm::fvec2 pos);
				glm::fvec2 get_Position() const;

				void set_Scale(glm::fvec2 scale);
				glm::fvec2 get_Scale() const;

				void set_ShaderProgram(ShaderProgram* p) { program = p; };
				void set_Texture(Texture* t) { tex = t; };
				void set_Multibuffer(Multibuffer* b){ data = b;	};

				void default_Multibuffer(){	data = &default_quadmesh; }
				void default_ShaderProgram() { program = &default_shader; }
				void default_Texture() { tex = &default_texture; }
				void default_State();

				friend class render_manager;
			};
		}
	}
}