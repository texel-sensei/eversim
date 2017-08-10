#include "core/system/game.h"

namespace eversim { namespace core { namespace system {

	gameobject* game::create_empty()
	{
		return objects.emplace(gameobject::creation_key{}, this);
	}

	void game::kill_object(gameobject* o)
	{
		if (o == nullptr) return;
		o->alive = false;
		dead_objects.insert(o);
		for(auto& child : o->children)
		{
			kill_object(child);
		}
	}

	void game::step_frame()
	{
	}

	void game::cleanup_dead()
	{
		for(auto& o : dead_objects)
		{
			objects.erase(objects.locate(o));
		}
	}

	void game::add_resources_directory(std::string const& path)
	{
	}
}}}
