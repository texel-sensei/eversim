#include "core/rendering/spritemap.h"

#include <algorithm>

namespace eversim {
	namespace core {
		namespace rendering {


			Area::Area(const glm::ivec2& min_pos, const glm::ivec2& max_pos) :
				min(min_pos), max(max_pos)
			{
				auto a = max - min;
				size_area = (a[0]+1) * (a[1]+1);
			}

			std::vector<Area> Area::place_rectangle(const glm::ivec2& size)
			{
				std::vector<Area> res;
				auto rec_area = max - min + glm::ivec2(1, 1);
				auto tex_area = size;
				//std::cout << rec_area[0] << "/" << rec_area[1] << std::endl;
				//std::cout << tex_area[0] << "/" << tex_area[1] << std::endl;
				if (tex_area == rec_area) {
					
					return {};
				}
				else if (tex_area[1] == rec_area[1])
				{

					glm::ivec2 ald(min[0] + tex_area[0], min[1]);
					glm::ivec2 adim;
					adim[1] = tex_area[1];
					adim[0] = max[0] - min[0] + 1 - tex_area[0];

					glm::ivec2 aru = ald + adim - glm::ivec2(1, 1);
					
					Area a(
						ald
						,
						aru
					);

					res.push_back(a);
				}
				else {


					glm::ivec2 ald(min[0] + tex_area[0], min[1]);
					glm::ivec2 adim;
					adim[1] = tex_area[1];
					adim[0] = max[0] - min[0] + 1 - tex_area[0];

					glm::ivec2 aru = ald + adim - glm::ivec2(1, 1);
					
					Area a(
						ald
						,
						aru
					);


					glm::ivec2 bld(min[0], min[1] + tex_area[1]);

					glm::ivec2 bdim;
					bdim[0] = max[0] - min[0] + 1;
					bdim[1] = max[1] - min[1] + 1 - tex_area[1];
					
					glm::ivec2 bru = bld + bdim - glm::ivec2(1, 1);

					Area b(
						bld, bru
					);

					res = { a,b };
				}
				return res;

			}

			float Area::if_it_fits_i_sits(const glm::ivec2& size) const
			{
				float size_smaller = (size[0]-1) * (size[1]-1);
				float ratio = size_area / size_smaller;

				auto diff = (max - min) - (size  - glm::ivec2(1,1));
				if (diff[0] < 0.f || diff[1] < 0.f) ratio = 0.f;

				return ratio;
			}

			bool Area::operator==(const Area& other) const
			{
				return (min == other.min)
						&& (max == other.max);
			}

			Spritemap::Spritemap() : offset({0,0})
			{
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &resolution[0]);
				resolution[1] = resolution[0];
				const size_t max_size = 512;
				if (resolution[0] > max_size)
					resolution = { max_size,max_size };
				LOG(INFO) << resolution[0] << "x" << resolution[1];
				canvas_tex.init(resolution);
				canvas_tex.clear();
				free_areas.emplace_back(glm::ivec2(0,0), resolution - glm::ivec2(1,1));
			}

			std::ostream& operator<<(std::ostream& out,const Area& a)
			{
				out << "(" << a.min[0] << "," << a.min[1]
					<< ") - (" << a.max[0] << "," << a.max[1] << ") area = " << a.size_area;
				return out;
			}

			void Spritemap::add_texture(ShaderProgram& program, Texture& tex)
			{
				
				std::vector<Area> free_area_copy(free_areas);

				std::sort(begin(free_area_copy), end(free_area_copy), 
					[&](const Area& a,const Area& b) 
				{
					float fits_a = a.if_it_fits_i_sits(tex.get_resolution());
					float fits_b = b.if_it_fits_i_sits(tex.get_resolution());
					return fits_a < fits_b;
				});

				/*LOG(INFO) << "Free Areas";
				for (auto& a : free_area_copy)
					LOG(INFO) << a << " " << a.if_it_fits_i_sits(tex.get_resolution());*/

				for (auto& area : free_area_copy)
				{
					float fits_area = area.if_it_fits_i_sits(tex.get_resolution());
					if (fits_area >= 1.f) {

						
						glm::ivec2 offset = area.min;

						/*LOG(INFO) << "texture fits area with " << fits_area << " place at (" <<
							offset[0] << "," << offset[1] << ")";*/

						canvas_tex.place_texture(program, tex, offset, glm::vec2(1, 1));

						auto endptr = std::remove_if(begin(free_areas), end(free_areas),
							[&](const Area &a) 
						{
							/*LOG(INFO) << "\ncompare\n" << a << "\nwith\n" << area
							<< "\nresult = " <<  (a == area);*/
							return a == area;
						});

						free_areas = std::vector<Area>(begin(free_areas),endptr);

						//LOG(INFO) << "size after remove_if = " << free_areas.size();

						auto free_parts = area.place_rectangle(tex.get_resolution());

						/*LOG(INFO) << "Free Parts";
						for (auto& a : free_areas)
							LOG(INFO) << a;*/

						for (auto& fp : free_parts) {
							free_areas.push_back(fp);
							/*canvas tmp;
							tmp.init((fp.max - fp.min + glm::ivec2(1, 1)));
							tmp.clear(glm::fvec4(1.f,0.f,0.f,1.f));*/
							//canvas_tex.place_texture(program, tmp., fp.min, glm::vec2(1, 1));
						}
							
						break;
					}
				}
				return;
				
			}
		}
	}
}
