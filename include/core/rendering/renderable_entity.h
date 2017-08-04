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
			private:
				glm::fmat4 data;
			public:

				//setter
				void set_M(const glm::fmat3& M)
				{
					for (size_t i = 0; i < 3; ++i)
						for (size_t j = 0; j < 3; ++j)
							data[i][j] = M[i][j];
				}

				void set_texoffset(const glm::ivec2& texoffset)
				{
					data[0][3] = texoffset[0];
					data[1][3] = texoffset[1];
				}

				void set_texsize(const glm::ivec2& texsize)
				{
					data[2][3] = texsize[0];
					data[3][3] = texsize[1];
				}

				void set_spritesize(const glm::ivec2& spritesize)
				{
					data[3][0] = spritesize[0];
					data[3][1] = spritesize[1];
				}
				//getter
				glm::fmat3 get_M() const
				{
					glm::fmat3 M;
					for (size_t i = 0; i < 3; ++i)
						for (size_t j = 0; j < 3; ++j)
							M[i][j] = data[i][j];
					return M;
				}

				glm::ivec2 get_texoffset() const
				{
					return { data[0][3] , data[1][3] };
				}

				glm::ivec2 get_texsize() const
				{
					return { data[2][3] , data[3][3] };
				}

				glm::ivec2 get_spritesize() const
				{
					return { data[3][0] , data[3][1] };
				}
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

				bool touched = false;
				void touch();
				void untouch();
			public:

				const instanced_entity_information& get_instanced_entity_information() const;
				void get_instanced_entity_information(instanced_entity_information& ifo) const;

				glm::fmat3 get_M() const;
				ShaderProgram* get_ShaderProgram() const;
				TextureBase* get_Texture() const;
				Multibuffer* get_Multibuffer() const;

				void set_M(const glm::fmat3& m);

				void set_ShaderProgram(ShaderProgram* p);
				void set_ShaderProgram(ShaderProgram& p);

				void set_Texture(Texture* t);
				void set_Texture(Texture& t);

				void set_Texture(Spritemap* sm);
				void set_Texture(Spritemap* sm,
					const glm::ivec2& offset, const glm::ivec2& resolution);

				void set_Texture(Spritemap& sm);
				void set_Texture(Spritemap& sm, 
					const glm::ivec2& offset, const glm::ivec2& resolution);

				void set_Multibuffer(Multibuffer* b);
				void set_Multibuffer(Multibuffer& b);

				void set_Position(glm::fvec2 pos);
				glm::fvec2 get_Position() const;

				void set_Scale(glm::fvec2 scale);
				glm::fvec2 get_Scale() const;
			
				void default_Multibuffer();
				void default_ShaderProgram();
				void default_Texture();
				void default_State();

				bool get_touched() const;

				void set_Type(const entity_type t);
				entity_type get_Type() const;

				friend class render_manager;
			};
		}
	}
}