#include "core/utility/area.h"

namespace eversim {
	namespace core {
		namespace utility {
		
			Area::Area(const glm::ivec2& min_pos, const glm::ivec2& max_pos) :
				min(min_pos), max(max_pos)
			{
				auto a = max - min;
				size_area = (a[0] + 1) * (a[1] + 1);
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
				float size_smaller = (size[0] - 1) * (size[1] - 1);
				float ratio = size_area / size_smaller;

				auto diff = (max - min) - (size - glm::ivec2(1, 1));
				if (diff[0] < 0.f || diff[1] < 0.f) ratio = 0.f;

				return ratio;
			}

			bool Area::operator==(const Area& other) const
			{
				return (min == other.min)
					&& (max == other.max);
			}

		}
	}
}