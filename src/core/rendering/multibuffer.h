#pragma once

#include "core/utility/plattform.h"

#include <easylogging++.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <map>
#include <utility>

namespace eversim {
	namespace core {
		namespace rendering {
			class Multibuffer
			{
			public:
				std::vector<std::vector<glm::fvec2>> arrays2;
				std::vector<std::vector<glm::fvec3>> arrays3;
				std::vector<std::vector<glm::fvec4>> arrays4;
				std::vector<std::pair<size_t,size_t>> metadata;

				GLenum Type = 0;
				GLuint VaoId = 0;
				GLuint *VboIds;
				GLenum type, first, count;

			public:
				std::string name;

				Multibuffer() = delete;
				explicit Multibuffer(const std::string& name);

				Multibuffer(const Multibuffer& other) = delete;
				Multibuffer(Multibuffer&&) = delete;

				~Multibuffer();

				Multibuffer& operator=(const Multibuffer& other) = delete;
				Multibuffer& operator=(Multibuffer&& other) = delete;

				void create();
				void upload();
				void create_and_upload() { create(); upload(); }
				void set_draw_mode(GLenum type, GLint first, GLint count);
				void bind() const;
				void draw() const;
				void bind_and_draw() const;
				void attach(const std::vector<glm::fvec2>&);
				void attach(std::vector<glm::fvec2>&&);
				void attach(const std::vector<glm::fvec3>&);
				void attach(std::vector<glm::fvec3>&&);
				void attach(const std::vector<glm::fvec4>&);
				void attach(std::vector<glm::fvec4>&&);
			};

		}
	}
}