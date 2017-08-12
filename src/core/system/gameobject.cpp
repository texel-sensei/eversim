#include "core/system/gameobject.h"
#include "core/system/game.h"
#include <boost/range/adaptor/map.hpp>

namespace eversim { namespace core { namespace system {
	gameobject* gameobject::clone() const
	{
		auto clone = the_game->create_empty();
		
		clone->position = this->position;
		clone->scale = this->scale;
		clone->angle = this->angle;

		for(auto& c : components_ | boost::adaptors::map_values)
		{
			clone->add_component(c->clone());
		}

		for(auto& child : children)
		{
			auto cloned_child = child->clone();
			clone->add_child(cloned_child);
		}
		return clone;
	}

	void gameobject::kill()
	{
		the_game->kill_object(this);
	}

	component* gameobject::add_component(std::unique_ptr<component> comp)
	{
		if(!comp)
		{
			EVERSIM_THROW(generic_error::InvalidArgument, "Null component not allowed!");
		}

		auto type = std::type_index(typeid(*comp));
		if(has_component(type))
		{
			EVERSIM_THROW(game_error::DuplicateComponent, type.name());
		}
		components_[type] = move(comp);
		return comp.get();
	}

	component* gameobject::get_component(std::type_index type)
	{
		const auto it = components_.find(type);
		if (it == components_.end()) return nullptr;
		return it->second.get();
	}

	gameobject::gameobject(creation_key, game* the_game)
		:the_game(the_game)
	{
	}

	void gameobject::add_child(gameobject* child)
	{
		EVERSIM_ASSERT(child->parent != this);
		EVERSIM_ASSERT(child != this);
		children.push_back(child);
		child->parent = this;
	}

	void gameobject::set_parent(gameobject* parent)
	{
		EVERSIM_ASSERT(this->parent != parent);
		EVERSIM_ASSERT(parent != this);
		parent->add_child(this);
	}

	void gameobject::update()
	{
		for(auto& c : components())
		{
			c.update();
		}
	}

	void gameobject::post_physics_update()
	{
		for(auto& c : components())
		{
			c.post_physics_update();
		}
	}

	component const* gameobject::get_component(std::type_index type) const
	{
		const auto it = components_.find(type);
		if (it == components_.end()) return nullptr;
		return it->second.get();
	}

	bool gameobject::has_component(std::type_index type) const
	{
		return get_component(type);
	}

	boost::any_range<component&, boost::bidirectional_traversal_tag> gameobject::components()
	{
		return boost::adaptors::transform( components_,
			[](auto& pair) -> component& {
				return *pair.second;
		});
	}


}}}
