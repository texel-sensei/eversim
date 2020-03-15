#include "core/rendering/vertex_shader.h"

#include <GL/glew.h>

#include <string>

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			VertexShader::VertexShader(const std::string& name) : AttachableShader(name)
			{
				id = glCreateShader(GL_VERTEX_SHADER);
			}


			VertexShader::~VertexShader()
			{
			}


			void VertexShader::create(const std::string& filename)
			{
				compile(filename);

				created = true;
			}

			void VertexShader::create(std::istream& file)
			{
				compile(file);

				created = true;
			}
		}
	}
}
