#include "core\rendering\vertex_shader.h"

#include <GL/glew.h>

#include <string>

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			VertexShader::VertexShader(const std::string& name) : AttachableShader(name)
			{
			}


			VertexShader::~VertexShader()
			{
			}


			void VertexShader::create(const std::string& filename)
			{
				id = glCreateShader(GL_VERTEX_SHADER);

				compile(filename);

				created = true;
			}
		}
	}
}