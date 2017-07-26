#pragma once


#include "core/utility/plattform.h"
#include "core/rendering/shader_program.h"

#include <easylogging++.h>
#include <glm/glm.hpp>

#include <string>
#include <cmath>

namespace eversim {
	namespace core {
		namespace rendering {
			class Camera {
			public:
				const std::string name;
				const float aspect_ratio;

				explicit Camera(const std::string&, 
					const glm::fvec2 left_right,
					const glm::fvec2 bottom_top,
					const float width_in_meters,
					const glm::fvec2 position = glm::fvec2(0, 0));

				void use(ShaderProgram&);
				void translate(const glm::fvec2& t);
				void rotate(const float angle); //2D Game, so lets only rotate around z-axis
				void set_width_in_meters(const float m);
				float get_width_in_meters() const;
			private:
				glm::fmat3 P, V;
				glm::fvec2 left_right, bottom_top, up_vector, right_vector, position;
				float width_in_meters;

				void calc();
			};
		}
	}
}