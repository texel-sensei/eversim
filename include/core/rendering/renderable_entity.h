#pragma once


#include "core/rendering/shader_program.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/spritemap.h"
#include "core/rendering/texture_base_class.h"

#include <glm/glm.hpp>

namespace eversim {
	namespace core {
		namespace rendering {

			class DrawcallEntity;

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
					for (auto i = 0; i < 3; ++i)
						for (auto j = 0; j < 3; ++j)
							data[i][j] = M[i][j];
				}

				void set_texoffset(const glm::ivec2& texoffset)
				{
					data[0][3] = static_cast<float>(texoffset[0]);
					data[1][3] = static_cast<float>(texoffset[1]);
				}

				void set_texsize(const glm::ivec2& texsize)
				{
					data[2][3] = static_cast<float>(texsize[0]);
					data[3][3] = static_cast<float>(texsize[1]);
				}

				void set_spritesize(const glm::ivec2& spritesize)
				{
					data[3][0] = static_cast<float>(spritesize[0]);
					data[3][1] = static_cast<float>(spritesize[1]);
				}
				//getter
				glm::fmat3 get_M() const
				{
					glm::fmat3 M;
					for (auto i = 0; i < 3; ++i)
						for (auto j = 0; j < 3; ++j)
							M[i][j] = data[i][j];
					return M;
				}

				glm::ivec2 get_texoffset() const
				{
					return { static_cast<int>(data[0][3]) ,static_cast<int>(data[1][3]) };
				}

				glm::ivec2 get_texsize() const
				{
					return { static_cast<int>(data[2][3]) , static_cast<int>(data[3][3]) };
				}

				glm::ivec2 get_spritesize() const
				{
					return { static_cast<int>(data[3][0]) , static_cast<int>(data[3][1]) };
				}
			};

			class RenderableEntity {

				std::pair<long long,std::weak_ptr<DrawcallEntity>> assigned_drawer 
					= {-1,std::weak_ptr<DrawcallEntity>()};
				
				glm::ivec2 texoffset = glm::ivec2(0,0);
				glm::ivec2 texsize, spritesize;
				
				glm::fvec2 position = glm::fvec2(0);
				glm::fvec2 scale = glm::fvec2(1);
				float rotation = 0.f;
				glm::fvec2 center_of_rotation = glm::fvec2(0);

				std::weak_ptr<ShaderProgram> program;
				std::weak_ptr<TextureBase> tex;
				std::weak_ptr<Multibuffer> data;

				entity_type type;

				explicit RenderableEntity(
					std::shared_ptr<ShaderProgram> program,
					std::shared_ptr<TextureBase> tex,
					std::weak_ptr<Multibuffer>  data,
					const entity_type t=DYNAMIC);

				bool touched = false;
				void touch();
				void untouch();

				void set_Drawer(size_t);
			public:

				~RenderableEntity();

				instanced_entity_information get_instanced_entity_information() const;
				void get_instanced_entity_information(instanced_entity_information& ifo) const;

				glm::fmat3 get_M() const;
				std::weak_ptr<ShaderProgram> get_ShaderProgram() const;
				std::weak_ptr<TextureBase> get_Texture() const;
				std::weak_ptr<Multibuffer> get_Multibuffer() const;

				void set_ShaderProgram(std::shared_ptr<ShaderProgram> p);

				void set_Texture(std::shared_ptr<TextureBase> p);
				void set_Texture(std::shared_ptr<TextureBase> p,
					const glm::ivec2& offset, const glm::ivec2& resolution);
				void set_Multibuffer(std::shared_ptr<Multibuffer> p);

				void set_Position(glm::fvec2 pos);
				glm::fvec2 get_Position() const;

				void set_Scale(glm::fvec2 scale);
				glm::fvec2 get_Scale() const;

				void set_Rotation(float rotation);
				float get_Rotation() const;

				void set_Center(glm::fvec2);
				glm::fvec2 get_Center() const;
			
				bool get_touched() const;

				void set_Type(const entity_type t);
				entity_type get_Type() const;

				void set_Drawer(std::weak_ptr<DrawcallEntity>,size_t);
				
				std::weak_ptr<DrawcallEntity> get_Drawer() const;
				long long get_Drawer_idx() const;

				friend class render_manager;
				friend class DrawcallEntity;
			};
		}
	}
}