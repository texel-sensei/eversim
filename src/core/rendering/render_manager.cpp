#include "core/rendering/render_manager.h"

#include "core/utility/sdl.h"
#include "core/utility/helper.h"
#include "core/rendering/spritemap.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <boost/align/aligned_allocator.hpp>

#include <algorithm>
#include <iostream>
#include <tuple>
#include <limits>

#include <boost/align/aligned_allocator.hpp>


using namespace std;
using namespace glm;

#undef max

namespace eversim { namespace core { namespace rendering {

	class render_manager* myrenderer;
	void draw_line(glm::vec2 a, glm::vec2 b, int dur)
	{
		myrenderer->draw_line(a, b, dur);
	}
	void draw_point(glm::vec2 p)
	{
		myrenderer->draw_point(p);
	}

	render_manager::render_manager(glm::ivec2 resolution, bool fullscreen) :
		resolution(resolution),
		default_buffer_ptr	(make_shared<Multibuffer>("default quad mesh")),
		default_texture_ptr	(make_shared<Texture>(ivec2(4, 4))),
		default_shader_ptr	(make_shared<ShaderProgram>("default uv shader"))
	{
		setup(fullscreen);
	
		auto& default_buffer = *default_buffer_ptr;
		default_buffer.attach(
		{
			{ 1,1 },
			{ 0,1 },
			{ 0,0 },
			{ 1,0 },
		}
		);
		default_buffer.attach(
		{
			{ 1,1 },
			{ 0,1 },
			{ 0,0 },
			{ 1,0 }
		}
		);
		default_buffer.set_draw_mode(GL_QUADS, 0, 4);
		default_buffer.create_and_upload();

		auto& default_shader = *default_shader_ptr;
		default_shader.create();

		default_shader.attach("textured_quad_vertex.glsl", GL_VERTEX_SHADER);
		default_shader.attach("textured_quad_fragment.glsl", GL_FRAGMENT_SHADER);

		default_shader.link();

		default_shader.logUnfiformslogAttributes();

		Spritemap::init_shader();
	}

	void render_manager::draw_line(glm::vec2 a, glm::vec2 b, int dur)
	{
		lines.push_back({ a, b, dur });
	}
	void render_manager::draw_point(glm::vec2 p)
	{
		points.emplace_back(p);
	}

	void render_manager::do_draw(Camera const& cam)
	{
		const auto V = cam.get_view_matrix();
		const auto P = cam.get_projection_matrix();

		const auto VP = P * V;

		glPointSize(5);
		glBegin(GL_POINTS);
		for(auto const& p : points)
		{
			auto proj = VP * glm::vec3(p, 1);
			proj /= proj.z;

			glColor3f(1, 1, 1);
			glVertex2f(proj.x, proj.y);
		}
		glEnd();

		
		glBegin(GL_LINES);
		for(auto& l : lines)
		{
			auto a = VP * glm::vec3(l.a, 1);
			a /= a.z;
			auto b = VP * glm::vec3(l.b, 1);
			b /= b.z;
			glColor3f(1, 0, 0);
			glVertex2f(a.x, a.y);
			glVertex2f(b.x, b.y);
			l.dur--;
		}
		glEnd();
		auto zero_dur = [](auto const& l) { return l.dur <= 0; };
		lines.erase(
			std::remove_if(lines.begin(), lines.end(), zero_dur),
			lines.end()
		);
		points.clear();
	}

	void render_manager::setup(bool fullscreen)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			cerr << "SDL_Init Error: " << SDL_GetError();
			throw sdl::sdl_error{ "Failed to init SDL!" };
		}
		quit_sdl = &SDL_Quit;

		auto flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if(fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		window = sdl::make_window(
			"eversim",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			resolution.x, resolution.y,
			flags
		);

		context = SDL_GL_CreateContext(window);

		SDL_GL_MakeCurrent(window, context);

		glewExperimental = GL_TRUE;
		auto res = glewInit();
		if (res != GLEW_OK) {
			cerr << "Failed to init glew!";
			cerr << glewGetErrorString(res);
			throw sdl::sdl_error{ "Failed to init GLEW!" };
		}

	}

	entity_shptr render_manager::add_entity(const entity_type type)
	{
		auto ptr = entity_shptr(new RenderableEntity(
			default_shader_ptr, default_texture_ptr, default_buffer_ptr,type));
		dirty_entities.push_back(ptr);
		return ptr;
	}

	texture_shptr  render_manager::add_texture(const std::string& path)
	{
		auto ptr = std::make_shared<Texture>(path);
		textures.push_back(ptr);
		return ptr;
	}

	texture_shptr  render_manager::add_texture(const std::string& path,
		filter filtering)
	{
		auto ptr = std::make_shared<Texture>(path,filtering);
		textures.push_back(ptr);
		return ptr;
	}

	spritemap_shptr  render_manager::add_spritemap(const size_t res)
	{
		auto ptr = std::make_shared<Spritemap>(res);
		spritemaps.push_back(ptr);
		return ptr;
	}

