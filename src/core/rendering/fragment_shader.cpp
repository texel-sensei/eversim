#include "core\rendering\fragment_shader.h"

using namespace std;

namespace eversim {
	namespace core {
		namespace rendering {
			FragmentShader::FragmentShader(const std::string& name) : AttachableShader(name)
			{
			}


			FragmentShader::~FragmentShader()
			{
			}

			void FragmentShader::create(const std::string& filename)
			{
				id = glCreateShader(GL_FRAGMENT_SHADER);

				compile(filename);

				created = true;
			}
		}
	}
}