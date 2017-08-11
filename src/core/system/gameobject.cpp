#include "core/system/gameobject.h"
#include "core/system/game.h"

namespace eversim { namespace core { namespace system {
	gameobject* gameobject::clone() const
	{
		auto clone = the_game->create_empty();
		
		clone->position = this->position;
		clone->scale = this->scale;
		clone->angle = this->angle;

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

	void gameobject::update(utility::clock::duration time_passed)
	{
	}
}}}