	spritemap_shptr  render_manager::add_spritemap(const size_t res,
		filter filtering)
	{
		LOG(ERROR) << "TODO spritemap with filterfunction";
		auto ptr = std::make_shared<Spritemap>(res);
		spritemaps.push_back(ptr);
		return ptr;
	}

	size_t render_manager::remove_expired_entities(std::vector<entity_wkptr>& es)
	{
		auto size = es.size();
		auto end_ptr = std::remove_if(begin(es), end(es),
			[](entity_wkptr& wptr)
		{

			if (wptr.expired()) return true;

			return wptr.lock()->get_ShaderProgram().expired() ||
				wptr.lock()->get_Multibuffer().expired();
		});

		es = std::vector<entity_wkptr>(begin(es), end_ptr);
		return size - es.size();
	}

	void render_manager::sort_entities_by_shader(std::vector<entity_wkptr>& es)
	{
		std::sort(begin(es), end(es),
			[&](entity_wkptr& a, entity_wkptr& b)
		{
			auto& ra = *(a.lock());
			auto& rb = *(b.lock());

			if (ra.get_ShaderProgram().expired() || rb.get_ShaderProgram().expired())
				return false;

			return ra.get_ShaderProgram().lock()->getID() < rb.get_ShaderProgram().lock()->getID();
		});
	}

	void render_manager::sort_entities_by_mesh(std::vector<entity_wkptr>& es)
	{
		std::sort(begin(es), end(es),
			[&](entity_wkptr& a, entity_wkptr& b)
		{
			auto& ra = *(a.lock());
			auto& rb = *(b.lock());

			if (ra.get_Multibuffer().expired() || rb.get_Multibuffer().expired())
				return false;

			return ra.get_Multibuffer().lock() < rb.get_Multibuffer().lock();
		});
	}

	void render_manager::draw(Camera& cam)
	{

		auto entity_fits_to_drawer = []
		(shared_ptr<DrawcallEntity>& d, RenderableEntity& e)
		{
			return
				(&*(d->get_Multibuffer().lock()) == &*(e.get_Multibuffer().lock())) &&
				(d->get_ShaderProgram().lock()->getID() == e.get_ShaderProgram().lock()->getID())
				;
		};

		auto add_entity = []
		(shared_ptr<DrawcallEntity>& d, weak_ptr<RenderableEntity>& e)
		{
			auto idx = d->add_entity(e);
			e.lock()->set_Drawer(d, idx);
		};

		remove_expired_entities(dirty_entities);

		//move every touched entity to other drawer if it does not fit anymore
		for(auto& drawer_ptr : static_drawers)
		{
			auto& drawer = *drawer_ptr;

			const auto& touched_entities = drawer.get_touched_entities();

			vector<entity_wkptr> entities;

			for(auto idx : touched_entities)
			{
				entities.push_back(drawer.get_entity(idx));
			}

			for(auto entity_ptr : entities)
			{
				if(!entity_ptr.expired())
				{
					auto idx = entity_ptr.lock()->get_Drawer_idx();
					auto& entity = *entity_ptr.lock();
					if(!entity_fits_to_drawer(drawer_ptr,entity) ||
						!drawer.contains_texture(entity.get_Texture().lock()->get_unique_id())
						)
					{
						drawer_ptr->remove_entity(idx);
						dirty_entities.push_back(entity_ptr);
					}
				}
			}
		}
		if(dirty_entities.size() > 0)
		{
			//Sort lexicographicly
			sort(begin(dirty_entities), end(dirty_entities),
				[](entity_wkptr aptr, entity_wkptr bptr) {
					auto& a = *aptr.lock();
					auto& b = *bptr.lock();

					auto amptr = &*(a.get_Multibuffer().lock());
					auto bmptr = &*(b.get_Multibuffer().lock());

					auto shader_id_a = a.get_ShaderProgram().lock()->getID();
					auto shader_id_b = b.get_ShaderProgram().lock()->getID();

					if (amptr == bmptr) {
						return shader_id_a < shader_id_b;
					}
					return amptr < bmptr;
				}
			);

			std::vector<std::pair<size_t, size_t>> mesh_blocks;
			std::shared_ptr<DrawcallEntity> drawer_ptr = nullptr;

			for (auto& wkptr : dirty_entities)
			{
				auto& entity = *wkptr.lock();
				
				if(drawer_ptr != nullptr && entity_fits_to_drawer(drawer_ptr,entity))
				{
					add_entity(drawer_ptr,wkptr);
				} else
				{
					auto it = std::find_if(begin(static_drawers), end(static_drawers),
						[&](std::shared_ptr<DrawcallEntity>& drawer)
					{
						return entity_fits_to_drawer(drawer, entity);
					}
					);

					if(it == end(static_drawers))
					{
						static_drawers.push_back(make_shared<DrawcallEntity>(
							entity.get_ShaderProgram(),
							entity.get_Multibuffer())
						);
						drawer_ptr = static_drawers.back();
					} else
					{
						drawer_ptr = *it;
					}


					add_entity(drawer_ptr, wkptr);	
				}
			}
		}

		dirty_entities.clear();
		
		for(auto& drawcall : static_drawers)
		{
			drawcall->upload();
			drawcall->draw(cam);
		}
	}
} /* rendering */ } /* core */ } /* eversim */