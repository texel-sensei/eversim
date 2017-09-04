#include "core\rendering\geometry_shader.h"

#include <string>

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			GeometryShader::GeometryShader(const std::string& name) : AttachableShader(name)
			{
				id = glCreateShader(GL_GEOMETRY_SHADER);
			}


			GeometryShader::~GeometryShader()
			{
			}


			void GeometryShader::create(const std::string& filename)
			{
				compile(filename);

				created = true;
			}

			void GeometryShader::create(std::istream& file)
			{
				compile(file);

				created = true;
			}
		}
	}
}