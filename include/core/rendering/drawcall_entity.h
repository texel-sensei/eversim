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
				bool touched = true;

				std::weak_ptr<ShaderProgram> program_ptr;
				std::weak_ptr<Multibuffer> buffer_ptr;

				Spritemap spritemap = Spritemap(512); //TODO
				shader_storage_buffer ssb;

				std::vector<instanced_entity_information> entity_info;
				std::map<size_t, TextureBase*> found_textures;
				std::vector<std::weak_ptr<RenderableEntity>> entities;

				void invalidate_if_expired();

			public:

				DrawcallEntity(
					std::weak_ptr<ShaderProgram> program_ptr,
					std::weak_ptr<Multibuffer> buffer_ptr
				);

				void touch();
				void untouch();
				bool get_touched() const;

				std::weak_ptr<Multibuffer> get_Multibuffer() const
				{
					return buffer_ptr;
				}

				void draw(Camera& cam);

				/*
				 * Add the data of the entity to the DrawcallEntity
				 * returns the index of the entity data
				 */
				size_t add_entity(std::weak_ptr<RenderableEntity> entity);

				instanced_entity_information get_entity_data(const size_t idx) const;

				void upload();
			};

		}
	}
}