#include "core\rendering\geometry_shader.h"

#include <string>

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			GeometryShader::GeometryShader(const std::string& name) : AttachableShader(name)
			{
			}


			GeometryShader::~GeometryShader()
			{
			}


			void GeometryShader::create(const std::string& filename)
			{
				id = glCreateShader(GL_GEOMETRY_SHADER);

				compile(filename);

				created = true;
			}
		}
	}
}