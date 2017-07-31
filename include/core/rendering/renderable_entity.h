#pragma once


#include "core/rendering/shader_program.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/texture.h"
#include "core/rendering/spritemap.h"
#include "core/rendering/texture_base_class.h"

#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {

			enum entity_type
			{
				DYNAMIC, STATIC
			};

			struct alignas(16) instanced_entity_information
			{
				glm::fmat4 M;
				explicit instanced_entity_information(const glm::fmat4& m) : M(m){}
			};

			/*
			* unit sized quad 0/0 to 1/1
			*/
			extern std::unique_ptr<Multibuffer> default_quadmesh_ptr;
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
				glm::ivec2 texoffset = glm::ivec2(0,0);
				glm::ivec2 texsize, spritesize;
				glm::fmat3 M = glm::fmat3(1.f);
				ShaderProgram* program;
				TextureBase* tex;
				Multibuffer* data;
				entity_type type=DYNAMIC;

				RenderableEntity();

				void bind() const;
				void draw() const;

				bool touched = false;
				void touch() { touched = true; }
				void untouch() { touched = false; };
			public:

				glm::fmat3 get_M() const { return M; };
				ShaderProgram* get_ShaderProgram() const { return program; };
				TextureBase* get_Texture() const { return tex; };
				Multibuffer* get_Multibuffer() const { return data; };

				void set_M(const glm::fmat3& m) { M = m; touch(); };

				void set_ShaderProgram(ShaderProgram* p) { program = p; touch(); };
				void set_ShaderProgram(ShaderProgram& p) { set_ShaderProgram(&p); touch(); };

				void set_Texture(Texture* t)
				{
					tex = t;
					texsize = t->get_resolution();
					spritesize = texsize;
					touch();
				};
				void set_Texture(Texture& t) { set_Texture(&t); };

				void set_Texture(Spritemap* sm)
				{
					tex = sm;
					texsize = sm->get_texture().get_resolution();
					spritesize = texsize;
					touch();
				};
				void set_Texture(Spritemap* sm,
					const glm::ivec2& offset, const glm::ivec2& resolution)
				{
					tex = sm;
					texoffset = offset;
					texsize = resolution;
					spritesize = sm->get_texture().get_resolution();
					touch();
				};

				void set_Texture(Spritemap& sm) 
						{ set_Texture(&sm); };
				void set_Texture(Spritemap& sm, 
					const glm::ivec2& offset, const glm::ivec2& resolution)
						{ set_Texture(&sm, offset, resolution);	};

				void set_Multibuffer(Multibuffer* b) { data = b; touch(); };
				void set_Multibuffer(Multibuffer& b) { data = &b; touch(); };

				void set_Position(glm::fvec2 pos);
				glm::fvec2 get_Position() const;

				void set_Scale(glm::fvec2 scale);
				glm::fvec2 get_Scale() const;
			
				void default_Multibuffer()
				{
					set_Multibuffer(*default_quadmesh_ptr);
					touch();
				}
				void default_ShaderProgram()
				{
					set_ShaderProgram(default_shader);
					touch();
				}
				void default_Texture()
				{
					set_Texture(default_texture);
					touch();
				}
				void default_State();

				bool get_touched() const { return touched; }

				void set_Type(const entity_type t) { type = t; }
				entity_type get_Type() const { return type; }

				friend class render_manager;
			};
		}
	}
}