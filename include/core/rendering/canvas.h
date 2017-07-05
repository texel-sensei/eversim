#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace eversim {
	namespace core {
		namespace rendering {

			class canvas {
			private:
				glm::ivec2 resolution;
				glm::f32mat4 M;
				GLuint tex;
				std::vector<canvas*> children;
			public:
				explicit canvas();
				/*
				Init canvas with empty texture of size ivec2
				at position defined by the modelmatrix M
				*/
				void init(const glm::ivec2& resolution,
					const glm::f32mat4& M);
				/*
				Init canvas with texture defined by path
				at position defined by the modelmatrix M
				*/
				void init(const std::string& path,
					const glm::f32mat4& M);
				void add_child(canvas*);

			};

		}
	}
}