#include "core/rendering/drawcall.h"
#include "core/utility/helper.h"

#include <exception>

using namespace std;

namespace eversim::core::rendering {

	void Drawcall::invalidate_if_expired()
	{
		valid = !(program_ptr.expired() ||
				buffer_ptr.expired());
	}

	Drawcall::Drawcall(
			std::weak_ptr<ShaderProgram> program_ptr,
			std::weak_ptr<Multibuffer> buffer_ptr
			) :
		program_ptr(program_ptr),
		buffer_ptr(buffer_ptr)
	{
		invalidate_if_expired();
		if(valid)
		{
			entities.resize(extra_space);
			entity_info.resize(extra_space);
		}
	}

	void Drawcall::draw(Camera& cam)
	{
		invalidate_if_expired();
		if(!valid)
		{
			LOG(ERROR) << "Drawcall has expired weak_ptrs";
			return;
		}

		auto program = program_ptr.lock();
		auto buffer = buffer_ptr.lock();

		program->use();
		cam.use(*program);

		buffer->bind();

		ssb.bind(42);

		spritemap.bind();

		glDrawArraysInstanced(
			buffer->type, buffer->first,
			buffer->count, static_cast<GLsizei>(idx_add)
		);

		glUseProgram(0);
	}

	void Drawcall::touch(const size_t idx)
	{
		touch();
		//TODO use std::set
		auto it = find_if(begin(entity_touched), end(entity_touched),
				[&](size_t i) { return idx == i; }
				);
		if(it == end(entity_touched))
			entity_touched.push_back(idx);
	}

	void Drawcall::touch()
	{
		touched = true;
	}

	bool Drawcall::get_touched() const
	{
		return touched;
	}

	size_t Drawcall::add_entity(std::weak_ptr<RenderableEntity> entity_ptr)
	{
		if (entity_ptr.expired())
		{
			LOG(ERROR) << "Drawcall: add_entity of expired entity";
			throw runtime_error("Drawcall: add_entity of expired entity");
		}

		touch();

		const auto idx = idx_add;

		if(idx_add >= entities.size())
		{
			entities.resize(entities.size()+extra_space);
			entity_info.resize(entities.size());
		}

		entities.at(idx_add) = entity_ptr;

		idx_add++;

		auto& entity = *(entity_ptr.lock());

		auto tex_wkptr = entity.get_Texture();

		if(tex_wkptr.expired())
		{
			LOG(ERROR) << "Drawcall: entity has an expired texture";
			throw runtime_error("Drawcall: entity has an expired texture");
		}

		auto* tex = &(*(tex_wkptr.lock()));

		found_textures[tex->get_unique_id()] = tex;

		//LOG(INFO) << "touched " << idx << " size => "<< idx_add ;
		touch(idx);

		return idx;
	}

	void Drawcall::reduce()
	{
		idx_add--;
		if (entities.size() > extra_space && idx_add+1 <= entities.size() - extra_space)
		{
			entities.resize(idx_add+1);
			entity_info.resize(idx_add+1);
		}
		touch();
	}

	void Drawcall::move_entity(size_t entity_idx, std::weak_ptr<Drawcall> target)
	{
		if (idx_add == 0 || target.expired()) return;

		auto entity_ptr = entities.at(entity_idx);
		auto& entity = *entity_ptr.lock();

		auto idx = target.lock()->add_entity(entity_ptr);
		entity.set_Drawer(target,idx);

		remove_entity(entity_idx);
	}

	void Drawcall::remove_entity(size_t entity_idx)
	{
		if (entity_idx >= idx_add) return;

		auto end_ptr = remove_if(begin(entity_touched), end(entity_touched),
				[&]( size_t idx )
				{
				return idx == entity_idx;
				});

		entity_touched = std::vector<size_t>(begin(entity_touched), end_ptr);

		if(entity_idx < idx_add-1)
		{
			std::swap(entities.at(idx_add-1), entities.at(entity_idx));
			std::swap(entity_info.at(idx_add-1), entity_info.at(entity_idx));

			auto entity_ptr = entities.at(entity_idx);

			if (!entity_ptr.expired())
			{
				auto& entity = *entity_ptr.lock();
				entity.set_Drawer(entity_idx);
			}

			touch(entity_idx);
		}

		reduce();


	}

	void Drawcall::remove_expired_entities()
	{
		for(size_t entity_idx = 0; entity_idx < idx_add; entity_idx++)
		{
			if (entities.at(entity_idx).expired())
				remove_entity(entity_idx);
		}
	}

	instanced_entity_information Drawcall::get_entity_data(const size_t idx) const
	{
		return entity_info.at(idx);
	}

	void Drawcall::upload()
	{
		for (auto& tex : found_textures)
		{
			auto* texbaseptr = tex.second;
			auto uid = texbaseptr->get_unique_id();

			auto count = added_textures.count(uid);

			if(count == 0)
			{
				texture_offsets[uid] = spritemap.add_texture(*texbaseptr);
				added_textures.emplace(uid);
			}
		}

		for (const auto idx : entity_touched)
		{
			update_instanced_entity_information(idx);
		}

		if (ssb_size < entity_info.size())
		{
			LOG(INFO) << "creating ssb of size " << entity_info.size();
			utility::byte_array_view view(entity_info);
			ssb_size = entity_info.size();
			ssb = shader_storage_buffer(view);
		} else
		{
			for (const auto idx : entity_touched)
			{
				auto& info = entity_info.at(idx);
				utility::byte_array_view view(
						reinterpret_cast<utility::byte*>(&info),sizeof(info)
						);
				ssb.update(view, idx * sizeof(instanced_entity_information));
			}
		}

		entity_touched.clear();
		touched = false;

	}

	void Drawcall::update_instanced_entity_information(const size_t idx)
	{
		if (idx >= idx_add) return;

		auto entity_wkptr = entities.at(idx);
		if (entity_wkptr.expired()) return;
		auto entity = entity_wkptr.lock();

		auto unique_id = entity->get_Texture().lock()->get_unique_id();
		auto* tex = found_textures[unique_id];

		auto& info = entity_info.at(idx);
		entity->get_instanced_entity_information(info);

		info.set_texoffset(texture_offsets[unique_id]);
		info.set_texsize(tex->get_resolution());
		info.set_spritesize(spritemap.get_resolution());
	}
}
