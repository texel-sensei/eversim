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


using namespace std;

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
		resolution(resolution)
	{
		setup(fullscreen);
		default_quadmesh_ptr = std::make_unique<Multibuffer>("default quad mesh");
		auto& default_quadmesh = *default_quadmesh_ptr;
		default_quadmesh.attach(
		{
			{ 1,1 },
			{ 0,1 },
			{ 0,0 },
			{ 1,0 },
		}
		);
		default_quadmesh.attach(
		{
			{ 1,1 },
			{ 0,1 },
			{ 0,0 },
			{ 1,0 }
		}
		);
		default_quadmesh.set_draw_mode(GL_QUADS, 0, 4);
		default_quadmesh.create_and_upload();

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

	entity_shptr render_manager::register_entity(const entity_type type)
	{
		auto ptr = entity_shptr(new RenderableEntity);
		if(type == STATIC)
		{
			static_entities.push_back(ptr);
			freshly_added_static_entities.push_back(ptr);
			ptr->set_Type(STATIC);
		} else if (type == DYNAMIC)
		{
			dynamic_entities.push_back(ptr);
			ptr->set_Type(DYNAMIC);
		}
		return ptr;
	}

	texture_shptr  render_manager::register_texture(const std::string& path)
	{
		auto ptr = std::make_shared<Texture>(path);
		textures.push_back(ptr);
		return ptr;
	}

	texture_shptr  render_manager::register_texture(const std::string& path,
		filter filtering)
	{
		auto ptr = std::make_shared<Texture>(path,filtering);
		textures.push_back(ptr);
		return ptr;
	}

	spritemap_shptr  render_manager::register_spritemap(const size_t resolution)
	{
		auto ptr = std::make_shared<Spritemap>(resolution);
		spritemaps.push_back(ptr);
		return ptr;
	}

	spritemap_shptr  render_manager::register_spritemap(const size_t resolution,
		filter filtering)
	{
		LOG(ERROR) << "TODO spritemap with filterfunction";
		auto ptr = std::make_shared<Spritemap>(resolution);
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

			if (ra.get_ShaderProgram() == nullptr || rb.get_ShaderProgram() == nullptr)
				return false;

			return ra.get_ShaderProgram()->getID() < rb.get_ShaderProgram()->getID();
		});
	}

	void render_manager::sort_entities_by_mesh(std::vector<entity_wkptr>& es)
	{
		std::sort(begin(es), end(es),
			[&](entity_wkptr& a, entity_wkptr& b)
		{
			auto& ra = *(a.lock());
			auto& rb = *(b.lock());

			if (ra.get_Multibuffer() == nullptr || rb.get_Multibuffer() == nullptr)
				return false;

			return ra.get_Multibuffer() < rb.get_Multibuffer();
		});
	}

	void render_manager::draw(Camera& cam)
	{
		//getchar();

	/*	auto deref = [](entity_wkptr& wkptr)
		{
			auto sptr = wkptr.lock();
			return sptr;
		};

		auto removed_dynamics = remove_expired_entities(dynamic_entities);

		sort_entities_by_shader(dynamic_entities);
		
		auto blocks = gen_blocks<GLuint>(
			dynamic_entities,
			[](const RenderableEntity& e)
		{
			return e.get_ShaderProgram()->getID();
		}
			);
		LOG(INFO) << "number of drawable blocks = " << blocks.size();
		for(auto& block : blocks)
		{
			auto fbeptr = deref(dynamic_entities.at(std::get<1>(block)));
			auto& fbe = *fbeptr;
			ShaderProgram& program = *(fbe.program);
			program.use();
			//LOG(INFO) << "bind program " << program.name;
			cam.use(program);
			for(auto i = std::get<1>(block); i < std::get<1>(block) + std::get<2>(block) ;++i)
			{
				RenderableEntity& entity = *(dynamic_entities.at(i).lock());

				auto location = glGetUniformLocation(program.getID(), "M");
				if (location == -1)
					LOG(INFO) << "Uniform name ""M"" does not exist";
				glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(entity.get_M()));

				entity.bind();
				entity.draw();
			}

			glUseProgram(0);
		}*/

		//Draw static entities
		auto removed_statics = remove_expired_entities(static_entities);
		remove_expired_entities(freshly_added_static_entities);

		sort_entities_by_mesh(static_entities);
		sort_entities_by_mesh(freshly_added_static_entities);

		if(freshly_added_static_entities.size() > 0)
		{
			auto mesh_blocks = gen_blocks<Multibuffer*>(
				freshly_added_static_entities,
				[](const RenderableEntity& e)
			{
				return e.get_Multibuffer();
			}
				);

			for (auto& block : mesh_blocks)
			{

				Multibuffer* buffer_ptr = std::get<0>(block);
				const size_t start_idx = std::get<1>(block);
				const size_t num_instances = std::get<2>(block);

				auto& fbe = *(freshly_added_static_entities.at(start_idx).lock());
				
				LOG(INFO) << "Multibufferptr = " << buffer_ptr;

				auto it = ssbs.find(buffer_ptr);

				if(it != ssbs.end())
				{
					//found, but changed ! TODO
				} else
				{
					//not found, add :)
					ssbs[buffer_ptr] = shader_storage_buffer();
				}

				auto& ssb = (ssbs.find(buffer_ptr))->second;

				std::vector<instanced_entity_information,boost::alignment::aligned_allocator<instanced_entity_information,16>> matrices;
				for (auto i = start_idx; i < start_idx + num_instances; ++i)
				{
					auto& entity = *(freshly_added_static_entities.at(i)).lock();
					matrices.emplace_back();
					auto& entity_information = matrices.back();
					entity.get_instanced_entity_information(entity_information);
				}

				utility::byte_array_view matrices_view = matrices;
				ssb = shader_storage_buffer(matrices_view);

			}
		}

		freshly_added_static_entities.clear();

		auto mesh_blocks = gen_blocks<Multibuffer*>(
			static_entities,
			[](const RenderableEntity& e)
		{
			return e.get_Multibuffer();
		}
		);

		//LOG(INFO) << mesh_blocks.size();
		//getchar();
		for (auto& block : mesh_blocks)
		{
			auto* buffer_ptr = std::get<0>(block);
			const auto start_idx = std::get<1>(block);
			const auto num_instances = std::get<2>(block);

			auto& fbe = *((static_entities.at(start_idx)).lock());

			auto& program = *(fbe.get_ShaderProgram());
			auto& texptr = *(fbe.get_Texture());
			auto& pointsuv = *(fbe.get_Multibuffer());

			program.use();
			cam.use(program);

			/*LOG(INFO) << "drawing ssb with data from multibuffer " << buffer_ptr 
				<< " and " << num_instances << "#instances from index " << start_idx;*/

			auto& ssb = ssbs.at(buffer_ptr);

			pointsuv.bind();
			ssb.bind(42);
			texptr.bind();
			
			glDrawArraysInstanced(pointsuv.type, 0, 4, num_instances);

			glUseProgram(0);
		}
	}
} /* rendering */ } /* core */ } /* eversim */