#include "core/rendering/render_manager.h"

#include "core/utility/sdl.h"
#include "core/utility/helper.h"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <iostream>
#include <tuple>
#include <limits>

#include <boost/align/aligned_allocator.hpp>
#include "core/system/program.h"


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
		default_shader.attach
		({
			{ "..\\resources\\shader\\textured_quad_vertex.glsl",GL_VERTEX_SHADER },
			{ "..\\resources\\shader\\textured_quad_fragment.glsl",GL_FRAGMENT_SHADER }
		});
		default_shader.link();

		default_shader.logUnfiformslogAttributes();
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

		if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
		if(type == STATIC)
			freshly_added_static_entities.push_back(ptr);
		 else
			dynamic_entities.push_back(ptr);
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
			return wptr.expired();
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

	void render_manager::set_spritmap_program(ShaderProgram& program)
	{
		spriteprog = &program;
	}

	void render_manager::draw(Camera& cam)
	{
		//Draw dynamic entities todo
		for(auto& dynamic : dynamic_entities)
		{
			if(!dynamic.expired())
			{
				auto& dynamic_entity = *dynamic.lock();
				DrawcallEntity drawer(dynamic_entity.get_ShaderProgram(),
					dynamic_entity.get_Texture(), dynamic_entity.get_Multibuffer());

				drawer.add_entity_data(dynamic_entity);
				drawer.upload();
				drawer.draw(cam);

			}
		}

		//Draw static entities
		auto removed_statics = remove_expired_entities(static_entities);
		remove_expired_entities(freshly_added_static_entities);

		sort_entities_by_mesh(static_entities);
		sort_entities_by_mesh(freshly_added_static_entities);

		if(freshly_added_static_entities.size() > 0)
		{
			auto mesh_blocks = gen_blocks<shared_ptr<Multibuffer>>(
				freshly_added_static_entities,
				[](const RenderableEntity& e)
			{
				return e.get_Multibuffer().lock();
			}
				);

			for (auto& block : mesh_blocks)
			{

				auto buffer_ptr = std::get<0>(block);
				const size_t start_idx = std::get<1>(block);
				const size_t num_instances = std::get<2>(block);
								
				LOG(INFO) << "Multibufferptr = " << buffer_ptr;

				std::map<size_t, TextureBase*> found_textures;
				for (auto i = start_idx; i < start_idx + num_instances; ++i)
				{
					auto& entity = *(freshly_added_static_entities.at(i)).lock();
					auto* tex = &(*entity.get_Texture().lock());
					found_textures[tex->get_unique_id()] = tex;
				}

				auto_spritemaps.push_back(std::make_shared<Spritemap>(512));
				auto& sm_ptr = auto_spritemaps.back();
				auto& sm = *(sm_ptr);

				std::map<size_t, glm::ivec2> texture_offsets;
				for (auto& tex : found_textures)
				{
					auto* texbaseptr = tex.second;
					texture_offsets[texbaseptr->get_unique_id()] = sm.add_texture(*spriteprog, *texbaseptr);
				}
				
				auto it = std::find_if(begin(drawers), end(drawers),
					[&](const DrawcallEntity& drawer)
				{
					return drawer.get_Multibuffer().lock() == buffer_ptr;
				}
				);

				DrawcallEntity* drawer_ptr = nullptr;

				if (it == drawers.end())
				{
					//not found, add
					auto& entity = *(freshly_added_static_entities.at(start_idx)).lock();

					drawers.emplace_back(
						entity.get_ShaderProgram(),
						sm_ptr,
						entity.get_Multibuffer()
					);

					drawer_ptr = &drawers.back();
				} else
				{
					//found
					drawer_ptr = &(*it);
				}

				if (drawer_ptr == nullptr) continue;
				auto& drawer = *drawer_ptr;
				for (auto i = start_idx; i < start_idx + num_instances; ++i)
				{
					auto& entity = *(freshly_added_static_entities.at(i)).lock();

					auto unique_id = entity.get_Texture().lock()->get_unique_id();
					auto* tex = found_textures[unique_id];

					drawer.add_entity_data(entity,
						texture_offsets[unique_id],
						tex->get_resolution(),
						sm.get_resolution()
					);
				}

				drawer.upload();
			}
		}

		freshly_added_static_entities.clear();

		for(auto& drawcall : drawers)
		{
			drawcall.draw(cam);
		}
	}
} /* rendering */ } /* core */ } /* eversim */