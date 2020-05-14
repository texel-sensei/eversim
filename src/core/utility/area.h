#pragma once

#include <glm/glm.hpp>

#include <iostream>
#include <vector>

namespace eversim {
	namespace core {
		namespace utility {

			class Area {
			public:
				glm::ivec2 min, max;
				float size_area;

				explicit Area(const glm::ivec2& size);
				explicit Area(const glm::ivec2& min_pos, const glm::ivec2& max_pos);

				bool operator==(const Area& other) const;

				std::vector<Area> place_rectangle(const glm::ivec2& size);
				float if_it_fits_i_sits(const glm::ivec2& size) const;
			};

			std::ostream& operator<<(std::ostream& out, const Area& a);

			class Areadivider
			{
			private:
				std::vector<Area> areas;
			public:
				Areadivider(){}
				explicit Areadivider(const Area& a);
				explicit Areadivider(const glm::ivec2& size);
				glm::ivec2 place_rectangle(const glm::ivec2& size);
			};

		}
	}
}
