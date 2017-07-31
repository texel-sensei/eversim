#include "core/rendering/texture.h"
#include "core/rendering/render_manager.h"

#include "core/world/tile_descriptor.h"
#include "core/world/level_loader.h"
#include "core/world/level.h"

#include "core/physics/constraints/angle_constraint.h"
#include "core/physics/body_template.h"
#include "core/physics/constraints/distance_constraint.h"
#include "core/physics/physics_manager.h"

#include <easylogging++.h>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#undef main

INITIALIZE_EASYLOGGINGPP

using namespace std;
using namespace eversim::core;

world::tile_descriptor dirt, grass, bricks;

bool direction_pressed[4];
bool handle_keypress(SDL_Keysym sym, bool down)
{
	switch (sym.sym)
	{
	case SDLK_q:
	case SDLK_ESCAPE:
		return false;
	case SDLK_w:
		direction_pressed[0] = down;
		break;
	case SDLK_a:
		direction_pressed[1] = down;
		break;
	case SDLK_s:
		direction_pressed[2] = down;
		break;
	case SDLK_d:
		direction_pressed[3] = down;
		break;
	}
	return true;
}

bool handle_sdl_events()
{
	SDL_Event event;
	auto should_continue = true;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			should_continue &= handle_keypress(event.key.keysym, true);
			break;
		case SDL_KEYUP:
			should_continue &= handle_keypress(event.key.keysym, false);
			break;
		default:
			break;
		}
	}
	return should_continue;
}

glm::vec2 get_input_speed(float speedmult, float dt)
{
	auto vel = glm::vec2{};
	if (direction_pressed[0])
		vel += glm::vec2{ 0.f, 3.f };
	if (direction_pressed[1])
		vel += glm::vec2{ -1.f, 0.f };
	if (direction_pressed[2])
		vel += glm::vec2{ 0.f, -1.f };
	if (direction_pressed[3])
		vel += glm::vec2{ 1.f, 0.f };
	return vel * speedmult * dt;
}

void initialize_tiles(world::level_loader& loader)
{
	dirt.name = "dirt";
	dirt.collision = world::collision_type::solid;
	dirt.texture_name = "dirt0.png";

	grass.name = "grass";
	grass.collision = world::collision_type::solid;
	grass.texture_name = "grass.png";

	bricks.name = "bricks";
	bricks.collision = world::collision_type::solid;
	bricks.texture_name = "stone_brick1.png";

	loader.register_tile_descriptor(&dirt);
	loader.register_tile_descriptor(&grass);
	loader.register_tile_descriptor(&bricks);
}

struct loaders {
	utility::texture_loader* tex;
	world::level_loader* lev;
	physics::body_template_loader* bdy;

	void add_directory(string const& path)
	{
		tex->add_search_directory(path + "/sprites");
		lev->add_search_directory(path + "/levels");
		bdy->add_search_directory(path + "/physics");
	}
};

int main(int argc, char* argv[])
{
	START_EASYLOGGINGPP(argc, argv);

	// init rendering
	const auto resolution = glm::ivec2(1920,1080);
	auto renderer = rendering::render_manager(resolution, false);

	// create loaders
	world::level_loader level_loader;
	initialize_tiles(level_loader);

	physics::body_template_loader body_loader;
	body_loader.register_factory("distance", make_unique<physics::distance_constraint_factory>());
	body_loader.register_factory("angle", make_unique<physics::angle_constraint_factory>());

	// add search directories	
	auto ldrs = loaders{&rendering::Texture::loader, &level_loader, &body_loader};
	ldrs.add_directory("../resources");
	ldrs.add_directory("./resources");

	// load level
	const auto level = level_loader.load("example_level");
	level->set_tile_size(0.5f);
	level->initialize_graphics(renderer);

	// initialize physics
	physics::physics_manager physics;
	physics.register_constraint_types<
		physics::distance_constraint, physics::angle_constraint
	>();

	physics.set_level(level.get());

	// prepare player
	//	1. physics
	const auto player_body_template = body_loader.load("cube.bdy");
	auto* player = physics.add_body(*player_body_template, { 16.f, 28.f }, 0.1f);

	//	2. rendering
	auto player_entity = renderer.register_entity();
	rendering::Texture kobold("brick_gray0\\big_kobold.png");
	player_entity->set_Texture(kobold);
	player_entity->set_Position(player->position);

	// setup camera
	rendering::Camera cam("default_cam",
		resolution,
		20.f);
	cam.set_position(player->position);

	// do neccessary gl setup
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// game loop
	while(handle_sdl_events())
	{
		const auto dt = 1.f / 60.f;

		// read input speed
		player->velocity += get_input_speed(2.f, dt);

		// do physics
		physics.integrate(dt);

		// pan camera to player
		const auto old_cam_pos = cam.get_position();
		cam.set_position(mix(player->position, old_cam_pos, 0.9f));

		// move player sprite to physics object
		player_entity->set_Position(player->position - glm::vec2(0.5));

		// draw everything
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.draw(cam);

		SDL_GL_SwapWindow(renderer.get_window());
	}
}
