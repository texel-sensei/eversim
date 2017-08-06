#include "core/rendering/drawcall_entity.h"
#include "core/utility/helper.h"

#include <exception>

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {

			void DrawcallEntity::invalidate_if_expired()
			{
				valid = !(program_ptr.expired() ||
					buffer_ptr.expired());
			}

			DrawcallEntity::DrawcallEntity(
				std::weak_ptr<ShaderProgram> program_ptr,
				std::weak_ptr<Multibuffer> buffer_ptr
			) : 
				program_ptr(program_ptr),
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

				auto& program = *program_ptr.lock();
				auto& buffer = *buffer_ptr.lock();

				program.use();
				cam.use(program);

				buffer.bind();

				ssb.bind(42);

				spritemap.bind();

				glDrawArraysInstanced(buffer.type, buffer.first, 
					buffer.count, static_cast<GLsizei>(entity_info.size()));

				glUseProgram(0);
			}

			void DrawcallEntity::touch(const size_t idx)
			{
				touch();
				entity_touched.push_back(idx);
			}

			void DrawcallEntity::touch()
			{
				touched = true;
			}

			bool DrawcallEntity::get_touched() const
			{
				return touched;
			}

			size_t DrawcallEntity::add_entity(std::weak_ptr<RenderableEntity> entity_ptr)
			{
				if (entity_ptr.expired())
				{
					LOG(ERROR) << "DrawcallEntity: add_entity of expired entity";
					throw exception("DrawcallEntity: add_entity of expired entity");
				}

				entities.push_back(entity_ptr);
				auto& entity = *(entity_ptr.lock());

				auto tex_wkptr = entity.get_Texture();

				if(tex_wkptr.expired())
				{
					LOG(ERROR) << "DrawcallEntity: entity has an expired texture";
					throw exception("DrawcallEntity: entity has an expired texture");
				}

				auto* tex = &(*(tex_wkptr.lock()));

				found_textures[tex->get_unique_id()] = tex;

				return entities.size() - 1;
			}

			instanced_entity_information DrawcallEntity::get_entity_data(const size_t idx) const
			{
				return entity_info.at(idx);
			}

			void DrawcallEntity::upload()
			{
				for (auto& tex : found_textures)
				{
					auto* texbaseptr = tex.second;
					texture_offsets[texbaseptr->get_unique_id()] =
						spritemap.add_texture(*texbaseptr);
				}

				size_t idx = 0;

				entity_info.resize(entities.size());
				for (auto entity_ptr : entities)
				{
					if(entity_ptr.expired())
					{
						LOG(ERROR) << "expired entity";
						continue;
					}
					
					update_instanced_entity_information(idx);

					idx++;
				}

				utility::byte_array_view view(entity_info);
				ssb = shader_storage_buffer(view);
			}

			void DrawcallEntity::update_instanced_entity_information(const size_t idx)
			{
				auto entity_wkptr = entities.at(idx);
				if (entity_wkptr.expired()) return;
				auto& entity = *entity_wkptr.lock();

				auto unique_id = entity.get_Texture().lock()->get_unique_id();
				auto* tex = found_textures[unique_id];

				auto& info = entity_info.at(idx);
				entity.get_instanced_entity_information(info);

				info.set_texoffset(texture_offsets[unique_id]);
				info.set_texsize(tex->get_resolution());
				info.set_spritesize(spritemap.get_resolution());
			}

			void DrawcallEntity::update()
			{
				for(const auto idx : entity_touched)
				{
					update_instanced_entity_information(idx);

					auto& info = entity_info.at(idx);

					std::vector<instanced_entity_information> tmp = {info};
					utility::byte_array_view view(tmp);
					
					ssb.update(view, idx);
				}
				entity_touched.clear();
			}
		}
	}
}