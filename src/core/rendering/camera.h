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
				explicit Camera(const std::string&, 
					const glm::ivec2& resolution,
					const float width_in_meters,
					const glm::fvec2 position = glm::fvec2(0, 0));

				void use(ShaderProgram&);
				void translate(const glm::fvec2& t);
				void set_position(const glm::fvec2& p);
				glm::fvec2 get_position() const;
				void rotate(const float radians); //2D Game, so lets only rotate around z-axis
				void set_rotation(const float radians);
				void set_width_in_meters(const float m);

				void set_aspect_ratio(const float ar);
				void set_resolution(const glm::ivec2& resolution);

				float get_width_in_meters() const;
				float get_rotation() const;

				float get_aspect_ratio() const;
				std::string get_name() const;

				glm::fmat3 get_projection_matrix() const;
				glm::fmat3 get_view_matrix() const;
			private:
				const std::string name;
				glm::fmat3 P, V;
				glm::fvec2 up_vector = {0,1}, position;
				float width_in_meters, rotation, aspect_ratio;

				void calc();
				void calc_P();
				void calc_V();
			};
		}
	}
}