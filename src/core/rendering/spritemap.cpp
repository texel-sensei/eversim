#include "core/rendering/spritemap.h"

#include <algorithm>

using namespace eversim::core::utility;

namespace eversim {
	namespace core {
		namespace rendering {

			ShaderProgram Spritemap::program = ShaderProgram("const_spritemap_shader");

			constexpr auto vertex_shader =
			R"#delim#(
				#version 440 core

				void main()
				{
				}
			)#delim#";

			constexpr auto geometry_shader =
			R"#delim#(
				#version 440 core

				layout(points) in;
				layout(triangle_strip, max_vertices = 4) out;

				out vec2 texcoord;

				uniform vec2 target_resolution;
				uniform vec2 texture_size; //Texture size
				uniform mat4 M;

				vec2 allign(vec2 v,vec2 s)
				{
					v /= s;
					v *= 2.0;
					v -= vec2(1.0);
					return v;
				}

				vec2 move(vec2 v)
				{
					vec4 p = M * vec4(v,0,1);
					return p.xy/p.w;
				}

				void main() 
				{
					float tw = texture_size.x;
					float th = texture_size.y;

					vec2 ur = allign( move(vec2(tw,th) ),target_resolution);
					vec2 ul = allign( move(vec2(0,th)),target_resolution);
					vec2 dr = allign( move(vec2(tw,0)),target_resolution);
					vec2 dl = allign( move(vec2(0,0)),target_resolution);
	
					gl_Position = vec4( ur, 0.5, 1.0 );
					texcoord = vec2( 1.0, 1.0 );
					EmitVertex();

					gl_Position = vec4(ul, 0.5, 1.0 );
					texcoord = vec2( 0.0, 1.0 ); 
					EmitVertex();

					gl_Position = vec4( dr, 0.5, 1.0 );
					texcoord = vec2( 1.0, 0.0 ); 
					EmitVertex();

					gl_Position = vec4(dl, 0.5, 1.0 );
					texcoord = vec2( 0.0, 0.0 ); 
					EmitVertex();

					EndPrimitive(); 
				}
			)#delim#";

			constexpr auto fragment_shader =
			R"#delim#(
				#version 440 core

				uniform sampler2D tex;

				in vec2 texcoord;
				out vec4 FragColor;

				void main()
				{
					FragColor = texture(tex,texcoord);
				}
			)#delim#";

			void Spritemap::init_shader()
			{
				program.create();
				program.attach<std::istream&>
				({
					{ std::istringstream(vertex_shader),GL_VERTEX_SHADER },
					{ std::istringstream(geometry_shader),GL_GEOMETRY_SHADER },
					{ std::istringstream(fragment_shader),GL_FRAGMENT_SHADER }
				});
				program.link();
			}

			Spritemap::Spritemap()
			{
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &resolution[0]);
				resolution[1] = resolution[0];
				const size_t max_size = 512;
				if (resolution[0] > max_size)
					resolution = { max_size,max_size };
				init();
			}

			Spritemap::Spritemap(const size_t res) : resolution({res,res})
			{
				init();
			}

			void Spritemap::init()
			{
				canvas_tex.init(resolution);
				canvas_tex.clear({ 1.f,1.f,1.f,0.f });
				divider = Areadivider(resolution);
				set_unique_id(canvas_tex.get_texture_id());
			}

			glm::ivec2 Spritemap::add_texture(TextureBase& tex)
			{
				auto pos = divider.place_rectangle(tex.get_resolution());
				if (pos != glm::ivec2(-1)) {
					canvas_tex.place_texture(program, tex, pos, glm::vec2(1, 1));
					placed_textures.emplace_back(pos,tex.get_resolution(),tex.get_unique_id());
				}
				
				return pos;
			}

			bool  Spritemap::contains_texture(const GLuint& uid) const
			{
				auto it = find_if(begin(placed_textures), end(placed_textures), 
					[uid](const placed_texture& pt) { return pt.uid == uid; }
				);
				return it != end(placed_textures);
			}

			void Spritemap::bind() const
			{
				glBindTexture(GL_TEXTURE_2D, canvas_tex.get_texture_id());
			}

			glm::ivec2 Spritemap::get_resolution() const
			{
				return resolution;
			}
		}
	}
}
