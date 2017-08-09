#pragma once

#include "core/rendering/camera.h"
#include "core/rendering/shader_program.h"
#include "core/rendering/renderable_entity.h"
#include "core/rendering/multibuffer.h"
#include "core/rendering/shader_storage_buffer.h"

#include <glm/glm.hpp>

#include <boost/align/aligned_allocator.hpp>

namespace eversim {
	namespace core {
		namespace rendering {

			class DrawcallEntity
			{
			private:
				bool valid = true;

				bool touched = false;
				std::vector<size_t> entity_touched;

				std::weak_ptr<ShaderProgram> program_ptr;
				std::weak_ptr<Multibuffer> buffer_ptr;

				Spritemap spritemap = Spritemap(512); //TODO
				shader_storage_buffer ssb;

				
				std::map<size_t, TextureBase*> found_textures;
				std::map<size_t, glm::ivec2> texture_offsets;


				const size_t extra_space = 500;
				std::vector<std::weak_ptr<RenderableEntity>> entities;
				std::vector<instanced_entity_information> entity_info;
				size_t idx_add = 0;

				void invalidate_if_expired();

				void update_instanced_entity_information(const size_t idx);

				void reduce();


			public:

				DrawcallEntity(
					std::weak_ptr<ShaderProgram> program_ptr,
					std::weak_ptr<Multibuffer> buffer_ptr
				);

				void touch();
				void touch(const size_t idx);
				bool get_touched() const;

				std::weak_ptr<Multibuffer> get_Multibuffer() const
				{
					return buffer_ptr;
				}

				void draw(Camera& cam);
				void update();

				/*
				 * Add the data of the entity to the DrawcallEntity
				 * returns the index of the entity data
				 */
				size_t add_entity(std::weak_ptr<RenderableEntity> entity);

				void move_entity(size_t entity_idx, std::weak_ptr<DrawcallEntity> target);
				void remove_entity(size_t entity_idx);

				instanced_entity_information get_entity_data(const size_t idx) const;
				void remove_expired_entities();
				void upload();
			};

		}
	}
}