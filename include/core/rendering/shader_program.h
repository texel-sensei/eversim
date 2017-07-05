#pragma once

#include "core\rendering\attachable_shader.h"
#include "core\rendering\vertex_shader.h"
#include "core\rendering\fragment_shader.h"
#include "core\rendering\geometry_shader.h"

#include <string>
#include <vector>
#include <utility>

namespace eversim {
	namespace core {
		namespace rendering {
			class ShaderProgram
			{
			private:
				GLuint id;

			public:
				const std::string name;

				ShaderProgram(const std::string& name);
				~ShaderProgram();

				void attach(const AttachableShader& shader);
				void attach(const std::string& filename,const GLenum TYPE);
				void attach(const std::vector<
					std::pair<std::string,GLenum>
							>& shaders);
				void create();
				void link();
				void use() const;
				GLuint getID() const { return id; }
			};
		}
	}
}