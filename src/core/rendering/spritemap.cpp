#include "core/rendering/spritemap.h"

#include <algorithm>

using namespace eversim::core::utility;

namespace eversim {
	namespace core {
		namespace rendering {
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

			bool Spritemap::add_texture(ShaderProgram& program, Texture& tex)
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
							
						return true;
					}
				}
				return false;
				
			}
		}
	}
}
