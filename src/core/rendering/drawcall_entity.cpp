#include "core/rendering/drawcall_entity.h"

namespace eversim {
	namespace core {
		namespace rendering {

			void DrawcallEntity::invalidate_if_expired()
			{
				/*valid = !(program_ptr == 1 ||
					texture_ptr == ||
					buffer_ptr.expired());*/
				//TODO !!!
			}

			DrawcallEntity::DrawcallEntity(
				ShaderProgram* program_ptr,
				TextureBase* texture_ptr,
				Multibuffer* buffer_ptr
			) : 
				program_ptr(program_ptr),
				texture_ptr(texture_ptr),
				buffer_ptr(buffer_ptr)
			{
				invalidate_if_expired();
			}

			void DrawcallEntity::draw(Camera& cam)
			{
				invalidate_if_expired();
				if(!valid)
				{
					LOG(ERROR) << "DrawcallEntity has expired weak_ptrs";
					return;
				} 

				auto& program = *program_ptr;
				auto& texture = *texture_ptr;
				auto& buffer = *buffer_ptr;

				program.use();
				cam.use(program);

				buffer.bind();

				ssb.bind(42);

				texture.bind();

				glDrawArraysInstanced(buffer.type, buffer.first, buffer.count, entity_info.size());

				glUseProgram(0);
			}

			void DrawcallEntity::touch()
			{
				touched = true;
			}

			void DrawcallEntity::untouch()
			{
				touched = false;
			}

			bool DrawcallEntity::get_touched() const
			{
				return touched;
			}

			size_t DrawcallEntity::add_entity_data(const RenderableEntity& entity)
			{
				entity_info.emplace_back();
				auto& info = entity_info.back();
				entity.get_instanced_entity_information(info);
				return entity_info.size() - 1;
			}

			size_t DrawcallEntity::add_entity_data(const RenderableEntity& entity,
				const glm::ivec2 texoffset,
				const glm::ivec2 texsize,
				const glm::ivec2 spritesize
			)
			{
				const auto size = add_entity_data(entity);
				auto& info = entity_info.back();
				info.set_texoffset(texoffset);
				info.set_texsize(texsize);
				info.set_spritesize(spritesize);
				return size;
			}

			instanced_entity_information DrawcallEntity::get_entity_data(const size_t idx) const
			{
				return entity_info.at(idx);
			}

			void DrawcallEntity::upload()
			{
				utility::byte_array_view view(entity_info);
				ssb = shader_storage_buffer(view);
			}
		}
	}
}